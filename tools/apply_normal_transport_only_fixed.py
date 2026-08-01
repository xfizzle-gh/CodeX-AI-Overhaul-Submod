#!/usr/bin/env python3
"""Idempotent entry point for the normal transport-only overlay."""
from __future__ import annotations

import argparse
import importlib.util
import sys
from pathlib import Path

MODULE_PATH = Path(__file__).with_name("apply_normal_transport_only.py")
spec = importlib.util.spec_from_file_location("_normal_transport_only_base", MODULE_PATH)
assert spec and spec.loader
base = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = base
spec.loader.exec_module(base)


def idempotent_replace_unique(text: str, old: str, new: str, label: str) -> str:
    old_count = text.count(old)
    new_count = text.count(new)
    if old_count == 1 and new_count == 0:
        return text.replace(old, new, 1)
    if old_count == 0 and new_count == 1:
        return text
    raise base.PatchError(
        f"{label}: expected one original or one patched occurrence; "
        f"found old={old_count}, new={new_count}"
    )


base.replace_unique = idempotent_replace_unique

PatchError = base.PatchError
apply = base.apply
validate = base.validate


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    if args.check:
        validate(args.root)
        print("Normal transport-only deployment validated: one friendly and one enemy truck.")
    else:
        changed = apply(args.root)
        print(f"Normal transport-only overlay patched {len(changed)} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
