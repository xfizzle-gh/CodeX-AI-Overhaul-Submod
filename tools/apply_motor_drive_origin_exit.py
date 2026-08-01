#!/usr/bin/env python3
"""Correct enemy motor movement and make truck withdrawal side-aware.

This overlay runs after the validated 60/90 baseline and the friendly-defender
one-shot overlay. It makes only two behavioral changes:

1. Reassert the enemy-attacker hull's AI/movement state and advance order two
   seconds after the original order, then wait the remaining 58 seconds before
   passenger emit. Total ride timing remains 60 seconds.
2. Replace generic map waypoint "0" withdrawal with the same named base-entry
   waypoint from which each truck arrived. Friendly paths use the side opposite
   enemy_spawnside; the enemy-attacker path uses enemy_spawnside itself.

Passenger-only emit, linked-package placement, ownership, and cleanup timing are
not changed.
"""
from __future__ import annotations

import argparse
from pathlib import Path


class PatchError(RuntimeError):
    pass


FILES = {
    "as": "attack_support_waves.inc",
    "ea": "enemy_attack_support.inc",
    "ds": "defense_support_waves.inc",
}

FINISHERS = {
    "as": "as_finish_motor",
    "ea": "ea_finish_motor",
    "ds": "ds_finish_motor",
}

HULL_TAGS = {
    "as": "attack_support_motor_hull",
    "ea": "ea_motor_hull",
    "ds": "def_sup_motor_hull",
}

FLAG_TAGS = {
    "as": "attack_support_flag1",
    "ea": "ea_flag1",
    "ds": "def_sup_motor_flag",
}

EXIT_HELPERS = {
    "as": "as_exit_motor_to_origin",
    "ea": "ea_exit_motor_to_origin",
    "ds": "ds_exit_motor_to_origin",
}

# enemy_spawnside 1 means enemy edge A; 2 means enemy edge B.
# Friendly trucks return to the opposite edge. Enemy trucks return to their own.
EXIT_WAYPOINTS = {
    "as": ("attack_support_entry_b", "attack_support_entry_a", "attack_support_entry_b"),
    "ds": ("attack_support_entry_b", "attack_support_entry_a", "attack_support_entry_b"),
    "ea": ("attack_support_entry_a", "attack_support_entry_b", "attack_support_entry_a"),
}

RETRY_MARKER = "; ENEMY MOTOR DRIVE RETRY — ownership/link state settled"


def balanced(text: str, start: int, opener: str, closer: str, label: str) -> int:
    depth = 0
    quoted = False
    escaped = False
    for index in range(start, len(text)):
        char = text[index]
        if quoted:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == '"':
                quoted = False
            continue
        if char == '"':
            quoted = True
        elif char == opener:
            depth += 1
        elif char == closer:
            depth -= 1
            if depth == 0:
                return index + 1
    raise PatchError(f"Unbalanced {label}")


def paren_block(text: str, name: str) -> tuple[int, int, str]:
    marker = f'(define "{name}"'
    start = text.find(marker)
    if start < 0:
        raise PatchError(f"Missing macro {name}")
    end = balanced(text, start, "(", ")", name)
    return start, end, text[start:end]


def action_block_containing(block: str, token: str, label: str) -> tuple[int, int, str]:
    token_at = block.find(token)
    if token_at < 0:
        raise PatchError(f"{label}: missing {token}")
    start = block.rfind('{"action"', 0, token_at)
    if start < 0:
        raise PatchError(f"{label}: action opener not found")
    end = balanced(block, start, "{", "}", label)
    if token_at >= end:
        raise PatchError(f"{label}: token is outside nearest action block")
    return start, end, block[start:end]


