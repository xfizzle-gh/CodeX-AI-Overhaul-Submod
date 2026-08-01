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
    "motor_visible_overlay_exit_fixed_test",
    ROOT / "tools" / "apply_motor_visible_package_overlay.py",
)
timing = load_module(
    "runtime_motor_timing_exit_fixed_test",
    ROOT / "tools" / "apply_runtime_proven_motor_60s.py",
)
defender = load_module(
    "friendly_defender_motor_exit_fixed_test",
    ROOT / "tools" / "apply_friendly_defender_motor_one_shot.py",
)
correction = load_module(
    "motor_drive_origin_exit_fixed",
    ROOT / "tools" / "apply_motor_drive_origin_exit_fixed.py",
)

FILES = (
    "attack_support_waves.inc",
    "enemy_attack_support.inc",
    "defense_support_waves.inc",
    "dcg_vars.inc",
)


class MotorDriveOriginExitFixedTests(unittest.TestCase):
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
        visible.validate_multi_root(self.multi)
        timing.validate_multi_root(self.multi)
        defender.validate(self.root)

    def tearDown(self) -> None:
        self.temp.cleanup()

    def text(self, name: str) -> str:
        return (self.multi / name).read_text(encoding="utf-8-sig")

    @staticmethod
    def first_brace_block(text: str, marker: str) -> str:
        marker_at = text.find(marker)
        if marker_at < 0:
            raise AssertionError(f"missing {marker}")
        start = text.find("{", marker_at)
        if start < 0:
            raise AssertionError(f"missing brace after {marker}")
        end = correction.balanced(text, start, "{", "}", marker)
        return text[start:end]

    def test_applies_and_validates_all_three_live_paths(self) -> None:
        changed = correction.apply(self.root)
        self.assertEqual(
            {path.name for path in changed},
            {
                "attack_support_waves.inc",
                "enemy_attack_support.inc",
                "defense_support_waves.inc",
            },
        )
        correction.validate(self.root)

    def test_enemy_retry_preserves_sixty_second_total_ride(self) -> None:
        correction.apply(self.root)
        enemy = correction.paren_block(
            self.text("enemy_attack_support.inc"), "ea_finish_motor"
        )[2]
        self.assertIn(correction.RETRY_MARKER, enemy)
        self.assertEqual(enemy.count('{"delay" {time 2}}'), 1)
        self.assertEqual(enemy.count('{"delay" {time 58}}'), 1)
        self.assertNotIn('{"delay" {time 60}}', enemy)
        self.assertGreaterEqual(enemy.count('{action advance}'), 2)
        self.assertGreaterEqual(
            enemy.count('{target {ignore_captured_by_user 0} {tag ea_flag1}}'), 2
        )
        self.assertEqual(enemy.count('{mode passengers}'), 1)

    def test_origin_exit_mapping_replaces_waypoint_zero(self) -> None:
        correction.apply(self.root)
        expected = {
            "as": ("attack_support_entry_b", "attack_support_entry_a"),
            "ds": ("attack_support_entry_b", "attack_support_entry_a"),
            "ea": ("attack_support_entry_a", "attack_support_entry_b"),
        }

        for prefix, filename in correction.FILES.items():
            text = self.text(filename)
            finisher = correction.paren_block(
                text, correction.FINISHERS[prefix]
            )[2]
            helper = correction.paren_block(
                text, correction.EXIT_HELPERS[prefix]
            )[2]
            self.assertIn(f'("{correction.EXIT_HELPERS[prefix]}")', finisher)
            self.assertNotIn('{waypoint "0"}', finisher)
            self.assertIn(f'{{waypoint "{expected[prefix][0]}"}}', helper)
            self.assertIn(f'{{waypoint "{expected[prefix][1]}"}}', helper)

    def test_package_placement_and_emit_blocks_remain_identical(self) -> None:
        before_place = {
            "as": correction.paren_block(
                self.text("attack_support_waves.inc"), "as_place_motor_visible"
            )[2],
            "ea": correction.paren_block(
                self.text("enemy_attack_support.inc"), "ea_place_motor_visible"
            )[2],
            "ds": correction.paren_block(
                self.text("defense_support_waves.inc"), "ds_place_motor_visible"
            )[2],
        }
        before_emit = {}
        for prefix, filename in correction.FILES.items():
            finisher = correction.paren_block(
                self.text(filename), correction.FINISHERS[prefix]
            )[2]
            before_emit[prefix] = self.first_brace_block(finisher, '{"emit"')

        correction.apply(self.root)

        after_place = {
            "as": correction.paren_block(
                self.text("attack_support_waves.inc"), "as_place_motor_visible"
            )[2],
            "ea": correction.paren_block(
                self.text("enemy_attack_support.inc"), "ea_place_motor_visible"
            )[2],
            "ds": correction.paren_block(
                self.text("defense_support_waves.inc"), "ds_place_motor_visible"
            )[2],
        }
        self.assertEqual(before_place, after_place)

        for prefix, filename in correction.FILES.items():
            finisher = correction.paren_block(
                self.text(filename), correction.FINISHERS[prefix]
            )[2]
            self.assertEqual(
                before_emit[prefix],
                self.first_brace_block(finisher, '{"emit"'),
                prefix,
            )

    def test_idempotent_and_check_only_is_read_only(self) -> None:
        before = {name: (self.multi / name).read_bytes() for name in FILES}
        predicted = correction.apply(self.root, check_only=True)
        self.assertEqual(
            {path.name for path in predicted},
            {
                "attack_support_waves.inc",
                "enemy_attack_support.inc",
                "defense_support_waves.inc",
            },
        )
        self.assertEqual(
            before,
            {name: (self.multi / name).read_bytes() for name in FILES},
        )

        correction.apply(self.root)
        first = {name: (self.multi / name).read_bytes() for name in FILES}
        self.assertEqual(correction.apply(self.root), [])
        self.assertEqual(
            first,
            {name: (self.multi / name).read_bytes() for name in FILES},
        )


if __name__ == "__main__":
    unittest.main()
