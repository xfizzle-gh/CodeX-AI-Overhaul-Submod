#!/usr/bin/env python3
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RUNTIME = ROOT / "resource/map/multi/attack_support_waves.inc"
TESTS = ROOT / "tests/test_allied_support_shared_fow.py"

text = RUNTIME.read_text(encoding="utf-8")
malformed = (
    "\t\t\t\t\t\t\t\t; Final player handoff for terrain FoW. Keep AI control and selection disabled above.\n"
    "\t\t\t\t{\"autoassign\"\n"
    "\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}\n"
    "\t\t\t\t}\n"
    "{\"entity_state\"\n"
)
canonical = (
    "\t\t\t\t; Final player handoff for terrain FoW. Keep AI control and selection disabled above.\n"
    "\t\t\t\t{\"autoassign\"\n"
    "\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}\n"
    "\t\t\t\t}\n"
    "\t\t\t\t{\"entity_state\"\n"
)
if text.count(malformed) != 1:
    raise RuntimeError(f"expected one malformed terrain FoW block, found {text.count(malformed)}")
RUNTIME.write_text(text.replace(malformed, canonical, 1), encoding="utf-8", newline="")

tests = TESTS.read_text(encoding="utf-8")
needle = "        self.assertEqual(finish.count('{\"autoassign\"'), 1)\n"
replacement = needle + "        self.assertIn('\\n\\t\\t\\t\\t{\"autoassign\"', finish)\n        self.assertIn('\\n\\t\\t\\t\\t{\"entity_state\"', finish[autoassign_pos:cleanup_pos])\n"
if tests.count(needle) != 1:
    raise RuntimeError(f"expected one terrain FoW count assertion, found {tests.count(needle)}")
TESTS.write_text(tests.replace(needle, replacement, 1), encoding="utf-8", newline="")
print("terrain_fow_format=canonical")
