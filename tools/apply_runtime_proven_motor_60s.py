#!/usr/bin/env python3
"""Tune only the runtime-proven two-path motor lifecycle.

This overlay is intentionally narrow. It starts from the exact runtime-validated
PR #67 checkpoint and changes only:

* motor ride before passenger emit: 28 -> 60 seconds
* departing-truck cleanup: 45 -> 90 seconds

It does not alter package claims, links, ownership, placement, actor state,
orders, waypoints, command variables, faction pools, or support-engine coverage.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path

RUNTIME_PROVEN_COMMIT = "38785d41db871dd989f72a64a532e62dfc1bb4dd"


class PatchError(RuntimeError):
    pass


@dataclass(frozen=True)
class Engine:
    filename: str
    finisher: str
    cleanup_trigger: str


ENGINES = (
    Engine(
        filename="attack_support_waves.inc",
        finisher="as_finish_motor",
        cleanup_trigger="attack_support/motor_cleanup",
    ),
    Engine(
        filename="enemy_attack_support.inc",
        finisher="ea_finish_motor",
        cleanup_trigger="enemy_attack/motor_cleanup",
    ),
)


def balanced_block(
    text: str, marker: str, opener: str, closer: str
) -> tuple[int, int, str]:
    marker_at = text.find(marker)
    if marker_at < 0:
        raise PatchError(f"Missing marker: {marker}")
    start = text.find(opener, marker_at)
    if start < 0:
        raise PatchError(f"Missing {opener} after marker: {marker}")

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
        elif char == opener:
            depth += 1
        elif char == closer:
            depth -= 1
            if depth == 0:
                return start, index + 1, text[start : index + 1]
    raise PatchError(f"Unbalanced block: {marker}")


def paren_block(text: str, marker: str) -> tuple[int, int, str]:
    return balanced_block(text, marker, "(", ")")


def brace_block(text: str, marker: str) -> tuple[int, int, str]:
    return balanced_block(text, marker, "{", "}")


def replace_exact_once(block: str, old: str, new: str, label: str) -> str:
    if new in block and old not in block:
        return block
    count = block.count(old)
    if count != 1:
        raise PatchError(f"{label}: expected exactly one {old!r}, found {count}")
    return block.replace(old, new, 1)


def patch_engine(text: str, engine: Engine) -> str:
    finisher_marker = f'(define "{engine.finisher}"'
    start, end, finisher = paren_block(text, finisher_marker)
    finisher = replace_exact_once(
        finisher,
        '{"delay" {time 28}}',
        '{"delay" {time 60}}',
        f"{engine.filename}:{engine.finisher}",
    )
    text = text[:start] + finisher + text[end:]

    cleanup_marker = f'{{"{engine.cleanup_trigger}"'
    start, end, cleanup = brace_block(text, cleanup_marker)
    cleanup = replace_exact_once(
        cleanup,
        '{"delay" {time 45}}',
        '{"delay" {time 90}}',
        f"{engine.filename}:{engine.cleanup_trigger}",
    )
    return text[:start] + cleanup + text[end:]


def validate_engine(text: str, engine: Engine) -> None:
    _, _, finisher = paren_block(text, f'(define "{engine.finisher}"')
    if finisher.count('{"delay" {time 60}}') != 1:
        raise PatchError(f"{engine.filename}: expected one 60-second ride")
    if '{"delay" {time 28}}' in finisher:
        raise PatchError(f"{engine.filename}: stale 28-second ride remains")
    if '{emit {mode passengers}}' not in finisher:
        raise PatchError(f"{engine.filename}: passenger-only emit was lost")
    if '{waypoint "0"}' not in finisher:
        raise PatchError(f"{engine.filename}: runtime-proven departure order changed")

    _, _, cleanup = brace_block(text, f'{{"{engine.cleanup_trigger}"')
    if cleanup.count('{"delay" {time 90}}') != 1:
        raise PatchError(f"{engine.filename}: expected one 90-second cleanup")
    if '{"delay" {time 45}}' in cleanup:
        raise PatchError(f"{engine.filename}: stale 45-second cleanup remains")


def patch_multi_root(multi_root: Path, *, check_only: bool = False) -> list[Path]:
    changed: list[Path] = []
    for engine in ENGINES:
        path = multi_root / engine.filename
        if not path.is_file():
            raise PatchError(f"Missing runtime-proven engine: {path}")
        original = path.read_text(encoding="utf-8-sig")
        patched = patch_engine(original, engine)
        validate_engine(patched, engine)
        if patched != original:
            changed.append(path)
            if not check_only:
                path.write_text(patched, encoding="utf-8")
    return changed


def validate_multi_root(multi_root: Path) -> None:
    for engine in ENGINES:
        path = multi_root / engine.filename
        if not path.is_file():
            raise PatchError(f"Missing runtime-proven engine: {path}")
        validate_engine(path.read_text(encoding="utf-8-sig"), engine)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--multi-root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    if args.check:
        validate_multi_root(args.multi_root)
        changed: list[Path] = []
    else:
        changed = patch_multi_root(args.multi_root)

    action = "validated" if args.check else f"patched {len(changed)} file(s)"
    print(
        "Runtime-proven motor timing "
        f"{action}: 60-second ride, 90-second departing-truck cleanup."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
