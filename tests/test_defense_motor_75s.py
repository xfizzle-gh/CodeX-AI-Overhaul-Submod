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


visible = load_module("motor_visible_overlay_75s_test", ROOT / "tools" / "apply_motor_visible_package_overlay.py")
timing = load_module("runtime_motor_timing_75s_test", ROOT / "tools" / "apply_runtime_proven_motor_60s.py")
defender = load_module("friendly_defender_motor_75s_test", ROOT / "tools" / "apply_friendly_defender_motor_one_shot.py")
correction = load_module("motor_drive_origin_exit_75s_test", ROOT / "tools" / "apply_motor_drive_origin_exit_fixed.py")
tuning = load_module("defense_motor_75s", ROOT / "tools" / "apply_defense_motor_75s.py")

FILES = ("attack_support_waves.inc", "enemy_attack_support.inc", "defense_support_waves.inc", "dcg_vars.inc")


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

    def test_only_timing_stop_and_resume_are_added(self) -> None:
        before_attack = (self.multi / "attack_support_waves.inc").read_bytes()
        changed = tuning.apply(self.root)
        self.assertEqual({p.name for p in changed}, {"enemy_attack_support.inc", "defense_support_waves.inc"})
        tuning.validate(self.root)
        self.assertEqual(before_attack, (self.multi / "attack_support_waves.inc").read_bytes())

        for filename, name, prefix in (
            ("enemy_attack_support.inc", "ea_finish_motor", "ea"),
            ("defense_support_waves.inc", "ds_finish_motor", "ds"),
        ):
            block = tuning.paren_block(self.text(filename), name)[2]
            self.assertEqual(block.count(tuning.STOP_MARKER), 1)
            self.assertEqual(block.count(tuning.RESUME_MARKER), 1)
            self.assertEqual(block.count('{"delay" {time 1}}'), 1)
            self.assertEqual(block.count('{mode passengers}'), 1)
            self.assertLess(block.find(tuning.STOP_MARKER), block.find('{"emit"'))
            self.assertLess(block.find('{"emit"'), block.find(tuning.RESUME_MARKER))
            self.assertLess(block.find(tuning.RESUME_MARKER), block.find(f'("{tuning.EXIT_HELPERS[prefix]}")'))
            for marker in tuning.FORBIDDEN_MARKERS:
                self.assertNotIn(marker, block)

    def test_75_second_contract_is_preserved(self) -> None:
        tuning.apply(self.root)
        enemy = tuning.paren_block(self.text("enemy_attack_support.inc"), "ea_finish_motor")[2]
        friendly = tuning.paren_block(self.text("defense_support_waves.inc"), "ds_finish_motor")[2]
        self.assertEqual(enemy.count('{"delay" {time 2}}'), 1)
        self.assertEqual(enemy.count('{"delay" {time 73}}'), 1)
        self.assertEqual(friendly.count('{"delay" {time 75}}'), 1)

    def test_idempotent_and_check_only_is_read_only(self) -> None:
        before = {name: (self.multi / name).read_bytes() for name in FILES}
        predicted = tuning.apply(self.root, check_only=True)
        self.assertEqual({p.name for p in predicted}, {"enemy_attack_support.inc", "defense_support_waves.inc"})
        self.assertEqual(before, {name: (self.multi / name).read_bytes() for name in FILES})
        tuning.apply(self.root)
        first = {name: (self.multi / name).read_bytes() for name in FILES}
        self.assertEqual(tuning.apply(self.root), [])
        self.assertEqual(first, {name: (self.multi / name).read_bytes() for name in FILES})


if __name__ == "__main__":
    unittest.main()
