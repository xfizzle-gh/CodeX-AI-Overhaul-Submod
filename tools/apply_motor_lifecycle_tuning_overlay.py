#!/usr/bin/env python3
"""Finalize validated motor timing, withdrawal routing, and cleanup lifetime.

Applies only to the two currently runtime-validated attacker-side motor engines:

* friendly attacker support in attack_support_waves.inc
* enemy attacker support in enemy_attack_support.inc

The canonical whole-linked-package/base-entry placement remains untouched.
This overlay changes three lifecycle details:

* passengers dismount 45 seconds after the inward drive order;
* empty trucks return to the same base-entry side they spawned from, rather
  than using map-global waypoint "0";
* departing trucks remain alive for 90 seconds after dismount before cleanup.
"""

from __future__ import annotations

import argparse
import re
from dataclasses import dataclass
from pathlib import Path


class PatchError(RuntimeError):
    pass


@dataclass(frozen=True)
class EngineSpec:
    filename: str
    finish_marker: str
    cleanup_marker: str
    hull_tag: str
    leaving_tag: str
    side_one_entry: str
    side_two_entry: str
    default_entry: str


SPECS = (
    EngineSpec(
        filename="attack_support_waves.inc",
        finish_marker='(define "as_finish_motor"',
        cleanup_marker='{"attack_support/motor_cleanup"',
        hull_tag="attack_support_motor_hull",
        leaving_tag="am_motor_leaving",
        side_one_entry="attack_support_entry_b",
        side_two_entry="attack_support_entry_a",
        default_entry="attack_support_entry_b",
    ),
    EngineSpec(
        filename="enemy_attack_support.inc",
        finish_marker='(define "ea_finish_motor"',
        cleanup_marker='{"enemy_attack/motor_cleanup"',
        hull_tag="ea_motor_hull",
        leaving_tag="ea_motor_leaving",
        side_one_entry="attack_support_entry_a",
        side_two_entry="attack_support_entry_b",
        default_entry="attack_support_entry_a",
    ),
)


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


def named_brace_block(text: str, marker: str) -> tuple[int, int, str]:
    start = text.find(marker)
    if start < 0:
        raise PatchError(f"Missing trigger marker: {marker}")

    brace = text.find("{", start)
    if brace < 0:
        raise PatchError(f"Missing opening brace after: {marker}")

    depth = 0
    in_string = False
    escaped = False
    for index in range(brace, len(text)):
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
        elif char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return brace, index + 1, text[brace : index + 1]

    raise PatchError(f"Unbalanced trigger: {marker}")


def withdrawal_switch(spec: EngineSpec, indent: str) -> str:
    inner = indent + "\t"
    deep = inner + "\t"
    return (
        f'{indent}; Return to the same base-entry side used for deployment.\n'
        f'{indent}; Map-global waypoint "0" can lie forward of the drop zone.\n'
        f'{indent}{{"switch"\n'
        f'{inner}{{"case"\n'
        f'{deep}{{condition {{type cmp_i}} {{var "enemy_spawnside$"}} {{op "=="}} {{value 1}}}}\n'
        f'{deep}{{"action"\n'
        f'{deep}\t{{selector {{ignore_captured_by_user 0}} {{tag {spec.hull_tag}}}}\n'
        f'{deep}\t{{drop orders}}\n'
        f'{deep}\t{{action move}}\n'
        f'{deep}\t{{waypoint "{spec.side_one_entry}"}}\n'
        f'{deep}}}\n'
        f'{inner}}}\n'
        f'{inner}{{"case"\n'
        f'{deep}{{condition {{type cmp_i}} {{var "enemy_spawnside$"}} {{op "=="}} {{value 2}}}}\n'
        f'{deep}{{"action"\n'
        f'{deep}\t{{selector {{ignore_captured_by_user 0}} {{tag {spec.hull_tag}}}}\n'
        f'{deep}\t{{drop orders}}\n'
        f'{deep}\t{{action move}}\n'
        f'{deep}\t{{waypoint "{spec.side_two_entry}"}}\n'
        f'{deep}}}\n'
        f'{inner}}}\n'
        f'{inner}{{"default"\n'
        f'{deep}{{"action"\n'
        f'{deep}\t{{selector {{ignore_captured_by_user 0}} {{tag {spec.hull_tag}}}}\n'
        f'{deep}\t{{drop orders}}\n'
        f'{deep}\t{{action move}}\n'
        f'{deep}\t{{waypoint "{spec.default_entry}"}}\n'
        f'{deep}}}\n'
        f'{inner}}}\n'
        f'{indent}}}'
    )


