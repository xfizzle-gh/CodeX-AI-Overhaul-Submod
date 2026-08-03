from __future__ import annotations

import shutil
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

import apply_motor_arrival_release_overlay as overlay  # noqa: E402


class MotorArrivalReleaseOverlayTests(unittest.TestCase):
    def make_tree(self) -> Path:
        temp = Path(tempfile.mkdtemp(prefix="motor-arrival-overlay-"))
        for engine in overlay.ENGINES:
            source = ROOT / engine.relative_path
            target = temp / engine.relative_path
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, target)
        self.addCleanup(shutil.rmtree, temp, True)
        return temp

    def test_all_four_engines_patch_once_and_are_idempotent(self) -> None:
        root = self.make_tree()
        changed = overlay.apply(root)
        self.assertEqual(len(changed), 4)
        self.assertEqual(overlay.apply(root), [])

        for engine in overlay.ENGINES:
            text = (root / engine.relative_path).read_text(encoding="utf-8-sig")
            finisher = overlay.balanced_define(text, engine.finisher)
            self.assertIn(overlay.MARKER, finisher)
            self.assertIn("{time 5}", finisher)
            self.assertIn("{value 12}", finisher)
            self.assertIn("60-second fallback reached", finisher)
            self.assertIn('{op "=="} {value 1}', finisher)
            self.assertIn('{op "=="} {value 2}', finisher)
            self.assertNotIn('{"delay" {time 7}}', finisher)
            self.assertEqual(finisher.count("{"), finisher.count("}"))
            self.assertEqual(finisher.count("("), finisher.count(")"))

    def test_check_mode_does_not_write(self) -> None:
        root = self.make_tree()
        before = {
            engine.relative_path: (root / engine.relative_path).read_bytes()
            for engine in overlay.ENGINES
        }
        changed = overlay.apply(root, check_only=True)
        self.assertEqual(len(changed), 4)
        after = {
            engine.relative_path: (root / engine.relative_path).read_bytes()
            for engine in overlay.ENGINES
        }
        self.assertEqual(before, after)


if __name__ == "__main__":
    unittest.main()
