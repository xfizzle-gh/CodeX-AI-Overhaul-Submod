#!/usr/bin/env python3
"""Add one friendly-defender motor path derived from the proven attack path.

The existing runtime-proven attack_support and enemy_attack engines are read-only.
Only defense_support_waves.inc and dcg_vars.inc are modified.
"""
from __future__ import annotations

import argparse
from pathlib import Path

BEGIN = "; BEGIN FRIENDLY DEFENDER MOTOR ONE-SHOT — DERIVED FROM RUNTIME-PROVEN PATH"
END = "; END FRIENDLY DEFENDER MOTOR ONE-SHOT"
FACTIONS = ("rusa", "ukr", "prc", "nato")


class PatchError(RuntimeError):
    pass


def balanced(text: str, marker: str, opener: str, closer: str) -> tuple[int, int, str]:
    marker_at = text.find(marker)
    if marker_at < 0:
        raise PatchError(f"Missing marker: {marker}")
    start = text.find(opener, marker_at)
    if start < 0:
        raise PatchError(f"Missing opener after marker: {marker}")
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
                return start, index + 1, text[start:index + 1]
    raise PatchError(f"Unbalanced block: {marker}")


def paren_block(text: str, marker: str) -> tuple[int, int, str]:
    return balanced(text, marker, "(", ")")


def brace_block(text: str, marker: str) -> tuple[int, int, str]:
    return balanced(text, marker, "{", "}")


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise PatchError(f"{label}: expected one {old!r}, found {count}")
    return text.replace(old, new, 1)


