#!/usr/bin/env python3
"""Corrected entry point for the four-quadrant normal transport overlay."""
from __future__ import annotations

import argparse
import sys
import types
from pathlib import Path

SOURCE_PATH = Path(__file__).with_name("apply_four_quadrant_transport_patrol.py")
source = SOURCE_PATH.read_text(encoding="utf-8")

replacements = (
    (
        '\\t\\t\\t\\t\\t\\t{{"6.entities" {{selector {{tag {source_hull}}}} {{count {{op ">="}} {{value 1}}}}}}',
        '\\t\\t\\t\\t\\t\\t{{"6.entities" {{selector {{tag {source_hull}}}}} {{count {{op ">="}} {{value 1}}}}}}',
        "source-hull availability selector",
    ),
    (
        '\\t\\t\\t\\t\\t\\t{{selector {{tag {engine.deploy_tag}}}}\\n\\t\\t\\t\\t\\t\\t{{tag_remove {engine.deploy_tag}}}',
        '\\t\\t\\t\\t\\t\\t{{selector {{tag {engine.deploy_tag}}}}}\\n\\t\\t\\t\\t\\t\\t{{tag_remove {engine.deploy_tag}}}',
        "post-dispatch tag-removal selector",
    ),
)
for old, new, label in replacements:
    if source.count(old) != 1:
        raise RuntimeError(
            f"Expected exactly one malformed {label} in the base generator"
        )
    source = source.replace(old, new, 1)

module_name = "_four_quadrant_transport_patrol_corrected"
base = types.ModuleType(module_name)
base.__file__ = str(SOURCE_PATH)
base.__package__ = ""
sys.modules[module_name] = base
exec(compile(source, str(SOURCE_PATH), "exec"), base.__dict__)

for exported in dir(base):
    if not exported.startswith("__"):
        globals()[exported] = getattr(base, exported)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if args.check:
        validate(args.root)
        print("Four-quadrant normal transport patrols validated.")
    else:
        changed = apply(args.root)
        print(f"Four-quadrant normal transport patrol patched {len(changed)} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