def render_exit_helper(prefix: str) -> str:
    helper = EXIT_HELPERS[prefix]
    hull = HULL_TAGS[prefix]
    side1, side2, default = EXIT_WAYPOINTS[prefix]

    def action(waypoint: str, indent: str) -> str:
        return (
            f'{indent}{{"action"\n'
            f'{indent}\t{{selector {{ignore_captured_by_user 0}} {{tag {hull}}}}}\n'
            f'{indent}\t{{drop orders}}\n'
            f'{indent}\t{{action move}}\n'
            f'{indent}\t{{waypoint "{waypoint}"}}\n'
            f'{indent}}}'
        )

    return (
        f'\t\t\t; Return to the same map edge used for insertion. Generic waypoint "0"\n'
        f'\t\t\t; is map-specific and can send an empty truck across the battlefield.\n'
        f'\t\t\t(define "{helper}"\n'
        f'\t\t\t\t{{"switch"\n'
        f'\t\t\t\t\t{{"case"\n'
        f'\t\t\t\t\t\t{{condition {{type cmp_i}} {{var "enemy_spawnside$"}} {{op "=="}} {{value 1}}}}\n'
        f'{action(side1, chr(9)*6)}\n'
        f'\t\t\t\t\t}}\n'
        f'\t\t\t\t\t{{"case"\n'
        f'\t\t\t\t\t\t{{condition {{type cmp_i}} {{var "enemy_spawnside$"}} {{op "=="}} {{value 2}}}}\n'
        f'{action(side2, chr(9)*6)}\n'
        f'\t\t\t\t\t}}\n'
        f'\t\t\t\t\t{{"default"\n'
        f'{action(default, chr(9)*6)}\n'
        f'\t\t\t\t\t}}\n'
        f'\t\t\t\t}}\n'
        f'\t\t\t)'
    )


def upsert_exit_helper(text: str, prefix: str) -> str:
    helper = EXIT_HELPERS[prefix]
    desired = render_exit_helper(prefix)
    marker = f'(define "{helper}"'
    existing = text.find(marker)
    if existing >= 0:
        end = balanced(text, existing, "(", ")", helper)
        return text[:existing] + desired + text[end:]

    finisher_marker = f'\t\t\t(define "{FINISHERS[prefix]}"'
    position = text.find(finisher_marker)
    if position < 0:
        raise PatchError(f"Missing insertion anchor for {helper}")
    return text[:position] + desired + "\n\n" + text[position:]


def patch_exit_call(text: str, prefix: str) -> str:
    start, end, block = paren_block(text, FINISHERS[prefix])
    helper_call = f'("{EXIT_HELPERS[prefix]}")'
    if helper_call in block:
        return text

    action_start, action_end, action = action_block_containing(
        block, '{waypoint "0"}', f"{prefix} exit action"
    )
    hull = HULL_TAGS[prefix]
    if f'{{tag {hull}}}' not in action or '{action move}' not in action:
        raise PatchError(f"{prefix} waypoint 0 action is not the expected hull withdrawal")

    indent_start = block.rfind("\n", 0, action_start) + 1
    indent = block[indent_start:action_start]
    replacement = indent + helper_call
    patched_block = block[:indent_start] + replacement + block[action_end:]
    return text[:start] + patched_block + text[end:]


def patch_enemy_drive_retry(text: str) -> str:
    start, end, block = paren_block(text, FINISHERS["ea"])
    if RETRY_MARKER in block:
        return text
    delay = '\t\t\t\t{"delay" {time 60}}'
    if block.count(delay) != 1:
        raise PatchError(
            f"enemy finisher expected one deployed 60-second delay, found {block.count(delay)}"
        )
    retry = (
        '\t\t\t\t{"delay" {time 2}}\n'
        f'\t\t\t\t{RETRY_MARKER}\n'
        '\t\t\t\t{"actor_state"\n'
        '\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag ea_motor_hull}}\n'
        '\t\t\t\t\t{control AI}\n'
        '\t\t\t\t\t{ai_move {mode enable}}\n'
        '\t\t\t\t\t{move_mode free}\n'
        '\t\t\t\t\t{movement {speed normal} {kind normal} {type normal}}\n'
        '\t\t\t\t}\n'
        '\t\t\t\t{"action"\n'
        '\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag ea_motor_hull}}\n'
        '\t\t\t\t\t{drop orders}\n'
        '\t\t\t\t\t{action advance}\n'
        '\t\t\t\t\t{target {ignore_captured_by_user 0} {tag ea_flag1}}\n'
        '\t\t\t\t}\n'
        '\t\t\t\t{"delay" {time 58}}'
    )
    patched_block = block.replace(delay, retry, 1)
    return text[:start] + patched_block + text[end:]


