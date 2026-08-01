#!/usr/bin/env python3
"""Comment-tolerant entry point for the 75-second stop-and-dismount overlay."""
from __future__ import annotations

import argparse
import importlib.util
import sys
from pathlib import Path

MODULE_PATH = Path(__file__).with_name("apply_defense_motor_75s.py")
spec = importlib.util.spec_from_file_location("_defense_motor_75s_base", MODULE_PATH)
assert spec and spec.loader
base = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = base
spec.loader.exec_module(base)


def fixed_patch_stop_before_emit(text: str, prefix: str) -> str:
    """Insert the stop immediately before emit, allowing existing comments."""
    start, end, block = base.paren_block(text, base.FINISHERS[prefix])
    if base.STOP_MARKER in block:
        return text

    ride_token = '{"delay" {time 73}}' if prefix == "ea" else '{"delay" {time 75}}'
    ride_at = block.find(ride_token)
    if ride_at < 0:
        raise base.PatchError(f"{prefix}: timed ride delay is missing")
    emit_at = block.find('{"emit"', ride_at)
    if emit_at < 0:
        raise base.PatchError(f"{prefix}: passenger emit is missing after ride delay")

    line_start = block.rfind("\n", 0, emit_at) + 1
    indent = block[line_start:emit_at]
    stop = base.render_stop(prefix, indent)
    block = block[:line_start] + stop + "\n" + block[line_start:]
    return text[:start] + block + text[end:]


base.patch_stop_before_emit = fixed_patch_stop_before_emit

PatchError = base.PatchError
FILES = base.FILES
FINISHERS = base.FINISHERS
DEPLOY_TAGS = base.DEPLOY_TAGS
HULL_TAGS = base.HULL_TAGS
PAX_TAGS = base.PAX_TAGS
EXIT_HELPERS = base.EXIT_HELPERS
HOLD_MARKER = base.HOLD_MARKER
STOP_MARKER = base.STOP_MARKER
RELEASE_MARKER = base.RELEASE_MARKER
EXIT_RESUME_MARKER = base.EXIT_RESUME_MARKER
balanced = base.balanced
paren_block = base.paren_block
render_hull_crew_selector = base.render_hull_crew_selector
apply = base.apply
validate = base.validate


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    if args.check:
        validate(args.root)
        print("Player-defense motors validated: 75s drive, stop, then passenger emit.")
    else:
        changed = apply(args.root)
        print(f"75-second stop-and-dismount lifecycle patched {len(changed)} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
