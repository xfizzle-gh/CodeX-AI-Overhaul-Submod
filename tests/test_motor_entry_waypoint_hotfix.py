from __future__ import annotations

import importlib.util
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PRODUCTION_SCRIPT = ROOT / "tools" / "apply_canonical_motor_production_overlay.py"
ISOLATION_SCRIPT = ROOT / "tools" / "apply_motor_linked_seat_hotfix.py"
ENTRY_SCRIPT = ROOT / "tools" / "apply_motor_entry_waypoint_hotfix.py"
FILES = (
    "resource/map/multi/faction_support_templates.inc",
    "resource/map/multi/dcg_vars.inc",
    "resource/map/multi/attack_support_waves.inc",
    "resource/map/multi/defense_support_waves.inc",
    "resource/map/multi/enemy_attack_support.inc",
    "resource/map/multi/enemy_defense_support.inc",
)


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec and spec.loader
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


production = load_module("canonical_motor_for_entry_test", PRODUCTION_SCRIPT)
isolation = load_module("motor_isolation_for_entry_test", ISOLATION_SCRIPT)
entry = load_module("motor_entry_waypoint_hotfix", ENTRY_SCRIPT)


class MotorEntryWaypointHotfixTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        for relative in FILES:
            source = ROOT / relative
            target = self.root / relative
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, target)

    def tearDown(self) -> None:
        self.temp.cleanup()

    def build_pre_entry_overlay(self) -> None:
        production.apply(self.root)
        isolation.apply(self.root)

    def test_all_four_motor_placers_use_numbered_entry_pads(self) -> None:
        self.build_pre_entry_overlay()
        changed = entry.apply(self.root)
        self.assertEqual(
            set(changed),
            {engine.relative_path for engine in entry.ENGINES},
        )

        for engine in entry.ENGINES:
            text = (self.root / engine.relative_path).read_text(encoding="utf-8-sig")
            entry.validate_engine(text, engine)

            placer = entry.paren_block(
                text, f'(define "{engine.placer_macro}"'
            )[2]
            self.assertNotIn(entry.BARE_A, placer)
            self.assertNotIn(entry.BARE_B, placer)
            self.assertEqual(
                placer.count(entry.NUMBERED_A) + placer.count(entry.NUMBERED_B),
                3,
            )
            self.assertIn(entry.NUMBERED_A, placer)
            self.assertIn(entry.NUMBERED_B, placer)

            finisher = entry.paren_block(
                text, f'(define "{engine.finisher}"'
            )[2]
            self.assertNotIn('waypoint "attack_support_entry_a1"', finisher)
            self.assertNotIn('waypoint "attack_support_entry_b1"', finisher)
            self.assertTrue(
                'waypoint "attack_support_entry_a"' in finisher
                or 'waypoint "attack_support_entry_b"' in finisher
            )

    def test_entry_hotfix_is_idempotent(self) -> None:
        self.build_pre_entry_overlay()
        entry.apply(self.root)
        before = {relative: (self.root / relative).read_bytes() for relative in FILES}
        changed = entry.apply(self.root)
        after = {relative: (self.root / relative).read_bytes() for relative in FILES}
        self.assertEqual(changed, [])
        self.assertEqual(before, after)

    def test_check_mode_reports_without_writing(self) -> None:
        self.build_pre_entry_overlay()
        before = {relative: (self.root / relative).read_bytes() for relative in FILES}
        changed = entry.apply(self.root, check_only=True)
        after = {relative: (self.root / relative).read_bytes() for relative in FILES}
        self.assertEqual(
            set(changed),
            {engine.relative_path for engine in entry.ENGINES},
        )
        self.assertEqual(before, after)


if __name__ == "__main__":
    unittest.main()