def read_text(path: Path) -> tuple[str, bool]:
    raw = path.read_bytes()
    return raw.decode("utf-8-sig"), raw.startswith(b"\xef\xbb\xbf")


def write_text(path: Path, text: str, bom: bool) -> None:
    raw = text.encode("utf-8")
    path.write_bytes((b"\xef\xbb\xbf" if bom else b"") + raw)


def validate(root: Path) -> None:
    multi = root / "resource/map/multi"
    texts = {
        prefix: (multi / filename).read_text(encoding="utf-8-sig")
        for prefix, filename in FILES.items()
    }

    for prefix, text in texts.items():
        helper = EXIT_HELPERS[prefix]
        _, _, finisher = paren_block(text, FINISHERS[prefix])
        _, _, helper_block = paren_block(text, helper)
        if f'("{helper}")' not in finisher:
            raise PatchError(f"{prefix} finisher does not call side-aware exit helper")
        if '{waypoint "0"}' in finisher:
            raise PatchError(f"{prefix} finisher still uses generic waypoint 0")
        for waypoint in set(EXIT_WAYPOINTS[prefix]):
            if f'{{waypoint "{waypoint}"}}' not in helper_block:
                raise PatchError(f"{prefix} exit helper missing {waypoint}")
        if finisher.count('{mode passengers}') != 1:
            raise PatchError(f"{prefix} passenger-only emit contract changed")
        if finisher.count(f'{{tag {HULL_TAGS[prefix]}}}') < 3:
            raise PatchError(f"{prefix} hull lifecycle tags are incomplete")

    _, _, enemy = paren_block(texts["ea"], FINISHERS["ea"])
    if RETRY_MARKER not in enemy:
        raise PatchError("Enemy drive retry marker is missing")
    if enemy.count('{"delay" {time 2}}') != 1:
        raise PatchError("Enemy drive retry must wait exactly 2 seconds")
    if enemy.count('{"delay" {time 58}}') != 1:
        raise PatchError("Enemy remaining ride must be exactly 58 seconds")
    if '{"delay" {time 60}}' in enemy:
        raise PatchError("Enemy retry must replace, not add to, the original 60-second delay")
    if enemy.count('{action advance}') < 2:
        raise PatchError("Enemy hull advance order was not reasserted")
    if enemy.count('{target {ignore_captured_by_user 0} {tag ea_flag1}}') < 2:
        raise PatchError("Enemy retry does not target the same selected flag")

    for prefix in ("as", "ds"):
        _, _, finisher = paren_block(texts[prefix], FINISHERS[prefix])
        if finisher.count('{"delay" {time 60}}') != 1:
            raise PatchError(f"{prefix} validated 60-second ride timing changed")


def apply(root: Path, *, check_only: bool = False) -> list[Path]:
    multi = root / "resource/map/multi"
    changed: list[Path] = []
    results: dict[str, tuple[Path, str, bool]] = {}

    for prefix, filename in FILES.items():
        path = multi / filename
        if not path.is_file():
            raise PatchError(f"Missing deployed support engine: {path}")
        text, bom = read_text(path)
        patched = upsert_exit_helper(text, prefix)
        patched = patch_exit_call(patched, prefix)
        if prefix == "ea":
            patched = patch_enemy_drive_retry(patched)
        results[prefix] = (path, patched, bom)
        if patched != text:
            changed.append(path)

    if not check_only:
        for path, patched, bom in results.values():
            write_text(path, patched, bom)
        validate(root)
    return changed


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
