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
    "motor_visible_overlay_75s_test",
    ROOT / "tools" / "apply_motor_visible_package_overlay.py",
)
timing = load_module(
    "runtime_motor_timing_75s_test",
    ROOT / "tools" / "apply_runtime_proven_motor_60s.py",
)
defender = load_module(
    "friendly_defender_motor_75s_test",
    ROOT / "tools" / "apply_friendly_defender_motor_one_shot.py",
)
correction = load_module(
    "motor_drive_origin_exit_75s_test",
    ROOT / "tools" / "apply_motor_drive_origin_exit_fixed.py",
)
tuning = load_module(
    "defense_motor_75s",
    ROOT / "tools" / "apply_defense_motor_75s.py",
)

FILES = (
    "attack_support_waves.inc",
    "enemy_attack_support.inc",
    "defense_support_waves.inc",
    "dcg_vars.inc",
)


class DefenseMotor75SecondTests(unittest.TestCase):
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
        correction.validate(self.root)

    def tearDown(self) -> None:
        self.temp.cleanup()

    def text(self, name: str) -> str:
        return (self.multi / name).read_text(encoding="utf-8-sig")

    def test_both_player_defense_trucks_dismount_at_75_seconds(self) -> None:
        changed = tuning.apply(self.root)
        self.assertEqual(
            {path.name for path in changed},
            {"enemy_attack_support.inc", "defense_support_waves.inc"},
        )
        tuning.validate(self.root)

        enemy = tuning.paren_block(
            self.text("enemy_attack_support.inc"), "ea_finish_motor"
        )[2]
        friendly = tuning.paren_block(
            self.text("defense_support_waves.inc"), "ds_finish_motor"
        )[2]

        self.assertEqual(enemy.count('{"delay" {time 2}}'), 1)
        self.assertEqual(enemy.count('{"delay" {time 73}}'), 1)
        self.assertNotIn('{"delay" {time 58}}', enemy)
        self.assertEqual(friendly.count('{"delay" {time 75}}'), 1)
        self.assertNotIn('{"delay" {time 60}}', friendly)

    def test_friendly_attacker_timing_remains_60_seconds(self) -> None:
        tuning.apply(self.root)
        attacker = tuning.paren_block(
            self.text("attack_support_waves.inc"), "as_finish_motor"
        )[2]
        self.assertEqual(attacker.count('{"delay" {time 60}}'), 1)
        self.assertNotIn('{"delay" {time 75}}', attacker)

    def test_package_and_passenger_emit_contracts_are_preserved(self) -> None:
        before_enemy = tuning.paren_block(
            self.text("enemy_attack_support.inc"), "ea_finish_motor"
        )[2]
        before_friendly = tuning.paren_block(
            self.text("defense_support_waves.inc"), "ds_finish_motor"
        )[2]

        tuning.apply(self.root)

        after_enemy = tuning.paren_block(
            self.text("enemy_attack_support.inc"), "ea_finish_motor"
        )[2]
        after_friendly = tuning.paren_block(
            self.text("defense_support_waves.inc"), "ds_finish_motor"
        )[2]

        self.assertEqual(before_enemy.count('{mode passengers}'), 1)
        self.assertEqual(after_enemy.count('{mode passengers}'), 1)
        self.assertEqual(before_friendly.count('{mode passengers}'), 1)
        self.assertEqual(after_friendly.count('{mode passengers}'), 1)
        self.assertIn('("ea_exit_motor_to_origin")', after_enemy)
        self.assertIn('("ds_exit_motor_to_origin")', after_friendly)

    def test_idempotent_and_check_only_is_read_only(self) -> None:
        before = {name: (self.multi / name).read_bytes() for name in FILES}
        predicted = tuning.apply(self.root, check_only=True)
        self.assertEqual(
            {path.name for path in predicted},
            {"enemy_attack_support.inc", "defense_support_waves.inc"},
        )
        self.assertEqual(
            before,
            {name: (self.multi / name).read_bytes() for name in FILES},
        )

        tuning.apply(self.root)
        first = {name: (self.multi / name).read_bytes() for name in FILES}
        self.assertEqual(tuning.apply(self.root), [])
        self.assertEqual(
            first,
            {name: (self.multi / name).read_bytes() for name in FILES},
        )


if __name__ == "__main__":
    unittest.main()
