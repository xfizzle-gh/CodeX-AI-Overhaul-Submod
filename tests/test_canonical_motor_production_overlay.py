from __future__ import annotations

import importlib.util
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "tools" / "apply_canonical_motor_production_overlay.py"
FILES = (
    "resource/map/multi/faction_support_templates.inc",
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


motor = load_module("canonical_motor_overlay", SCRIPT)


class CanonicalMotorProductionOverlayTests(unittest.TestCase):
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

    def test_all_quadrants_and_factions_receive_canonical_contract(self) -> None:
        changed = motor.apply(self.root)
        self.assertEqual(len(changed), 4)

        templates = (
            self.root / "resource/map/multi/faction_support_templates.inc"
        ).read_text(encoding="utf-8-sig")
        motor.validate_templates(templates)

        for engine in motor.ENGINES:
            text = (self.root / engine.relative_path).read_text(encoding="utf-8-sig")
            motor.validate_engine(text, engine)

            finisher = motor.paren_block(text, f'(define "{engine.finisher}"')[2]
            self.assertEqual(finisher.count('{"delay" {time 60}}'), 1)
            self.assertIn('{emit {mode passengers}}', finisher)
            self.assertNotIn('attack_support_entry_a1', finisher)
            self.assertNotIn('attack_support_entry_b1', finisher)

            clock = motor.brace_block(
                text, '{"' + engine.namespace + '/motor_clock"'
            )[2]
            command = clock.index(
                '{"set_i" {var "' + engine.namespace + '_wave_cmd$"}'
            )
            schedule = [
                int(value)
                for value in motor.re.findall(
                    r'\{"delay"\s+\{time\s+([0-9]+)\}\}', clock[:command]
                )
            ]
            self.assertEqual(schedule, [30, 30, 30, 180, 240, 300])

            cleanup = motor.brace_block(
                text, '{"' + engine.namespace + '/motor_cleanup"'
            )[2]
            self.assertEqual(cleanup.count('{"delay" {time 90}}'), 1)

            for faction in motor.FACTIONS:
                trigger = motor.brace_block(
                    text,
                    '{"' + engine.trigger_pattern.format(faction=faction) + '"',
                )[2]
                condition = trigger[: trigger.index("{actions")]
                self.assertIn(f"ally_sup_{faction}_motor_hull", condition)
                self.assertNotIn(f"ally_sup_{faction}_p1_hull", condition)
                self.assertIn(f'(\"{engine.placer_macro}\")', trigger)

    def test_overlay_is_idempotent(self) -> None:
        motor.apply(self.root)
        first = {
            relative: (self.root / relative).read_bytes() for relative in FILES
        }
        changed = motor.apply(self.root)
        second = {
            relative: (self.root / relative).read_bytes() for relative in FILES
        }
        self.assertEqual(changed, [])
        self.assertEqual(first, second)

    def test_check_mode_does_not_write(self) -> None:
        before = {
            relative: (self.root / relative).read_bytes() for relative in FILES
        }
        changed = motor.apply(self.root, check_only=True)
        after = {
            relative: (self.root / relative).read_bytes() for relative in FILES
        }
        self.assertEqual(len(changed), 4)
        self.assertEqual(before, after)


if __name__ == "__main__":
    unittest.main()
