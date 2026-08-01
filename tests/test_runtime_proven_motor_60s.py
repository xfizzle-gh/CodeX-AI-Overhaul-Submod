from __future__ import annotations

import importlib.util
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "tools" / "apply_runtime_proven_motor_60s.py"


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec and spec.loader
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


motor = load_module("runtime_proven_motor_60s", SCRIPT)


class RuntimeProvenMotorTimingTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.multi = Path(self.temp.name)
        self.original: dict[str, str] = {}
        for engine in motor.ENGINES:
            source = ROOT / "resource" / "map" / "multi" / engine.filename
            target = self.multi / engine.filename
            shutil.copy2(source, target)
            self.original[engine.filename] = target.read_text(encoding="utf-8-sig")

    def tearDown(self) -> None:
        self.temp.cleanup()

    @staticmethod
    def reverse_timing_only(text: str, engine) -> str:
        start, end, finisher = motor.paren_block(
            text, f'(define "{engine.finisher}"'
        )
        if finisher.count('{"delay" {time 60}}') != 1:
            raise AssertionError("patched finisher does not have exactly one 60s ride")
        finisher = finisher.replace(
            '{"delay" {time 60}}', '{"delay" {time 28}}', 1
        )
        text = text[:start] + finisher + text[end:]

        start, end, cleanup = motor.brace_block(
            text, f'{{"{engine.cleanup_trigger}"'
        )
        if cleanup.count('{"delay" {time 90}}') != 1:
            raise AssertionError("patched cleanup does not have exactly one 90s delay")
        cleanup = cleanup.replace(
            '{"delay" {time 90}}', '{"delay" {time 45}}', 1
        )
        return text[:start] + cleanup + text[end:]

    def test_only_approved_timing_values_change(self) -> None:
        changed = motor.patch_multi_root(self.multi)
        self.assertEqual(
            {path.name for path in changed},
            {engine.filename for engine in motor.ENGINES},
        )

        for engine in motor.ENGINES:
            patched = (self.multi / engine.filename).read_text(encoding="utf-8-sig")
            motor.validate_engine(patched, engine)
            restored = self.reverse_timing_only(patched, engine)
            self.assertEqual(
                restored,
                self.original[engine.filename],
                f"{engine.filename} changed outside the approved 28->60 and 45->90 values",
            )

    def test_overlay_is_idempotent(self) -> None:
        motor.patch_multi_root(self.multi)
        first = {
            engine.filename: (self.multi / engine.filename).read_bytes()
            for engine in motor.ENGINES
        }
        changed = motor.patch_multi_root(self.multi)
        second = {
            engine.filename: (self.multi / engine.filename).read_bytes()
            for engine in motor.ENGINES
        }
        self.assertEqual(changed, [])
        self.assertEqual(first, second)

    def test_check_mode_does_not_write(self) -> None:
        before = {
            engine.filename: (self.multi / engine.filename).read_bytes()
            for engine in motor.ENGINES
        }
        changed = motor.patch_multi_root(self.multi, check_only=True)
        after = {
            engine.filename: (self.multi / engine.filename).read_bytes()
            for engine in motor.ENGINES
        }
        self.assertEqual(
            {path.name for path in changed},
            {engine.filename for engine in motor.ENGINES},
        )
        self.assertEqual(before, after)


if __name__ == "__main__":
    unittest.main()
