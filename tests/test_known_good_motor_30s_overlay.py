from __future__ import annotations

import importlib.util
import shutil
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "tools" / "apply_known_good_motor_30s_test.py"

spec = importlib.util.spec_from_file_location("motor_30s", SCRIPT)
assert spec and spec.loader
motor_30s = importlib.util.module_from_spec(spec)
spec.loader.exec_module(motor_30s)


class KnownGoodMotorThirtySecondOverlayTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.multi = Path(self.temp.name)
        for name in ("attack_support_waves.inc", "enemy_attack_support.inc"):
            shutil.copy2(ROOT / "resource" / "map" / "multi" / name, self.multi / name)

    def tearDown(self) -> None:
        self.temp.cleanup()

    def test_actual_known_good_files_become_one_shot_30_second_tests(self) -> None:
        motor_30s.patch_multi_root(self.multi)
        motor_30s.validate_multi_root(self.multi)

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

    def test_overlay_is_idempotent(self) -> None:
        motor_30s.patch_multi_root(self.multi)
        first = {
            path.name: path.read_bytes()
            for path in self.multi.iterdir()
            if path.is_file()
        }
        motor_30s.patch_multi_root(self.multi)
        second = {
            path.name: path.read_bytes()
            for path in self.multi.iterdir()
            if path.is_file()
        }
        self.assertEqual(first, second)


if __name__ == "__main__":
    unittest.main()
