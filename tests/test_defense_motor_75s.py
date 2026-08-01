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
    "defense_motor_turnaround",
    ROOT / "tools" / "apply_defense_motor_turnaround.py",
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

    def finishers(self) -> tuple[str, str]:
        enemy = tuning.paren_block(
            self.text("enemy_attack_support.inc"), "ea_finish_motor"
        )[2]
        friendly = tuning.paren_block(
            self.text("defense_support_waves.inc"), "ds_finish_motor"
        )[2]
        return enemy, friendly

    def test_turnaround_begins_before_existing_passenger_emit(self) -> None:
        before_attack = (self.multi / "attack_support_waves.inc").read_bytes()
        changed = tuning.apply(self.root)
        self.assertEqual(
            {path.name for path in changed},
            {"enemy_attack_support.inc", "defense_support_waves.inc"},
        )
        tuning.validate(self.root)
        self.assertEqual(
            before_attack,
            (self.multi / "attack_support_waves.inc").read_bytes(),
        )

        for prefix, block in zip(("ea", "ds"), self.finishers()):
            helper = f'("{tuning.EXIT_HELPERS[prefix]}")'
            self.assertEqual(block.count(tuning.PRETURN_MARKER), 1)
            self.assertEqual(block.count(tuning.STOP_MARKER), 1)
            self.assertEqual(block.count(tuning.RESUME_MARKER), 1)
            self.assertEqual(block.count(tuning.REASSERT_MARKER), 1)
            self.assertEqual(block.count(helper), 2)
            self.assertEqual(block.count('{"delay" {time 1}}'), 1)
            self.assertEqual(block.count('{mode passengers}'), 1)

            ride = block.find(
                '{"delay" {time 73}}' if prefix == "ea" else '{"delay" {time 75}}'
            )
            preturn = block.find(tuning.PRETURN_MARKER)
            first_helper = block.find(helper, preturn)
            stop = block.find(tuning.STOP_MARKER)
            emit = block.find('{"emit"', stop)
            resume = block.find(tuning.RESUME_MARKER)
            final_helper = block.rfind(helper)
            reassert = block.find(tuning.REASSERT_MARKER)
            self.assertTrue(
                0 <= ride < preturn < first_helper < stop < emit < resume < final_helper < reassert
            )
            self.assertEqual(
                block[preturn:stop].count('{"delay" {time 0.5}}'),
                1,
            )

            for marker in tuning.FORBIDDEN_MARKERS:
                self.assertNotIn(marker, block)

    def test_infantry_attack_order_is_reasserted_after_truck_withdrawal(self) -> None:
        tuning.apply(self.root)
        for prefix, block in zip(("ea", "ds"), self.finishers()):
            helper = f'("{tuning.EXIT_HELPERS[prefix]}")'
            reassert_at = block.find(tuning.REASSERT_MARKER)
            final_helper_at = block.rfind(helper)
            self.assertGreater(reassert_at, final_helper_at)
            tail = block[reassert_at:]
            self.assertIn(f'{{tag {tuning.PAX_TAGS[prefix]}}}', tail)
            self.assertIn('{drop orders}', tail)
            self.assertIn('{action advance}', tail)
            self.assertIn(f'{{tag {tuning.FLAG_TAGS[prefix]}}}', tail)

    def test_75_second_contract_is_preserved(self) -> None:
        tuning.apply(self.root)
        enemy, friendly = self.finishers()
        self.assertEqual(enemy.count('{"delay" {time 2}}'), 1)
        self.assertEqual(enemy.count('{"delay" {time 73}}'), 1)
        self.assertNotIn('{"delay" {time 58}}', enemy)
        self.assertEqual(friendly.count('{"delay" {time 75}}'), 1)
        self.assertNotIn('{"delay" {time 60}}', friendly)

    def test_friendly_attacker_path_remains_byte_identical(self) -> None:
        before = (self.multi / "attack_support_waves.inc").read_bytes()
        tuning.apply(self.root)
        self.assertEqual(before, (self.multi / "attack_support_waves.inc").read_bytes())

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
