#!/usr/bin/env python3
"""Idempotent entry point for transport flag-perimeter waypoints."""
from __future__ import annotations

import argparse
import importlib.util
import math
import sys
from pathlib import Path

MODULE_PATH = Path(__file__).with_name("apply_transport_flag_perimeter_waypoints.py")
spec = importlib.util.spec_from_file_location("_transport_perimeter_base", MODULE_PATH)
assert spec and spec.loader
base = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = base
spec.loader.exec_module(base)

original_patch_text = base.patch_text


def existing_route_is_current(text: str, label: str) -> bool:
    try:
        flags = base.extract_flags(text, label)
        actual = base.parse_waypoints(text, label)
    except base.PatchError:
        return False
    expected = base.route_points(flags)
    if len(actual) != len(expected):
        return False
    return all(
        math.hypot(current.x - target.x, current.y - target.y) <= 0.1
        and abs(current.z - target.z) <= 0.1
        for current, target in zip(actual, expected)
    )


def patch_text(text: str, label: str) -> str:
    if existing_route_is_current(text, label):
        return text
    return original_patch_text(text, label)


base.patch_text = patch_text

PatchError = base.PatchError
Flag = base.Flag
WAYPOINT_PREFIX = base.WAYPOINT_PREFIX
WAYPOINT_COUNT = base.WAYPOINT_COUNT
OFFSET = base.OFFSET
RADIUS = base.RADIUS
CLOSEST_TO_FLAG = base.CLOSEST_TO_FLAG
MAP_DIR_PATTERN = base.MAP_DIR_PATTERN
extract_flags = base.extract_flags
parse_waypoints = base.parse_waypoints
route_points = base.route_points
map_files = base.map_files
validate_text = base.validate_text
validate = base.validate
apply = base.apply


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if args.check:
        validate(args.root)
        print("Transport flag-perimeter waypoints validated on all 14 maps.")
    else:
        changed = apply(args.root)
        print(f"Transport flag-perimeter waypoints patched {len(changed)} map(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
