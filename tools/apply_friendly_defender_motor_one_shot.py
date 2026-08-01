#!/usr/bin/env python3
"""Add one friendly-defender motor path derived from the runtime-proven path.

This stage intentionally does not redesign the transport lifecycle. It reads the
already-deployed runtime-proven friendly-attacker blocks, applies only the
namespace/ownership substitutions required by defense_support_waves.inc, and
inserts one forced +30 second defender truck.

The existing attack_support_waves.inc and enemy_attack_support.inc files are
read-only inputs and must remain byte-identical.
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


def replace_exact(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise PatchError(f"{label}: expected one {old!r}, found {count}")
    return text.replace(old, new, 1)


def transform_common(block: str) -> str:
    replacements = (
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
    )
    for old, new in replacements:
        block = block.replace(old, new)
    return block


def derive_placer(attack: str) -> str:
    block = paren_block(attack, '(define "as_place_motor_visible"')[2]
    block = replace_exact(
        block,
        '(define "as_place_motor_visible"',
        '(define "ds_place_motor_visible"',
        "placer name",
    )
    return transform_common(block)


def derive_finisher(attack: str) -> str:
    block = paren_block(attack, '(define "as_finish_motor"')[2]
    block = replace_exact(
        block,
        '(define "as_finish_motor"',
        '(define "ds_finish_motor"',
        "finisher name",
    )
    block = replace_exact(
        block,
        '("am_own_to_support")',
        '("ds_own_to_defenderbot")',
        "defender ownership",
    )
    block = transform_common(block)
    block = block.replace("45s later", "90s later")
    return block


def derive_trigger(attack: str, faction: str) -> str:
    source_name = f'{{"attack_support/ally_{faction}_motor"'
    block = brace_block(attack, source_name)[2]
    replacements = (
        (f'{{"attack_support/ally_{faction}_motor"', f'{{"defense_support/ally_{faction}_motor"'),
        ('{var "user_is_defender$"} {op "=="} {value 0}', '{var "user_is_defender$"} {op "=="} {value 1}'),
        ('attack_support_wave_cmd', 'defense_support_wave_cmd'),
        ('id_attack_support', 'id_defenderbot'),
        ('attack_support_motor_left', 'defense_support_motor_left'),
        ('("as_announce_motor")', '("ds_announce_wave")'),
        ('ATTACK SUPPORT MOTORIZED INSERT', 'DEFENSE SUPPORT MOTORIZED INSERT'),
        ('("as_place_motor_visible")', '("ds_place_motor_visible")'),
        ('("as_finish_motor")', '("ds_finish_motor")'),
    )
    for old, new in replacements:
        block = replace_exact(block, old, new, f"{faction} trigger")
    return transform_common(block)


def derive_cleanup(attack: str) -> str:
    block = brace_block(attack, '{"attack_support/motor_cleanup"')[2]
    block = replace_exact(
        block,
        '{"attack_support/motor_cleanup"',
        '{"defense_support/motor_cleanup"',
        "cleanup name",
    )
    block = replace_exact(
        block,
        '{var "user_is_defender$"} {op "=="} {value 0}',
        '{var "user_is_defender$"} {op "=="} {value 1}',
        "cleanup mission side",
    )
    return transform_common(block)


def render_poke() -> str:
    lines = ['\t\t\t(define "ds_poke_faction_motor"']
    for faction in FACTIONS:
        lines.append(f'\t\t\t\t{{"trigger" {{name "defense_support/ally_{faction}_motor"}}}}')
    lines.append('\t\t\t)')
    return "\n".join(lines)


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
        "\t\t\t; Every lifecycle block below is mechanically derived from the live-proven",
        "\t\t\t; friendly-attacker implementation. Only defender namespace, ownership,",
        "\t\t\t; mission-side gate, and tags differ.",
        derive_placer(attack),
        derive_finisher(attack),
        render_poke(),
    ]
    parts.extend(derive_trigger(attack, faction) for faction in FACTIONS)
    parts.append(render_test_trigger())
    parts.append(derive_cleanup(attack))
    parts.append(END)
    return "\n\n".join(parts)


def upsert_section(defense: str, section: str) -> str:
    begin = defense.find(BEGIN)
    if begin >= 0:
        line_start = defense.rfind("\n", 0, begin) + 1
        end = defense.find(END, begin)
        if end < 0:
            raise PatchError("Existing defender motor section has no end marker")
        line_end = defense.find("\n", end)
        if line_end < 0:
            line_end = len(defense)
        else:
            line_end += 1
        return defense[:line_start] + section + "\n" + defense[line_end:]

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
    addition = anchor + "\n" + "\n".join(token for token in tokens if token not in text)
    return text.replace(anchor, addition, 1)


def validate(root: Path) -> None:
    multi = root / "resource/map/multi"
    attack = (multi / "attack_support_waves.inc").read_text(encoding="utf-8-sig")
    defense = (multi / "defense_support_waves.inc").read_text(encoding="utf-8-sig")
    variables = (multi / "dcg_vars.inc").read_text(encoding="utf-8-sig")

    if attack.count('{"delay" {time 60}}') < 1:
        raise PatchError("Runtime-proven 60-second attack motor baseline is not deployed")
    if BEGIN not in defense or END not in defense:
        raise PatchError("Friendly defender motor section is missing")
    for token in (
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
        'def_sup_motor_hull',
        'def_sup_motor_pax',
        'def_sup_motor_leaving',
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
    defense_raw = defense_path.read_bytes()
    vars_raw = vars_path.read_bytes()
    defense = defense_raw.decode("utf-8-sig")
    variables = vars_raw.decode("utf-8-sig")

    section = build_section(attack)
    patched_defense = upsert_section(defense, section)
    patched_vars = patch_vars(variables)
    changed: list[Path] = []

    if patched_defense != defense:
        changed.append(defense_path)
        if not check_only:
            defense_path.write_text(patched_defense, encoding="utf-8")
    if patched_vars != variables:
        changed.append(vars_path)
        if not check_only:
            vars_path.write_text(patched_vars, encoding="utf-8")

    if not check_only:
        if attack_path.read_bytes() != attack_before:
            raise PatchError("Existing runtime-proven friendly-attacker engine changed")
        if enemy_path.read_bytes() != enemy_before:
            raise PatchError("Existing runtime-proven enemy-attacker engine changed")
        validate(root)
    else:
        # Validate the would-be result without writing by using structural checks here.
        if BEGIN not in patched_defense or '{"delay" {time 60}}' not in patched_defense:
            raise PatchError("Would-be defender motor section failed validation")
        if '{"defense_support_motor_left"}' not in patched_vars:
            raise PatchError("Would-be defender motor variables failed validation")
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
        changed = apply(args.root)
        print(f"Friendly defender motor one-shot patched {len(changed)} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