def replace_all(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count < 1:
        raise PatchError(f"{label}: expected at least one {old!r}")
    return text.replace(old, new)


def transform_common(block: str) -> str:
    for old, new in (
        ("attack_support_deploy", "def_sup_deploy"),
        ("attack_support_src", "def_sup_src"),
        ("attack_support_flag1", "def_sup_motor_flag"),
        ("attack_support_motor_hull", "def_sup_motor_hull"),
        ("attack_support_motor_pax", "def_sup_motor_pax"),
        ("am_motor_leaving", "def_sup_motor_leaving"),
        ("attack_support_transferred", "defense_support_transferred"),
        ("attack_support_g1", "def_sup_motor_g1"),
        ("attack_support_g2", "def_sup_motor_g2"),
        ("attack_support_g3", "def_sup_motor_g3"),
        ("attack_support_g4", "def_sup_motor_g4"),
    ):
        block = block.replace(old, new)
    return block


def derive_placer(attack: str) -> str:
    block = paren_block(attack, '(define "as_place_motor_visible"')[2]
    block = replace_once(
        block,
        '(define "as_place_motor_visible"',
        '(define "ds_place_motor_visible"',
        "placer name",
    )
    return transform_common(block)


def derive_finisher(attack: str) -> str:
    block = paren_block(attack, '(define "as_finish_motor"')[2]
    block = replace_once(
        block,
        '(define "as_finish_motor"',
        '(define "ds_finish_motor"',
        "finisher name",
    )
    block = replace_once(
        block,
        '("am_own_to_support")',
        '("ds_own_to_defenderbot")',
        "defender ownership",
    )
    return transform_common(block).replace("45s later", "90s later")


def derive_trigger(attack: str, faction: str) -> str:
    block = brace_block(attack, f'{{"attack_support/ally_{faction}_motor"')[2]
    block = replace_once(
        block,
        f'{{"attack_support/ally_{faction}_motor"',
        f'{{"defense_support/ally_{faction}_motor"',
        f"{faction} trigger name",
    )
    block = replace_once(
        block,
        '{var "user_is_defender$"} {op "=="} {value 0}',
        '{var "user_is_defender$"} {op "=="} {value 1}',
        f"{faction} mission side",
    )
    block = replace_all(
        block,
        "attack_support_wave_cmd",
        "defense_support_wave_cmd",
        f"{faction} command namespace",
    )
    block = replace_all(
        block,
        "attack_support_motor_left",
        "defense_support_motor_left",
        f"{faction} motor budget namespace",
    )
    for old, new, label in (
        ("id_attack_support", "id_defenderbot", "owner gate"),
        ('("as_announce_motor")', '("ds_announce_wave")', "announcement"),
        ("ATTACK SUPPORT MOTORIZED INSERT", "DEFENSE SUPPORT MOTORIZED INSERT", "debug title"),
        ('("as_place_motor_visible")', '("ds_place_motor_visible")', "placement call"),
        ('("as_finish_motor")', '("ds_finish_motor")', "finisher call"),
    ):
        block = replace_once(block, old, new, f"{faction} {label}")
    return transform_common(block)


def derive_cleanup(attack: str) -> str:
    block = brace_block(attack, '{"attack_support/motor_cleanup"')[2]
    block = replace_once(
        block,
        '{"attack_support/motor_cleanup"',
        '{"defense_support/motor_cleanup"',
        "cleanup name",
    )
    block = replace_once(
        block,
        '{var "user_is_defender$"} {op "=="} {value 0}',
        '{var "user_is_defender$"} {op "=="} {value 1}',
        "cleanup mission side",
    )
    return transform_common(block)


def render_poke() -> str:
    body = ['\t\t\t(define "ds_poke_faction_motor"']
    body.extend(
        f'\t\t\t\t{{"trigger" {{name "defense_support/ally_{faction}_motor"}}}}'
        for faction in FACTIONS
    )
    body.append("\t\t\t)")
    return "\n".join(body)


def render_test_trigger() -> str:
    return '''\t\t\t; Stage-2 validation only: exactly one friendly defender truck at +30s.
\t\t\t{"defense_support/motor_test"
\t\t\t\t{condition
\t\t\t\t\t{expression "1 & 2 & 3 & 4"}
\t\t\t\t\t{terms
\t\t\t\t\t\t{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
\t\t\t\t\t\t{"2.cmp_i" {var "defense_support_armed$"} {op "=="} {value 1}}
\t\t\t\t\t\t{"3.cmp_i" {var "prep_inform$"} {op "=="} {value 1}}
\t\t\t\t\t\t{"4.cmp_i" {var "defense_support_motor_test_done$"} {op "=="} {value 0}}
\t\t\t\t\t}
\t\t\t\t}
\t\t\t\t{actions
\t\t\t\t\t{"set_i" {var "defense_support_motor_test_done$"} {op "="} {value 1}}
\t\t\t\t\t{"set_i" {var "defense_support_motor_left$"} {op "="} {value 1}}
\t\t\t\t\t{"delay" {time 30}}
\t\t\t\t\t{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 19}}
\t\t\t\t\t("ds_poke_faction_motor")
\t\t\t\t}
\t\t\t}'''


def build_section(attack: str) -> str:
    parts = [
        BEGIN,
        "\t\t\t; Mechanically derived from the live-proven friendly-attacker path.",
        derive_placer(attack),
        derive_finisher(attack),
        render_poke(),
    ]
    parts.extend(derive_trigger(attack, faction) for faction in FACTIONS)
    parts.extend((render_test_trigger(), derive_cleanup(attack), END))
    return "\n\n".join(parts)


def upsert_section(defense: str, section: str) -> str:
    begin = defense.find(BEGIN)
    if begin >= 0:
        start = defense.rfind("\n", 0, begin) + 1
        end = defense.find(END, begin)
        if end < 0:
            raise PatchError("Existing defender motor section has no end marker")
        end = defense.find("\n", end)
        end = len(defense) if end < 0 else end + 1
        return defense[:start] + section + "\n" + defense[end:]
    anchor = "\t\t\t; Infantry hybrid only (no light veh on defense edge waves)."
    pos = defense.find(anchor)
    if pos < 0:
        raise PatchError("Defense motor insertion anchor is missing")
    return defense[:pos] + section + "\n\n" + defense[pos:]


def patch_vars(text: str) -> str:
    tokens = (
        '\t\t\t{"defense_support_motor_left"}',
        '\t\t\t{"defense_support_motor_test_done"}',
    )
    if all(token in text for token in tokens):
        return text
    anchor = '\t\t\t{"defense_support_place"}'
    if text.count(anchor) != 1:
        raise PatchError("dcg_vars defense_support_place anchor is not unique")
    missing = "\n".join(token for token in tokens if token not in text)
    return text.replace(anchor, anchor + "\n" + missing, 1)


def read_text(path: Path) -> tuple[str, bool]:
    raw = path.read_bytes()
    return raw.decode("utf-8-sig"), raw.startswith(b"\xef\xbb\xbf")


def write_text(path: Path, text: str, bom: bool) -> None:
    raw = text.encode("utf-8")
    path.write_bytes((b"\xef\xbb\xbf" if bom else b"") + raw)


def validate(root: Path) -> None:
    multi = root / "resource/map/multi"
    attack = (multi / "attack_support_waves.inc").read_text(encoding="utf-8-sig")
    defense = (multi / "defense_support_waves.inc").read_text(encoding="utf-8-sig")
    variables = (multi / "dcg_vars.inc").read_text(encoding="utf-8-sig")
    if '{"delay" {time 60}}' not in attack:
        raise PatchError("Runtime-proven 60-second baseline is not deployed")
    for token in (
        BEGIN,
        END,
        '(define "ds_place_motor_visible"',
        '(define "ds_finish_motor"',
        '(define "ds_poke_faction_motor"',
        '{"defense_support/motor_test"',
        '{"defense_support/motor_cleanup"',
        '{"delay" {time 60}}',
        '{"delay" {time 90}}',
        '{"delay" {time 30}}',
        '{mode passengers}',
        '{waypoint "0"}',
        '("ds_own_to_defenderbot")',
    ):
        if token not in defense:
            raise PatchError(f"Defense motor section missing {token}")
    for faction in FACTIONS:
        if f'{{"defense_support/ally_{faction}_motor"' not in defense:
            raise PatchError(f"Missing friendly defender {faction} motor trigger")
    for token in (
        '{"defense_support_motor_left"}',
        '{"defense_support_motor_test_done"}',
    ):
        if token not in variables:
            raise PatchError(f"dcg_vars missing {token}")


def apply(root: Path, *, check_only: bool = False) -> list[Path]:
    multi = root / "resource/map/multi"
    attack_path = multi / "attack_support_waves.inc"
    enemy_path = multi / "enemy_attack_support.inc"
    defense_path = multi / "defense_support_waves.inc"
    vars_path = multi / "dcg_vars.inc"
    for path in (attack_path, enemy_path, defense_path, vars_path):
        if not path.is_file():
            raise PatchError(f"Missing required file: {path}")

    attack_before = attack_path.read_bytes()
    enemy_before = enemy_path.read_bytes()
    attack = attack_before.decode("utf-8-sig")
    defense, defense_bom = read_text(defense_path)
    variables, vars_bom = read_text(vars_path)
    patched_defense = upsert_section(defense, build_section(attack))
    patched_vars = patch_vars(variables)
    changed: list[Path] = []

    if patched_defense != defense:
        changed.append(defense_path)
        if not check_only:
            write_text(defense_path, patched_defense, defense_bom)
    if patched_vars != variables:
        changed.append(vars_path)
        if not check_only:
            write_text(vars_path, patched_vars, vars_bom)

    if not check_only:
        if attack_path.read_bytes() != attack_before:
            raise PatchError("Friendly-attacker engine changed")
        if enemy_path.read_bytes() != enemy_before:
            raise PatchError("Enemy-attacker engine changed")
        validate(root)
    return changed


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if args.check:
        validate(args.root)
        print("Friendly defender motor one-shot validated.")
    else:
        print(f"Friendly defender motor one-shot patched {len(apply(args.root))} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
