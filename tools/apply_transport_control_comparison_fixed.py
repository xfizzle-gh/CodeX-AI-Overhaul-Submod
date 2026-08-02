#!/usr/bin/env python3
"""Indentation-independent entry point for the transport comparison overlay."""
from __future__ import annotations

import argparse
import importlib.util
import sys
from pathlib import Path

MODULE_PATH = Path(__file__).with_name("apply_transport_control_comparison.py")
spec = importlib.util.spec_from_file_location("_transport_comparison_base", MODULE_PATH)
assert spec and spec.loader
base = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = base
spec.loader.exec_module(base)


def fixed_patch_support(text: str, *, friendly: bool) -> str:
    return base.upsert_before(
        text,
        base.FRIEND_BEGIN if friendly else base.ENEMY_BEGIN,
        base.FRIEND_END if friendly else base.ENEMY_END,
        base.control_section(friendly=friendly),
        (
            '{"defense_support/motor_cleanup"'
            if friendly
            else '{"enemy_attack/motor_cleanup"'
        ),
    )


base.patch_support = fixed_patch_support

PatchError = base.PatchError
PACKAGE_BEGIN = base.PACKAGE_BEGIN
PACKAGE_END = base.PACKAGE_END
TAG_BEGIN = base.TAG_BEGIN
TAG_END = base.TAG_END
FRIEND_BEGIN = base.FRIEND_BEGIN
FRIEND_END = base.FRIEND_END
ENEMY_BEGIN = base.ENEMY_BEGIN
ENEMY_END = base.ENEMY_END
FRIEND_PACKAGE = base.FRIEND_PACKAGE
FRIEND_HULL = base.FRIEND_HULL
FRIEND_PAX = base.FRIEND_PAX
ENEMY_PACKAGE = base.ENEMY_PACKAGE
ENEMY_HULL = base.ENEMY_HULL
ENEMY_PAX = base.ENEMY_PAX
marked_bounds = base.marked_bounds
apply = base.apply
validate = base.validate


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if args.check:
        validate(args.root)
        print("Transport comparison validated: scripted inserts plus two normal controls.")
    else:
        changed = apply(args.root)
        print(f"Transport comparison patched {len(changed)} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
