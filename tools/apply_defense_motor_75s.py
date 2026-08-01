#!/usr/bin/env python3
"""Extend both player-defense motor rides from 60 to 75 seconds.

Applied after the movement/origin-exit correction:
- friendly defender: 60 -> 75 seconds before passenger emit
- enemy attacker: retry remains at 2 seconds, remaining ride 58 -> 73 seconds

The friendly-attacker runtime-proven path remains at 60 seconds.
"""
from __future__ import annotations

import argparse
from pathlib import Path


class PatchError(RuntimeError):
    pass


FILES = {
    "ea": "enemy_attack_support.inc",
    "ds": "defense_support_waves.inc",
}

FINISHERS = {
    "ea": "ea_finish_motor",
    "ds": "ds_finish_motor",
}


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


def replace_delay(text: str, prefix: str, old: int, new: int) -> str:
    start, end, block = paren_block(text, FINISHERS[prefix])
    old_token = f'{{"delay" {{time {old}}}}}'
    new_token = f'{{"delay" {{time {new}}}}}'

    if new_token in block and old_token not in block:
        return text
    if block.count(old_token) != 1:
        raise PatchError(
            f"{prefix} finisher expected exactly one {old}-second delay, "
            f"found {block.count(old_token)}"
        )

    patched = block.replace(old_token, new_token, 1)
    return text[:start] + patched + text[end:]


def read_text(path: Path) -> tuple[str, bool]:
    raw = path.read_bytes()
    return raw.decode("utf-8-sig"), raw.startswith(b"\xef\xbb\xbf")


def write_text(path: Path, text: str, bom: bool) -> None:
    raw = text.encode("utf-8")
    path.write_bytes((b"\xef\xbb\xbf" if bom else b"") + raw)


def validate(root: Path) -> None:
    multi = root / "resource/map/multi"
    enemy = (multi / FILES["ea"]).read_text(encoding="utf-8-sig")
    defender = (multi / FILES["ds"]).read_text(encoding="utf-8-sig")
    attacker = (multi / "attack_support_waves.inc").read_text(encoding="utf-8-sig")

    _, _, enemy_finish = paren_block(enemy, FINISHERS["ea"])
    _, _, defender_finish = paren_block(defender, FINISHERS["ds"])
    _, _, attacker_finish = paren_block(attacker, "as_finish_motor")

    if enemy_finish.count('{"delay" {time 2}}') != 1:
        raise PatchError("Enemy movement retry must remain at 2 seconds")
    if enemy_finish.count('{"delay" {time 73}}') != 1:
        raise PatchError("Enemy remaining ride must be exactly 73 seconds")
    if '{"delay" {time 58}}' in enemy_finish:
        raise PatchError("Enemy 58-second remainder was not replaced")
    if enemy_finish.count('{mode passengers}') != 1:
        raise PatchError("Enemy passenger-only emit contract changed")

    if defender_finish.count('{"delay" {time 75}}') != 1:
        raise PatchError("Friendly defender ride must be exactly 75 seconds")
    if '{"delay" {time 60}}' in defender_finish:
        raise PatchError("Friendly defender 60-second delay was not replaced")
    if defender_finish.count('{mode passengers}') != 1:
        raise PatchError("Friendly defender passenger-only emit contract changed")

    if attacker_finish.count('{"delay" {time 60}}') != 1:
        raise PatchError("Friendly-attacker validated 60-second timing changed")


def apply(root: Path, *, check_only: bool = False) -> list[Path]:
    multi = root / "resource/map/multi"
    changed: list[Path] = []
    results: list[tuple[Path, str, bool]] = []

    for prefix, filename in FILES.items():
        path = multi / filename
        if not path.is_file():
            raise PatchError(f"Missing deployed support engine: {path}")
        text, bom = read_text(path)
        if prefix == "ea":
            patched = replace_delay(text, prefix, 58, 73)
        else:
            patched = replace_delay(text, prefix, 60, 75)
        results.append((path, patched, bom))
        if patched != text:
            changed.append(path)

    if not check_only:
        for path, patched, bom in results:
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
        print("Player-defense motor rides validated at 75 seconds.")
    else:
        changed = apply(args.root)
        print(f"75-second defense timing patched {len(changed)} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
