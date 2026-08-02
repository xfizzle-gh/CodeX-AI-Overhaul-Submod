#!/usr/bin/env python3
"""Validated entry point for the turnaround-first 75-second motor overlay.

The source finisher already contains an unrelated 0.5-second startup delay. This
wrapper scopes the new half-second assertion to the turnaround segment rather
than incorrectly counting the entire finisher.
"""
from __future__ import annotations

import argparse
import importlib.util
import sys
from pathlib import Path

MODULE_PATH = Path(__file__).with_name("apply_defense_motor_75s.py")
spec = importlib.util.spec_from_file_location("_defense_motor_turnaround_base", MODULE_PATH)
assert spec and spec.loader
base = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = base
spec.loader.exec_module(base)


def fixed_validate_finisher(block: str, prefix: str) -> None:
    hull = base.HULL_TAGS[prefix]
    pax = base.PAX_TAGS[prefix]
    flag = base.FLAG_TAGS[prefix]
    ride_token = '{"delay" {time 73}}' if prefix == "ea" else '{"delay" {time 75}}'
    helper_call = f'("{base.EXIT_HELPERS[prefix]}")'

    for marker in base.FORBIDDEN_MARKERS:
        if marker in block:
            raise base.PatchError(
                f"{prefix}: abandoned passenger-AI rewrite marker is present"
            )

    for marker in (
        base.PRETURN_MARKER,
        base.STOP_MARKER,
        base.RESUME_MARKER,
        base.REASSERT_MARKER,
    ):
        if block.count(marker) != 1:
            raise base.PatchError(f"{prefix}: expected exactly one marker {marker}")
    if block.count(helper_call) != 2:
        raise base.PatchError(f"{prefix}: expected exactly two origin helper calls")
    if block.count('{"delay" {time 1}}') != 1:
        raise base.PatchError(f"{prefix}: expected one one-second stop dwell")
    if block.count('{mode passengers}') != 1:
        raise base.PatchError(f"{prefix}: passenger-only emit contract changed")

    ride_at = block.find(ride_token)
    preturn_at = block.find(base.PRETURN_MARKER)
    stop_at = block.find(base.STOP_MARKER)
    emit_at = block.find('{"emit"', stop_at)
    resume_at = block.find(base.RESUME_MARKER)
    final_helper_at = block.rfind(helper_call)
    reassert_at = block.find(base.REASSERT_MARKER)
    if not (
        0
        <= ride_at
        < preturn_at
        < stop_at
        < emit_at
        < resume_at
        < final_helper_at
        < reassert_at
    ):
        raise base.PatchError(
            f"{prefix}: ride/turn/stop/emit/exit/reassert order is invalid"
        )

    turn_segment = block[preturn_at:stop_at]
    if turn_segment.count('{"delay" {time 0.5}}') != 1:
        raise base.PatchError(
            f"{prefix}: turnaround segment needs exactly one half-second dwell"
        )

    stop_block = block[stop_at:emit_at]
    if (
        f'{{tag {hull}}}' not in stop_block
        or '{movement {speed stop}}' not in stop_block
    ):
        raise base.PatchError(f"{prefix}: hull stop state is incomplete")

    reassert_block = block[reassert_at:]
    for token in (
        f'{{tag {pax}}}',
        '{drop orders}',
        '{action advance}',
        f'{{tag {flag}}}',
    ):
        if token not in reassert_block:
            raise base.PatchError(
                f"{prefix}: post-exit infantry reassert missing {token}"
            )


base.validate_finisher = fixed_validate_finisher

PatchError = base.PatchError
FILES = base.FILES
FINISHERS = base.FINISHERS
HULL_TAGS = base.HULL_TAGS
PAX_TAGS = base.PAX_TAGS
FLAG_TAGS = base.FLAG_TAGS
EXIT_HELPERS = base.EXIT_HELPERS
PRETURN_MARKER = base.PRETURN_MARKER
STOP_MARKER = base.STOP_MARKER
RESUME_MARKER = base.RESUME_MARKER
REASSERT_MARKER = base.REASSERT_MARKER
FORBIDDEN_MARKERS = base.FORBIDDEN_MARKERS
balanced = base.balanced
paren_block = base.paren_block
patch_file = base.patch_file
apply = base.apply
validate = base.validate


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if args.check:
        validate(args.root)
        print("Turnaround-first transport lifecycle validated.")
    else:
        changed = apply(args.root)
        print(f"Turnaround-first passenger drop patched {len(changed)} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
