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
    "normal_only_visible",
    ROOT / "tools" / "apply_motor_visible_package_overlay.py",
)
timing = load_module(
    "normal_only_timing",
    ROOT / "tools" / "apply_runtime_proven_motor_60s.py",
)
defender = load_module(
    "normal_only_defender",
    ROOT / "tools" / "apply_friendly_defender_motor_one_shot.py",
)
normal = load_module(
    "normal_transport_only",
    ROOT / "tools" / "apply_normal_transport_only.py",
)
base = load_module(
    "normal_transport_base_test",
    ROOT / "tools" / "apply_transport_control_comparison.py",
)

FILES = (
    "attack_support_waves.inc",
    "enemy_attack_support.inc",
    "defense_support_waves.inc",
    "faction_support_templates.inc",
    "dcg_vars.inc",
)


class NormalTransportOnlyTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        self.multi = self.root / "resource/map/multi"
        self.multi.mkdir(parents=True)
        for name in FILES:
            shutil.copy2(ROOT / "resource/map/multi" / name, self.multi / name)

        # Recreate the proven baseline plus the defender-side helper macros.
        visible.patch_multi_root(self.multi)
        timing.patch_multi_root(self.multi)
        defender.apply(self.root)

    def tearDown(self) -> None:
        self.temp.cleanup()

    def text(self, name: str) -> str:
        return (self.multi / name).read_text(encoding="utf-8-sig")

    def test_exactly_two_normal_transport_controls_remain_active(self) -> None:
        normal.apply(self.root)
        normal.validate(self.root)

        defense = self.text("defense_support_waves.inc")
        enemy = self.text("enemy_attack_support.inc")

        self.assertNotIn('{"defense_support/motor_test"', defense)
        self.assertNotIn('{"enemy_attack/motor_test"', enemy)
        self.assertEqual(
            defense.count('{"defense_support/motor_compare_normal_transport"'), 1
        )
        self.assertEqual(
            enemy.count('{"enemy_attack/motor_compare_normal_transport"'), 1
        )

    def test_controls_have_no_scripted_dismount_return_or_deletion(self) -> None:
        normal.apply(self.root)
        defense = self.text("defense_support_waves.inc")
        enemy = self.text("enemy_attack_support.inc")

        for text, begin, end in (
            (defense, base.FRIEND_BEGIN, base.FRIEND_END),
            (enemy, base.ENEMY_BEGIN, base.ENEMY_END),
        ):
            bounds = base.marked_bounds(text, begin, end)
            self.assertIsNotNone(bounds)
            assert bounds is not None
            block = text[bounds[0] : bounds[1]]
            self.assertEqual(block.count('{action advance}'), 1)
            self.assertIn('{"delay" {time 45}}', block)
            for forbidden in (
                '{"emit"',
                '{mode passengers}',
                'exit_motor_to_origin',
                'motor_cleanup',
                '{"delete"',
                '{"delay" {time 75}}',
            ):
                self.assertNotIn(forbidden, block)

    def test_enemy_scripted_motor_budget_is_disabled(self) -> None:
        normal.apply(self.root)
        enemy = self.text("enemy_attack_support.inc")
        self.assertEqual(
            enemy.count(
                '{"set_i" {var "enemy_attack_motor_left$"} {op "="} {value 0}}'
            ),
            1,
        )
        self.assertEqual(
            enemy.count(
                '{"set_i" {var "enemy_attack_motor_test$"} {op "="} {value 0}}'
            ),
            1,
        )

    def test_linked_friendly_fmtv_and_enemy_ural_packages_exist(self) -> None:
        normal.apply(self.root)
        templates = self.text("faction_support_templates.inc")
        self.assertEqual(templates.count('{Entity "fmtv" 0xb500'), 1)
        self.assertEqual(templates.count('{Entity "ural" 0xb510'), 1)
        for seat in range(1, 9):
            self.assertIn(f'{{0xb500 "seat{seat}"}}', templates)
            self.assertIn(f'{{0xb510 "seat{seat}"}}', templates)

    def test_friendly_attacker_engine_remains_byte_identical(self) -> None:
        before = (self.multi / "attack_support_waves.inc").read_bytes()
        normal.apply(self.root)
        after = (self.multi / "attack_support_waves.inc").read_bytes()
        self.assertEqual(before, after)

    def test_idempotent_and_check_only_is_read_only(self) -> None:
        before = {name: (self.multi / name).read_bytes() for name in FILES}
        predicted = normal.apply(self.root, check_only=True)
        self.assertEqual(
            {path.name for path in predicted},
            {
                "enemy_attack_support.inc",
                "defense_support_waves.inc",
                "faction_support_templates.inc",
                "dcg_vars.inc",
            },
        )
        self.assertEqual(
            before,
            {name: (self.multi / name).read_bytes() for name in FILES},
        )

        normal.apply(self.root)
        first = {name: (self.multi / name).read_bytes() for name in FILES}
        self.assertEqual(normal.apply(self.root), [])
        self.assertEqual(
            first,
            {name: (self.multi / name).read_bytes() for name in FILES},
        )


if __name__ == "__main__":
    unittest.main()
