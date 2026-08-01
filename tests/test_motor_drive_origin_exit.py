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
    "motor_visible_overlay_exit_test",
    ROOT / "tools" / "apply_motor_visible_package_overlay.py",
)
timing = load_module(
    "runtime_motor_timing_exit_test",
    ROOT / "tools" / "apply_runtime_proven_motor_60s.py",
)
defender = load_module(
    "friendly_defender_motor_exit_test",
    ROOT / "tools" / "apply_friendly_defender_motor_one_shot.py",
)
correction = load_module(
    "motor_drive_origin_exit",
    ROOT / "tools" / "apply_motor_drive_origin_exit.py",
)

FILES = (
    "attack_support_waves.inc",
    "enemy_attack_support.inc",
    "defense_support_waves.inc",
    "dcg_vars.inc",
)


class MotorDriveOriginExitTests(unittest.TestCase):
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

    def test_preserves_package_placement_and_passenger_emit(self) -> None:
        before = {
            "as_place": correction.paren_block(
                self.text("attack_support_waves.inc"), "as_place_motor_visible"
            )[2],
            "ea_place": correction.paren_block(
                self.text("enemy_attack_support.inc"), "ea_place_motor_visible"
            )[2],
            "ds_place": correction.paren_block(
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
        correction.validate(self.root)

        after = {
            "as_place": correction.paren_block(
                self.text("attack_support_waves.inc"), "as_place_motor_visible"
            )[2],
            "ea_place": correction.paren_block(
                self.text("enemy_attack_support.inc"), "ea_place_motor_visible"
            )[2],
            "ds_place": correction.paren_block(
                self.text("defense_support_waves.inc"), "ds_place_motor_visible"
            )[2],
        }
        self.assertEqual(before, after)

        for prefix, filename in correction.FILES.items():
            finisher = correction.paren_block(
                self.text(filename), correction.FINISHERS[prefix]
            )[2]
            self.assertEqual(
                before_emit[prefix],
                self.first_brace_block(finisher, '{"emit"'),
                prefix,
            )

    def test_enemy_retry_keeps_total_ride_at_sixty_seconds(self) -> None:
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

    def test_each_path_returns_to_its_own_entry_edge(self) -> None:
        correction.apply(self.root)

        expected = {
            "as": {
                1: "attack_support_entry_b",
                2: "attack_support_entry_a",
            },
            "ds": {
                1: "attack_support_entry_b",
                2: "attack_support_entry_a",
            },
            "ea": {
                1: "attack_support_entry_a",
                2: "attack_support_entry_b",
            },
        }

        for prefix, filename in correction.FILES.items():
            text = self.text(filename)
            helper = correction.paren_block(
                text, correction.EXIT_HELPERS[prefix]
            )[2]
            finisher = correction.paren_block(
                text, correction.FINISHERS[prefix]
            )[2]
            self.assertIn(f'("{correction.EXIT_HELPERS[prefix]}")', finisher)
            self.assertNotIn('{waypoint "0"}', finisher)
            for side, waypoint in expected[prefix].items():
                self.assertIn(
                    f'{{var "enemy_spawnside$"}} {{op "=="}} {{value {side}}}',
                    helper,
                )
                self.assertIn(f'{{waypoint "{waypoint}"}}', helper)

    def test_overlay_is_idempotent(self) -> None:
        correction.apply(self.root)
        first = {name: (self.multi / name).read_bytes() for name in FILES}
        changed = correction.apply(self.root)
        second = {name: (self.multi / name).read_bytes() for name in FILES}
        self.assertEqual(changed, [])
        self.assertEqual(first, second)

    def test_check_only_reports_without_writing(self) -> None:
        before = {name: (self.multi / name).read_bytes() for name in FILES}
        changed = correction.apply(self.root, check_only=True)
        after = {name: (self.multi / name).read_bytes() for name in FILES}
        self.assertEqual(
            {path.name for path in changed},
            {
                "attack_support_waves.inc",
                "enemy_attack_support.inc",
                "defense_support_waves.inc",
            },
        )
        self.assertEqual(before, after)


if __name__ == "__main__":
    unittest.main()
