#!/usr/bin/env python3
"""Deploy exactly two normal AI combat transports in player-defense tests.

The linked NATO FMTV and Russian Ural packages are retained from the successful
comparison test. The older timer-driven transport triggers are disabled while
leaving their placement and ownership helper macros available to the controls.

Normal transports receive one ordinary advance order. They have no scripted
passenger emit, turnaround, withdrawal, deletion, or cleanup.
"""
from __future__ import annotations

import argparse
import importlib.util
import sys
from pathlib import Path

MODULE_PATH = Path(__file__).with_name("apply_transport_control_comparison.py")
spec = importlib.util.spec_from_file_location("_normal_transport_base", MODULE_PATH)
assert spec and spec.loader
base = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = base
spec.loader.exec_module(base)


class PatchError(RuntimeError):
    pass


FILES = (
    "attack_support_waves.inc",
    "faction_support_templates.inc",
    "defense_support_waves.inc",
    "enemy_attack_support.inc",
    "dcg_vars.inc",
)


def balanced_end(text: str, start: int, opener: str, closer: str, label: str) -> int:
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
    raise PatchError(f"Unbalanced block: {label}")


def remove_trigger(text: str, name: str) -> str:
    marker = f'{{"{name}"'
    start = text.find(marker)
    if start < 0:
        return text
    block_start = start
    while block_start > 0 and text[block_start - 1] in "\t ":
        block_start -= 1
    end = balanced_end(text, start, "{", "}", name)
    while end < len(text) and text[end] in "\r\n":
        end += 1
    return text[:block_start] + text[end:]


def replace_unique(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise PatchError(f"{label}: expected one occurrence, found {count}")
    return text.replace(old, new, 1)


def patch_support(text: str, *, friendly: bool) -> str:
    text = base.upsert_before(
        text,
        base.FRIEND_BEGIN if friendly else base.ENEMY_BEGIN,
        base.FRIEND_END if friendly else base.ENEMY_END,
        base.control_section(friendly=friendly),
        '{"defense_support/motor_cleanup"' if friendly else '{"enemy_attack/motor_cleanup"',
    )

    if friendly:
        # Keep ds_place_motor_visible and ds_own_to_defenderbot for the normal
        # FMTV control, but remove the timer-driven one-shot dispatch entirely.
        return remove_trigger(text, "defense_support/motor_test")

    # Remove the forced enemy motor test and prevent the production cmd-19 path
    # from receiving a motor budget. The normal Ural control is independent.
    text = remove_trigger(text, "enemy_attack/motor_test")
    text = replace_unique(
        text,
        '{"set_i" {var "enemy_attack_motor_left$"} {op "="} {value 1}}',
        '{"set_i" {var "enemy_attack_motor_left$"} {op "="} {value 0}}',
        "enemy motor budget",
    )
    text = replace_unique(
        text,
        '{"set_i" {var "enemy_attack_motor_test$"} {op "="} {value 1}}',
        '{"set_i" {var "enemy_attack_motor_test$"} {op "="} {value 0}}',
        "enemy forced motor mode",
    )
    return text


def read_text(path: Path) -> tuple[str, bool]:
    raw = path.read_bytes()
    return raw.decode("utf-8-sig"), raw.startswith(b"\xef\xbb\xbf")


def write_text(path: Path, text: str, bom: bool) -> None:
    raw = text.encode("utf-8")
    path.write_bytes((b"\xef\xbb\xbf" if bom else b"") + raw)


def validate_templates(text: str) -> None:
    for marker in (
        base.PACKAGE_BEGIN,
        base.PACKAGE_END,
        base.TAG_BEGIN,
        base.TAG_END,
    ):
        if text.count(marker) != 1:
            raise PatchError(f"Template marker count invalid: {marker}")
    for token in ('{Entity "fmtv" 0xb500', '{Entity "ural" 0xb510'):
        if text.count(token) != 1:
            raise PatchError(f"Normal transport entity missing or duplicated: {token}")
    for seat in range(1, 9):
        if f'{{0xb500 "seat{seat}"}}' not in text:
            raise PatchError(f"Friendly FMTV seat{seat} link missing")
        if f'{{0xb510 "seat{seat}"}}' not in text:
            raise PatchError(f"Enemy Ural seat{seat} link missing")


def validate(root: Path) -> None:
    multi = root / "resource/map/multi"
    templates = (multi / "faction_support_templates.inc").read_text(encoding="utf-8-sig")
    defense = (multi / "defense_support_waves.inc").read_text(encoding="utf-8-sig")
    enemy = (multi / "enemy_attack_support.inc").read_text(encoding="utf-8-sig")
    variables = (multi / "dcg_vars.inc").read_text(encoding="utf-8-sig")

    validate_templates(templates)
    base.validate_control(defense, friendly=True)
    base.validate_control(enemy, friendly=False)

    if '{"defense_support/motor_test"' in defense:
        raise PatchError("Timer-driven friendly defender transport is still active")
    if '{"enemy_attack/motor_test"' in enemy:
        raise PatchError("Timer-driven enemy attacker transport is still active")

    for token in (
        '{"set_i" {var "enemy_attack_motor_left$"} {op "="} {value 0}}',
        '{"set_i" {var "enemy_attack_motor_test$"} {op "="} {value 0}}',
    ):
        if enemy.count(token) != 1:
            raise PatchError(f"Enemy scripted motor disablement missing: {token}")

    for token in ('{"motor_compare_friend_done"}', '{"motor_compare_enemy_done"}'):
        if variables.count(token) != 1:
            raise PatchError(f"Normal transport variable missing or duplicated: {token}")

    # The only active transport triggers in this player-defense test are the two
    # normal controls. Their blocks contain no scripted lifecycle instructions.
    for text, begin in (
        (defense, base.FRIEND_BEGIN),
        (enemy, base.ENEMY_BEGIN),
    ):
        bounds = base.marked_bounds(
            text,
            begin,
            base.FRIEND_END if begin == base.FRIEND_BEGIN else base.ENEMY_END,
        )
        if not bounds:
            raise PatchError(f"Normal transport section missing: {begin}")
        block = text[bounds[0] : bounds[1]]
        for forbidden in (
            '{"emit"',
            '{mode passengers}',
            'exit_motor_to_origin',
            'motor_cleanup',
            '{"delete"',
            '{"delay" {time 75}}',
        ):
            if forbidden in block:
                raise PatchError(f"Normal transport block contains scripted behavior: {forbidden}")


def apply(root: Path, *, check_only: bool = False) -> list[Path]:
    multi = root / "resource/map/multi"
    paths = {name: multi / name for name in FILES}
    for path in paths.values():
        if not path.is_file():
            raise PatchError(f"Missing required file: {path}")

    attack_before = paths["attack_support_waves.inc"].read_bytes()
    results: dict[str, tuple[str, bool]] = {}
    changed: list[Path] = []

    for name, path in paths.items():
        text, bom = read_text(path)
        if name == "faction_support_templates.inc":
            patched = base.patch_templates(text)
        elif name == "defense_support_waves.inc":
            patched = patch_support(text, friendly=True)
        elif name == "enemy_attack_support.inc":
            patched = patch_support(text, friendly=False)
        elif name == "dcg_vars.inc":
            patched = base.patch_vars(text)
        else:
            patched = text
        results[name] = (patched, bom)
        if patched != text:
            changed.append(path)

    if not check_only:
        for name, (patched, bom) in results.items():
            write_text(paths[name], patched, bom)
        if paths["attack_support_waves.inc"].read_bytes() != attack_before:
            raise PatchError("Friendly-attacker engine changed")
        validate(root)
    return changed


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
