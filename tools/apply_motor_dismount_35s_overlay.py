#!/usr/bin/env python3
"""Increase motorized passenger travel time from 28 to 35 seconds.

This is a narrow tuning overlay. It changes only the travel delay inside the
validated friendly-attacker and enemy-attacker motor finish macros. Placement,
ownership, passenger emit, infantry advance, truck withdrawal, and cleanup are
left unchanged.
"""

from __future__ import annotations

import argparse
from pathlib import Path


class PatchError(RuntimeError):
    pass


def named_paren_block(text: str, marker: str) -> tuple[int, int, str]:
    start = text.find(marker)
    if start < 0:
        raise PatchError(f"Missing macro marker: {marker}")

    depth = 0
    in_string = False
    escaped = False
    for index in range(start, len(text)):
        char = text[index]
        if in_string:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == '"':
                in_string = False
            continue
        if char == '"':
            in_string = True
        elif char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
            if depth == 0:
                return start, index + 1, text[start : index + 1]

    raise PatchError(f"Unbalanced macro: {marker}")


def patch_macro(text: str, marker: str) -> str:
    start, end, block = named_paren_block(text, marker)

    old = '{"delay" {time 28}}'
    new = '{"delay" {time 35}}'

    if new in block:
        if old in block:
            raise PatchError(f"{marker}: contains both 28s and 35s travel delays")
        return text

    if block.count(old) != 1:
        raise PatchError(f"{marker}: expected exactly one 28-second travel delay")

    block = block.replace(old, new, 1)
    return text[:start] + block + text[end:]


def validate_multi_root(multi_root: Path) -> None:
    checks = (
        (multi_root / "attack_support_waves.inc", '(define "as_finish_motor"'),
        (multi_root / "enemy_attack_support.inc", '(define "ea_finish_motor"'),
    )

    for path, marker in checks:
        if not path.is_file():
            raise PatchError(f"Missing deployed support engine: {path}")
        text = path.read_text(encoding="utf-8-sig")
        _, _, block = named_paren_block(text, marker)
        if block.count('{"delay" {time 35}}') != 1:
            raise PatchError(f"{path.name}: expected exactly one 35-second motor travel delay")
        if '{"delay" {time 28}}' in block:
            raise PatchError(f"{path.name}: stale 28-second motor travel delay remains")
        if '{emit\n' not in block and '{"emit"' not in block:
            raise PatchError(f"{path.name}: passenger emit is missing from motor lifecycle")


def patch_multi_root(multi_root: Path) -> None:
    paths = (
        (multi_root / "attack_support_waves.inc", '(define "as_finish_motor"'),
        (multi_root / "enemy_attack_support.inc", '(define "ea_finish_motor"'),
    )

    for path, marker in paths:
        if not path.is_file():
            raise PatchError(f"Missing deployed support engine: {path}")
        text = path.read_text(encoding="utf-8-sig")
        patched = patch_macro(text, marker)
        path.write_text(patched, encoding="utf-8")

    validate_multi_root(multi_root)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--multi-root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    if args.check:
        validate_multi_root(args.multi_root)
    else:
        patch_multi_root(args.multi_root)

    print("Motor dismount timing ready: passengers remain mounted for 35 seconds.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
