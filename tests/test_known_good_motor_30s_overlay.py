from __future__ import annotations

import importlib.util
import shutil
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TIMING_SCRIPT = ROOT / "tools" / "apply_known_good_motor_30s_test.py"
PLACEMENT_SCRIPT = ROOT / "tools" / "apply_motor_visible_package_overlay.py"


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec and spec.loader
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


motor_30s = load_module("motor_30s", TIMING_SCRIPT)
motor_visible = load_module("motor_visible", PLACEMENT_SCRIPT)


class KnownGoodMotorThirtySecondOverlayTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.multi = Path(self.temp.name)
        for name in ("attack_support_waves.inc", "enemy_attack_support.inc"):
            shutil.copy2(ROOT / "resource" / "map" / "multi" / name, self.multi / name)

    def tearDown(self) -> None:
        self.temp.cleanup()

    def apply_all(self) -> None:
        motor_30s.patch_multi_root(self.multi)
        motor_visible.patch_multi_root(self.multi)
        motor_30s.validate_multi_root(self.multi)
        motor_visible.validate_multi_root(self.multi)

    def test_actual_known_good_files_become_visible_one_shot_30_second_tests(self) -> None:
        self.apply_all()

        attack = (self.multi / "attack_support_waves.inc").read_text(encoding="utf-8")
        enemy = (self.multi / "enemy_attack_support.inc").read_text(encoding="utf-8")

        _, _, attack_test = motor_30s.named_block(attack, '{"attack_support/motor_test"')
        _, _, enemy_test = motor_30s.named_block(enemy, '{"enemy_attack/motor_test"')

        self.assertEqual(attack_test.count('{"delay" {time 30}}'), 1)
        self.assertEqual(enemy_test.count('{"delay" {time 30}}'), 1)
        self.assertEqual(attack_test.count('("as_poke_faction_motor")'), 1)
        self.assertEqual(enemy_test.count('("ea_poke_motor")'), 1)
        self.assertNotIn('{"delay" {time 15}}', attack_test)
        self.assertNotIn('{"delay" {time 45}}', attack_test)
        self.assertNotIn('{"delay" {time 15}}', enemy_test)
        self.assertNotIn('{"delay" {time 45}}', enemy_test)
        self.assertIn('attack_support_air_test$"} {op "="} {value 0}', attack)

        self.assertIn('(define "as_place_motor_visible"', attack)
        self.assertIn('(define "ea_place_motor_visible"', enemy)
        self.assertIn('target_waypoint "attack_support_entry_a"', attack)
        self.assertIn('target_waypoint "attack_support_entry_b"', attack)
        self.assertIn('target_waypoint "attack_support_entry_a"', enemy)
        self.assertIn('target_waypoint "attack_support_entry_b"', enemy)

        attack_macro_start = attack.index('(define "as_place_motor_visible"')
        attack_motor_start = attack.index('; ===== MOTORIZED INSERT (cmd 19)', attack_macro_start)
        attack_macro = attack[attack_macro_start:attack_motor_start]
        enemy_macro_start = enemy.index('(define "ea_place_motor_visible"')
        enemy_motor_start = enemy.index('; ===== MOTORIZED INSERT (cmd 19)', enemy_macro_start)
        enemy_macro = enemy[enemy_macro_start:enemy_motor_start]
        self.assertNotIn('attack_support_rear_a1', attack_macro)
        self.assertNotIn('attack_support_rear_b1', attack_macro)
        self.assertNotIn('attack_support_rear_a1', enemy_macro)
        self.assertNotIn('attack_support_rear_b1', enemy_macro)

        for faction in ("rusa", "ukr", "prc", "nato"):
            marker = f'{{"attack_support/ally_{faction}_motor"'
            _, _, block = motor_visible.named_block(attack, marker)
            self.assertIn('("as_place_motor_visible")', block)
            self.assertNotIn('("am_place_at_entry")', block)

            marker = f'{{"enemy_attack/{faction}_motor"'
            _, _, block = motor_visible.named_block(enemy, marker)
            self.assertIn('("ea_place_motor_visible")', block)
            self.assertNotIn('("ea_place_at_entry")', block)

    def test_combined_overlays_are_idempotent(self) -> None:
        self.apply_all()
        first = {
            path.name: path.read_bytes()
            for path in self.multi.iterdir()
            if path.is_file()
        }
        self.apply_all()
        second = {
            path.name: path.read_bytes()
            for path in self.multi.iterdir()
            if path.is_file()
        }
        self.assertEqual(first, second)


if __name__ == "__main__":
    unittest.main()
