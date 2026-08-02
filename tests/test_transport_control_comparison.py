from __future__ import annotations

import importlib.util
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec and spec.loader
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


visible = load_module(
    "motor_visible_overlay_compare_test",
    ROOT / "tools" / "apply_motor_visible_package_overlay.py",
)
timing = load_module(
    "runtime_motor_timing_compare_test",
    ROOT / "tools" / "apply_runtime_proven_motor_60s.py",
)
defender = load_module(
    "friendly_defender_motor_compare_test",
    ROOT / "tools" / "apply_friendly_defender_motor_one_shot.py",
)
correction = load_module(
    "motor_drive_origin_exit_compare_test",
    ROOT / "tools" / "apply_motor_drive_origin_exit_fixed.py",
)
tuning = load_module(
    "defense_motor_turnaround_compare_test",
    ROOT / "tools" / "apply_defense_motor_turnaround.py",
)
comparison = load_module(
    "transport_control_comparison_fixed_test",
    ROOT / "tools" / "apply_transport_control_comparison_fixed.py",
)

FILES = (
    "attack_support_waves.inc",
    "enemy_attack_support.inc",
    "defense_support_waves.inc",
    "faction_support_templates.inc",
    "dcg_vars.inc",
)


class TransportControlComparisonTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        self.multi = self.root / "resource/map/multi"
        self.multi.mkdir(parents=True)
        for name in FILES:
            shutil.copy2(ROOT / "resource/map/multi" / name, self.multi / name)

        visible.patch_multi_root(self.multi)
        timing.patch_multi_root(self.multi)
        defender.apply(self.root)
        correction.apply(self.root)
        tuning.apply(self.root)
        tuning.validate(self.root)

    def tearDown(self) -> None:
        self.temp.cleanup()

    def text(self, name: str) -> str:
        return (self.multi / name).read_text(encoding="utf-8-sig")

    def control_block(self, *, friendly: bool) -> str:
        text = self.text(
            "defense_support_waves.inc" if friendly else "enemy_attack_support.inc"
        )
        begin = comparison.FRIEND_BEGIN if friendly else comparison.ENEMY_BEGIN
        end = comparison.FRIEND_END if friendly else comparison.ENEMY_END
        bounds = comparison.marked_bounds(text, begin, end)
        assert bounds
        return text[bounds[0] : bounds[1]]

    def test_adds_exactly_two_independent_linked_control_packages(self) -> None:
        changed = comparison.apply(self.root)
        self.assertEqual(
            {path.name for path in changed},
            {
                "faction_support_templates.inc",
                "defense_support_waves.inc",
                "enemy_attack_support.inc",
                "dcg_vars.inc",
            },
        )
        comparison.validate(self.root)

        templates = self.text("faction_support_templates.inc")
        self.assertEqual(templates.count('{Entity "fmtv" 0xb500'), 1)
        self.assertEqual(templates.count('{Entity "ural" 0xb510'), 1)
        self.assertEqual(templates.count(comparison.PACKAGE_BEGIN), 1)
        self.assertEqual(templates.count(comparison.TAG_BEGIN), 1)
        self.assertEqual(templates.count('{MID 9900}'), 1)
        self.assertEqual(templates.count('{MID 9910}'), 1)
        self.assertEqual(templates.count('{MID 9920}'), 1)
        self.assertEqual(templates.count('{MID 9930}'), 1)
        for seat in range(1, 9):
            self.assertIn(f'{{0xb500 "seat{seat}"}}', templates)
            self.assertIn(f'{{0xb510 "seat{seat}"}}', templates)

    def test_controls_receive_only_normal_attack_orders(self) -> None:
        comparison.apply(self.root)
        for friendly in (True, False):
            block = self.control_block(friendly=friendly)
            self.assertEqual(block.count('{"delay" {time 45}}'), 1)
            self.assertEqual(block.count('{action advance}'), 1)
            self.assertEqual(block.count('{drop orders}'), 1)
            self.assertIn('{control AI}', block)
            self.assertIn('{ai_move {mode enable}}', block)
            for forbidden in (
                '{"emit"',
                '{mode passengers}',
                'motor_leaving',
                'exit_motor_to_origin',
                'motor_cleanup',
                '{"delete"',
            ):
                self.assertNotIn(forbidden, block)

    def test_control_sides_are_nato_defender_and_russian_attacker(self) -> None:
        comparison.apply(self.root)
        friendly = self.control_block(friendly=True)
        enemy = self.control_block(friendly=False)

        self.assertIn(
            '{var "faction_support_army$"} {op "=="} {value 3}',
            friendly,
        )
        self.assertIn('{var "id_defenderbot$"} {op ">"} {value 0}', friendly)
        self.assertIn('("ds_own_to_defenderbot")', friendly)
        self.assertIn('("ds_place_motor_visible")', friendly)

        self.assertIn(
            '{var "enemy_attack_army$"} {op "=="} {value 1}',
            enemy,
        )
        self.assertIn('{var "id_1st_enemy$"} {op ">"} {value 0}', enemy)
        self.assertIn('("ea_own_to_enemy")', enemy)
        self.assertIn('("ea_place_motor_visible")', enemy)

    def test_scripted_transport_paths_remain_present_beside_controls(self) -> None:
        comparison.apply(self.root)
        for filename, name in (
            ("defense_support_waves.inc", "ds_finish_motor"),
            ("enemy_attack_support.inc", "ea_finish_motor"),
        ):
            text = self.text(filename)
            block = tuning.paren_block(text, name)[2]
            self.assertEqual(block.count('{mode passengers}'), 1)
            self.assertEqual(block.count(tuning.PRETURN_MARKER), 1)
            self.assertEqual(block.count(tuning.REASSERT_MARKER), 1)

    def test_friendly_attacker_file_remains_unchanged(self) -> None:
        before = (self.multi / "attack_support_waves.inc").read_bytes()
        comparison.apply(self.root)
        self.assertEqual(before, (self.multi / "attack_support_waves.inc").read_bytes())

    def test_idempotent_and_check_only_is_read_only(self) -> None:
        before = {name: (self.multi / name).read_bytes() for name in FILES}
        predicted = comparison.apply(self.root, check_only=True)
        self.assertEqual(
            {path.name for path in predicted},
            {
                "faction_support_templates.inc",
                "defense_support_waves.inc",
                "enemy_attack_support.inc",
                "dcg_vars.inc",
            },
        )
        self.assertEqual(
            before,
            {name: (self.multi / name).read_bytes() for name in FILES},
        )

        comparison.apply(self.root)
        first = {name: (self.multi / name).read_bytes() for name in FILES}
        self.assertEqual(comparison.apply(self.root), [])
        self.assertEqual(
            first,
            {name: (self.multi / name).read_bytes() for name in FILES},
        )


if __name__ == "__main__":
    unittest.main()
