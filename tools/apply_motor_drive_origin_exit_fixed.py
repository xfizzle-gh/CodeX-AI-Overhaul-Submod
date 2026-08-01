#!/usr/bin/env python3
"""Indentation-independent entry point for the motor drive/exit correction."""
from __future__ import annotations

import argparse
import importlib.util
import sys
from pathlib import Path

MODULE_PATH = Path(__file__).with_name("apply_motor_drive_origin_exit.py")
spec = importlib.util.spec_from_file_location("_motor_drive_origin_exit_base", MODULE_PATH)
assert spec and spec.loader
base = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = base
spec.loader.exec_module(base)


def fixed_upsert_exit_helper(text: str, prefix: str) -> str:
    """Insert or replace a helper without depending on source indentation."""
    helper = base.EXIT_HELPERS[prefix]
    desired = base.render_exit_helper(prefix)
    helper_marker = f'(define "{helper}"'

    existing = text.find(helper_marker)
    if existing >= 0:
        line_start = text.rfind("\n", 0, existing) + 1
        end = base.balanced(text, existing, "(", ")", helper)
        return text[:line_start] + desired + text[end:]

    finisher_marker = f'(define "{base.FINISHERS[prefix]}"'
    position = text.find(finisher_marker)
    if position < 0:
        raise base.PatchError(f"Missing insertion anchor for {helper}")

    line_start = text.rfind("\n", 0, position) + 1
    return text[:line_start] + desired + "\n\n" + text[line_start:]


base.upsert_exit_helper = fixed_upsert_exit_helper

PatchError = base.PatchError
FILES = base.FILES
FINISHERS = base.FINISHERS
HULL_TAGS = base.HULL_TAGS
EXIT_HELPERS = base.EXIT_HELPERS
EXIT_WAYPOINTS = base.EXIT_WAYPOINTS
RETRY_MARKER = base.RETRY_MARKER
balanced = base.balanced
paren_block = base.paren_block
apply = base.apply
validate = base.validate


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    if args.check:
        validate(args.root)
        print("Motor drive retry and origin-side exits validated.")
    else:
        changed = apply(args.root)
        print(f"Motor drive/exit correction patched {len(changed)} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
