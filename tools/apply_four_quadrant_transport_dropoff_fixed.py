#!/usr/bin/env python3
"""Executable entry point for the reversible perimeter drop-off overlay."""
from __future__ import annotations

import argparse
import importlib.util
import sys
from pathlib import Path

SOURCE_PATH = Path(__file__).with_name("apply_four_quadrant_transport_dropoff.py")

spec = importlib.util.spec_from_file_location("_transport_dropoff_impl", SOURCE_PATH)
if spec is None or spec.loader is None:
    raise RuntimeError(f"Could not load {SOURCE_PATH}")
impl = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = impl
spec.loader.exec_module(impl)

# The inherited functions were defined inside the corrected patrol generator's
# implementation module. Bind the replacement renderer and validator directly
# into those original global dictionaries so patch_engine/apply resolve them.
impl.base.patch_engine.__globals__["render_engine"] = impl.render_engine
impl.base.apply.__globals__["validate"] = impl.validate

apply = impl.base.apply
validate = impl.validate
validate_engine = impl.validate_engine
render_engine = impl.render_engine
ENGINES = impl.ENGINES
FACTIONS = impl.FACTIONS
FILES = impl.FILES
PatchError = impl.PatchError
marked_bounds = impl.marked_bounds
base = impl.base
HOLD_MARKER = impl.HOLD_MARKER
DROPOFF_MARKER = impl.DROPOFF_MARKER
ARRIVAL_DISTANCE = impl.ARRIVAL_DISTANCE
DROPPED_STEP = impl.DROPPED_STEP


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if args.check:
        validate(args.root)
        print("Four-quadrant perimeter drop-off transports validated.")
    else:
        changed = apply(args.root)
        print(f"Four-quadrant perimeter drop-off patched {len(changed)} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