def patch_finish(text: str, spec: EngineSpec) -> str:
    start, end, block = named_paren_block(text, spec.finish_marker)

    timing_tokens = {
        28: '{"delay" {time 28}}',
        35: '{"delay" {time 35}}',
        45: '{"delay" {time 45}}',
    }
    present = [seconds for seconds, token in timing_tokens.items() if token in block]
    if present == [28] or present == [35]:
        block = block.replace(timing_tokens[present[0]], timing_tokens[45], 1)
    elif present != [45]:
        raise PatchError(
            f"{spec.finish_marker}: expected one 28s, 35s, or 45s travel delay; found {present}"
        )

    if '{waypoint "0"}' in block:
        pattern = re.compile(
            rf'(?P<indent>^[ \t]*)\{{"action"\s*\n'
            rf'^[ \t]*\{{selector \{{ignore_captured_by_user 0\}} \{{tag {re.escape(spec.hull_tag)}\}}\}}\s*\n'
            rf'^[ \t]*\{{drop orders\}}\s*\n'
            rf'^[ \t]*\{{action move\}}\s*\n'
            rf'^[ \t]*\{{waypoint "0"\}}\s*\n'
            rf'^[ \t]*\}}',
            re.MULTILINE,
        )
        match = pattern.search(block)
        if not match:
            raise PatchError(f"{spec.finish_marker}: waypoint 0 withdrawal action was not recognized")
        block = block[: match.start()] + withdrawal_switch(spec, match.group("indent")) + block[match.end() :]
    else:
        required = (spec.side_one_entry, spec.side_two_entry, spec.default_entry)
        if not all(f'{{waypoint "{entry}"}}' in block for entry in required):
            raise PatchError(f"{spec.finish_marker}: withdrawal route is neither waypoint 0 nor base-entry routing")

    return text[:start] + block + text[end:]


def patch_cleanup(text: str, spec: EngineSpec) -> str:
    start, end, block = named_brace_block(text, spec.cleanup_marker)
    old = '{"delay" {time 45}}'
    new = '{"delay" {time 90}}'

    if new in block:
        if old in block:
            raise PatchError(f"{spec.cleanup_marker}: contains both 45s and 90s cleanup delays")
        return text
    if block.count(old) != 1:
        raise PatchError(f"{spec.cleanup_marker}: expected exactly one 45-second cleanup delay")

    block = block.replace(old, new, 1)
    return text[:start] + block + text[end:]


def patch_engine(text: str, spec: EngineSpec) -> str:
    return patch_cleanup(patch_finish(text, spec), spec)


def validate_engine(text: str, spec: EngineSpec) -> None:
    _, _, finish = named_paren_block(text, spec.finish_marker)
    if finish.count('{"delay" {time 45}}') != 1:
        raise PatchError(f"{spec.filename}: expected exactly one 45-second travel delay")
    for stale in ('{"delay" {time 28}}', '{"delay" {time 35}}', '{waypoint "0"}'):
        if stale in finish:
            raise PatchError(f"{spec.filename}: stale lifecycle token remains: {stale}")
    for entry in (spec.side_one_entry, spec.side_two_entry, spec.default_entry):
        if f'{{waypoint "{entry}"}}' not in finish:
            raise PatchError(f"{spec.filename}: missing withdrawal entry {entry}")
    if f'{{tag_add {spec.leaving_tag}}}' not in finish:
        raise PatchError(f"{spec.filename}: departing hull tag is missing")
    if '{emit\n' not in finish and '{"emit"' not in finish:
        raise PatchError(f"{spec.filename}: passenger emit is missing")

    _, _, cleanup = named_brace_block(text, spec.cleanup_marker)
    if cleanup.count('{"delay" {time 90}}') != 1:
        raise PatchError(f"{spec.filename}: expected exactly one 90-second cleanup delay")
    if '{"delay" {time 45}}' in cleanup:
        raise PatchError(f"{spec.filename}: stale 45-second cleanup delay remains")
    if f'{{tag {spec.leaving_tag}}}' not in cleanup:
        raise PatchError(f"{spec.filename}: cleanup no longer selects the departing hull")


def validate_multi_root(multi_root: Path) -> None:
    for spec in SPECS:
        path = multi_root / spec.filename
        if not path.is_file():
            raise PatchError(f"Missing deployed support engine: {path}")
        validate_engine(path.read_text(encoding="utf-8-sig"), spec)


def patch_multi_root(multi_root: Path) -> None:
    for spec in SPECS:
        path = multi_root / spec.filename
        if not path.is_file():
            raise PatchError(f"Missing deployed support engine: {path}")
        text = path.read_text(encoding="utf-8-sig")
        path.write_text(patch_engine(text, spec), encoding="utf-8")

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

    print(
        "Motor lifecycle ready: 45s mounted, return to base entry, "
        "90s post-dismount cleanup."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
