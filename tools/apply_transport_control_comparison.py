#!/usr/bin/env python3
"""Add two attack-only transport controls to a player-defense test.

This experiment runs beside the scripted 75-second insert lifecycle:

- friendly control: one NATO FMTV package owned by DefenderBot
- enemy control: one Russian Ural package owned by the attacking enemy AI

Each control package is placed as one linked vehicle at its own entry edge and
receives one ordinary advance order toward an active flag. It has no scripted
passenger emit, no timed turnaround, no withdrawal order, and no cleanup. The
engine therefore decides whether and when its linked passengers dismount.
"""
from __future__ import annotations

import argparse
from pathlib import Path


class PatchError(RuntimeError):
    pass


TEMPLATE_PACKAGE_BEGIN = "; BEGIN TRANSPORT CONTROL COMPARISON PACKAGES"
TEMPLATE_PACKAGE_END = "; END TRANSPORT CONTROL COMPARISON PACKAGES"
TEMPLATE_TAG_BEGIN = "; BEGIN TRANSPORT CONTROL COMPARISON TAGS"
TEMPLATE_TAG_END = "; END TRANSPORT CONTROL COMPARISON TAGS"
FRIEND_SECTION_BEGIN = "; BEGIN FRIENDLY NORMAL-COMBAT TRANSPORT CONTROL"
FRIEND_SECTION_END = "; END FRIENDLY NORMAL-COMBAT TRANSPORT CONTROL"
ENEMY_SECTION_BEGIN = "; BEGIN ENEMY NORMAL-COMBAT TRANSPORT CONTROL"
ENEMY_SECTION_END = "; END ENEMY NORMAL-COMBAT TRANSPORT CONTROL"

FILES = (
    "faction_support_templates.inc",
    "defense_support_waves.inc",
    "enemy_attack_support.inc",
    "dcg_vars.inc",
)

FRIEND_PACKAGE = "motor_compare_friend_nato"
FRIEND_HULL = "motor_compare_friend_hull"
FRIEND_PAX = "motor_compare_friend_pax"
ENEMY_PACKAGE = "motor_compare_enemy_rusa"
ENEMY_HULL = "motor_compare_enemy_hull"
ENEMY_PAX = "motor_compare_enemy_pax"


def marked_section(text: str, begin: str, end: str) -> tuple[int, int] | None:
    begin_at = text.find(begin)
    if begin_at < 0:
        return None
    end_at = text.find(end, begin_at)
    if end_at < 0:
        raise PatchError(f"Existing section {begin} has no end marker")
    line_start = text.rfind("\n", 0, begin_at) + 1
    line_end = text.find("\n", end_at)
    line_end = len(text) if line_end < 0 else line_end + 1
    return line_start, line_end


def upsert_before(text: str, begin: str, end: str, section: str, anchor: str) -> str:
    existing = marked_section(text, begin, end)
    rendered = section.rstrip() + "\n"
    if existing:
        return text[: existing[0]] + rendered + text[existing[1] :]
    anchor_at = text.find(anchor)
    if anchor_at < 0:
        raise PatchError(f"Missing insertion anchor for {begin}: {anchor}")
    line_start = text.rfind("\n", 0, anchor_at) + 1
    return text[:line_start] + rendered + "\n" + text[line_start:]


def render_templates() -> str:
    return f'''{TEMPLATE_PACKAGE_BEGIN}
; Independent control A: friendly NATO FMTV with two cab crew and eight linked passengers.
\t{{Entity "fmtv" 0xb500
\t\t{{Position -8200 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9900}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/nato/2022s/usarmy_crew" 0xb501
\t\t{{Position -8195 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9901}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/nato/2022s/usarmy_crew" 0xb502
\t\t{{Position -8190 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9902}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/nato/2022s/nato_squadlead" 0xb503
\t\t{{Position -8185 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9903}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/nato/2022s/nato_rifleman" 0xb504
\t\t{{Position -8180 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9904}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/nato/2022s/nato_rifleman" 0xb505
\t\t{{Position -8175 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9905}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/nato/2022s/nato_mg" 0xb506
\t\t{{Position -8170 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9906}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/nato/2022s/nato_rifleman" 0xb507
\t\t{{Position -8165 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9907}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/nato/2022s/nato_rifleman" 0xb508
\t\t{{Position -8160 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9908}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/nato/2022s/nato_mg" 0xb509
\t\t{{Position -8155 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9909}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/nato/2022s/nato_antitank" 0xb50a
\t\t{{Position -8150 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9910}}
\t\t{{Able "-select"}}
\t}}

; Independent control B: enemy Russian Ural with two cab crew and eight linked passengers.
\t{{Entity "ural" 0xb510
\t\t{{Position -8000 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9920}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/rusa/2022s/rus_vehicleman" 0xb511
\t\t{{Position -7995 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9921}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/rusa/2022s/rus_vehicleman" 0xb512
\t\t{{Position -7990 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9922}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/rusa/2022s/rus90_squadlead" 0xb513
\t\t{{Position -7985 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9923}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/rusa/2022s/rus90_rifleman" 0xb514
\t\t{{Position -7980 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9924}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/rusa/2022s/rus90_rifleman" 0xb515
\t\t{{Position -7975 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9925}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/rusa/2022s/rus90_mg" 0xb516
\t\t{{Position -7970 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9926}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/rusa/2022s/rus90_rifleman" 0xb517
\t\t{{Position -7965 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9927}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/rusa/2022s/rus90_rifleman" 0xb518
\t\t{{Position -7960 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9928}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/rusa/2022s/rus90_mg" 0xb519
\t\t{{Position -7955 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9929}}
\t\t{{Able "-select"}}
\t}}
\t{{Human "mp/rusa/2022s/rus90_antitank" 0xb51a
\t\t{{Position -7950 -37000}}
\t\t{{Player 0}}
\t\t{{MID 9930}}
\t\t{{Able "-select"}}
\t}}

\t{{Link 0xb501 {{0xb500 "driver"}}}}
\t{{Link 0xb502 {{0xb500 "commander"}}}}
\t{{Link 0xb503 {{0xb500 "seat1"}}}}
\t{{Link 0xb504 {{0xb500 "seat2"}}}}
\t{{Link 0xb505 {{0xb500 "seat3"}}}}
\t{{Link 0xb506 {{0xb500 "seat4"}}}}
\t{{Link 0xb507 {{0xb500 "seat5"}}}}
\t{{Link 0xb508 {{0xb500 "seat6"}}}}
\t{{Link 0xb509 {{0xb500 "seat7"}}}}
\t{{Link 0xb50a {{0xb500 "seat8"}}}}
\t{{Link 0xb511 {{0xb510 "driver"}}}}
\t{{Link 0xb512 {{0xb510 "commander"}}}}
\t{{Link 0xb513 {{0xb510 "seat1"}}}}
\t{{Link 0xb514 {{0xb510 "seat2"}}}}
\t{{Link 0xb515 {{0xb510 "seat3"}}}}
\t{{Link 0xb516 {{0xb510 "seat4"}}}}
\t{{Link 0xb517 {{0xb510 "seat5"}}}}
\t{{Link 0xb518 {{0xb510 "seat6"}}}}
\t{{Link 0xb519 {{0xb510 "seat7"}}}}
\t{{Link 0xb51a {{0xb510 "seat8"}}}}
{TEMPLATE_PACKAGE_END}'''


def render_tags() -> str:
    friend_lines = [
        f'\t{{Tags "motor_compare_tpl" "{FRIEND_PACKAGE}" "{FRIEND_HULL}" "hidden" 0xb500}}',
        f'\t{{Tags "motor_compare_tpl" "{FRIEND_PACKAGE}" "motor_compare_friend_crew" "hidden" 0xb501}}',
        f'\t{{Tags "motor_compare_tpl" "{FRIEND_PACKAGE}" "motor_compare_friend_crew" "hidden" 0xb502}}',
    ]
    friend_lines.extend(
        f'\t{{Tags "motor_compare_tpl" "{FRIEND_PACKAGE}" "{FRIEND_PAX}" "hidden" 0xb50{suffix}}}'
        for suffix in ("3", "4", "5", "6", "7", "8", "9", "a")
    )
    enemy_lines = [
        f'\t{{Tags "motor_compare_tpl" "{ENEMY_PACKAGE}" "{ENEMY_HULL}" "hidden" 0xb510}}',
        f'\t{{Tags "motor_compare_tpl" "{ENEMY_PACKAGE}" "motor_compare_enemy_crew" "hidden" 0xb511}}',
        f'\t{{Tags "motor_compare_tpl" "{ENEMY_PACKAGE}" "motor_compare_enemy_crew" "hidden" 0xb512}}',
    ]
    enemy_lines.extend(
        f'\t{{Tags "motor_compare_tpl" "{ENEMY_PACKAGE}" "{ENEMY_PAX}" "hidden" 0xb51{suffix}}}'
        for suffix in ("3", "4", "5", "6", "7", "8", "9", "a")
    )
    return "\n".join(
        [TEMPLATE_TAG_BEGIN, *friend_lines, *enemy_lines, TEMPLATE_TAG_END]
    )


def render_flag_pick(flag_tag: str, indent: str) -> str:
    return f'''{indent}{{"entity_state"
{indent}\t{{selector {{tag {flag_tag}}}}}
{indent}\t{{tag_remove {flag_tag}}}
{indent}}}
{indent}{{"entity_state"
{indent}\t{{selector
{indent}\t\t{{source advanced}}
{indent}\t\t{{group
{indent}\t\t\t{{select {{tag {{tag flag}}}}}}
{indent}\t\t\t{{exclude {{state {{state inactive}}}}}}
{indent}\t\t}}
{indent}\t\t{{sort {{type shuffle}}}}
{indent}\t\t{{amount 1}}
{indent}\t}}
{indent}\t{{tag_add {flag_tag}}}
{indent}}}'''


def render_control_section(*, friendly: bool) -> str:
    if friendly:
        begin, end = FRIEND_SECTION_BEGIN, FRIEND_SECTION_END
        trigger = "defense_support/motor_compare_normal_transport"
        done = "motor_compare_friend_done"
        armed = "defense_support_armed"
        army = "faction_support_army"
        owner_id = "id_defenderbot"
        package, hull = FRIEND_PACKAGE, FRIEND_HULL
        shared_deploy = "def_sup_deploy"
        placer = "ds_place_motor_visible"
        owner = "ds_own_to_defenderbot"
        flag = "motor_compare_friend_flag"
        title = "NORMAL CONTROL: NATO FMTV ATTACK-ONLY"
        army_value = 3
    else:
        begin, end = ENEMY_SECTION_BEGIN, ENEMY_SECTION_END
        trigger = "enemy_attack/motor_compare_normal_transport"
        done = "motor_compare_enemy_done"
        armed = "enemy_attack_armed"
        army = "enemy_attack_army"
        owner_id = "id_1st_enemy"
        package, hull = ENEMY_PACKAGE, ENEMY_HULL
        shared_deploy = "ea_deploy"
        placer = "ea_place_motor_visible"
        owner = "ea_own_to_enemy"
        flag = "motor_compare_enemy_flag"
        title = "NORMAL CONTROL: RUSSIAN URAL ATTACK-ONLY"
        army_value = 1

    return f'''{begin}
\t\t\t; +45s control: whole linked package, one ordinary flag advance order.
\t\t\t; Deliberately no scripted emit, turnaround, withdrawal, or cleanup.
\t\t\t{{"{trigger}"
\t\t\t\t{{condition
\t\t\t\t\t{{expression "1 & 2 & 3 & 4 & 5 & 6"}}
\t\t\t\t\t{{terms
\t\t\t\t\t\t{{"1.cmp_i" {{var "user_is_defender$"}} {{op "=="}} {{value 1}}}}
\t\t\t\t\t\t{{"2.cmp_i" {{var "{armed}$"}} {{op "=="}} {{value 1}}}}
\t\t\t\t\t\t{{"3.cmp_i" {{var "{army}$"}} {{op "=="}} {{value {army_value}}}}}
\t\t\t\t\t\t{{"4.cmp_i" {{var "{owner_id}$"}} {{op ">"}} {{value 0}}}}
\t\t\t\t\t\t{{"5.cmp_i" {{var "{done}$"}} {{op "=="}} {{value 0}}}}
\t\t\t\t\t\t{{"6.entities" {{selector {{tag {hull}}}} {{count {{op ">="}} {{value 1}}}}}}
\t\t\t\t\t}}
\t\t\t\t}}
\t\t\t\t{{actions
\t\t\t\t\t{{"set_i" {{var "{done}$"}} {{op "="}} {{value 1}}}}
\t\t\t\t\t{{"delay" {{time 45}}}}
\t\t\t\t\t{{"switch"
\t\t\t\t\t\t{{"case"
\t\t\t\t\t\t\t{{condition {{type cmp_i}} {{var "support_debug$"}} {{op "=="}} {{value 1}}}}
\t\t\t\t\t\t\t{{"timer" {{time 8}} {{title "{title}"}}}}
\t\t\t\t\t\t}}
\t\t\t\t\t\t{{"default"}}
\t\t\t\t\t}}
\t\t\t\t\t{{"entity_state"
\t\t\t\t\t\t{{selector {{source advanced}} {{group {{select {{tag {{tag {package}}}}}}}}}}}
\t\t\t\t\t\t{{tag_add {shared_deploy}}}
\t\t\t\t\t}}
\t\t\t\t\t("{placer}")
\t\t\t\t\t{{"entity_state"
\t\t\t\t\t\t{{selector {{source advanced}} {{group {{select {{tag {{tag {package}}}}}}}}}}}
\t\t\t\t\t\t{{tag_remove motor_compare_tpl}}
\t\t\t\t\t\t{{tag_remove hidden}}
\t\t\t\t\t\t{{inactive off}}
\t\t\t\t\t\t{{impregnability disabled}}
\t\t\t\t\t\t{{discovered on}}
\t\t\t\t\t}}
\t\t\t\t\t{{"delay" {{time 0.2}}}}
\t\t\t\t\t("{owner}")
\t\t\t\t\t{{"delay" {{time 0.4}}}}
\t\t\t\t\t{{"actor_state"
\t\t\t\t\t\t{{selector {{ignore_captured_by_user 0}} {{tag {package}}}}}
\t\t\t\t\t\t{{control AI}}
\t\t\t\t\t\t{{ai_move {{mode enable}}}}
\t\t\t\t\t\t{{weapon_prepare on}}
\t\t\t\t\t\t{{fire_mode open}}
\t\t\t\t\t\t{{move_mode free}}
\t\t\t\t\t\t{{movement {{speed normal}} {{kind normal}} {{type normal}}}}
\t\t\t\t\t\t{{ai {{no_retreat on}} {{advance_ratio 1}} {{retreat_ratio 0}}}}
\t\t\t\t\t}}
{render_flag_pick(flag, chr(9) * 5)}
\t\t\t\t\t{{"action"
\t\t\t\t\t\t{{selector {{ignore_captured_by_user 0}} {{tag {hull}}}}}
\t\t\t\t\t\t{{drop orders}}
\t\t\t\t\t\t{{action advance}}
\t\t\t\t\t\t{{target {{ignore_captured_by_user 0}} {{tag {flag}}}}}
\t\t\t\t\t}}
\t\t\t\t\t{{"entity_state"
\t\t\t\t\t\t{{selector {{tag {package}}}}}
\t\t\t\t\t\t{{tag_remove {shared_deploy}}}
\t\t\t\t\t}}
\t\t\t\t}}
\t\t\t}}
{end}'''


def patch_templates(text: str) -> str:
    text = upsert_before(
        text,
        TEMPLATE_PACKAGE_BEGIN,
        TEMPLATE_PACKAGE_END,
        render_templates(),
        "; ===== E2 AIR PACKAGE POOLS",
    )
    text = upsert_before(
        text,
        TEMPLATE_TAG_BEGIN,
        TEMPLATE_TAG_END,
        render_tags(),
        '\t{Tags "ally_sup_tpl" "support_e2_tpl"',
    )
    return text


def patch_vars(text: str) -> str:
    tokens = (
        '\t\t\t{"motor_compare_friend_done"}',
        '\t\t\t{"motor_compare_enemy_done"}',
    )
    if all(token in text for token in tokens):
        return text
    anchor = '\t\t\t{"enemy_attack_motor_test_done"}'
    if text.count(anchor) != 1:
        raise PatchError("dcg_vars comparison anchor is not unique")
    missing = "\n".join(token for token in tokens if token not in text)
    return text.replace(anchor, anchor + "\n" + missing, 1)


def patch_support(text: str, *, friendly: bool) -> str:
    if friendly:
        return upsert_before(
            text,
            FRIEND_SECTION_BEGIN,
            FRIEND_SECTION_END,
            render_control_section(friendly=True),
            '\t\t\t{"defense_support/motor_cleanup"',
        )
    return upsert_before(
        text,
        ENEMY_SECTION_BEGIN,
        ENEMY_SECTION_END,
        render_control_section(friendly=False),
        '\t\t\t{"enemy_attack/motor_cleanup"',
    )


def read_text(path: Path) -> tuple[str, bool]:
    raw = path.read_bytes()
    return raw.decode("utf-8-sig"), raw.startswith(b"\xef\xbb\xbf")


def write_text(path: Path, text: str, bom: bool) -> None:
    raw = text.encode("utf-8")
    path.write_bytes((b"\xef\xbb\xbf" if bom else b"") + raw)


def validate_control(text: str, *, friendly: bool) -> None:
    begin = FRIEND_SECTION_BEGIN if friendly else ENEMY_SECTION_BEGIN
    end = FRIEND_SECTION_END if friendly else ENEMY_SECTION_END
    package = FRIEND_PACKAGE if friendly else ENEMY_PACKAGE
    hull = FRIEND_HULL if friendly else ENEMY_HULL
    trigger = (
        "defense_support/motor_compare_normal_transport"
        if friendly
        else "enemy_attack/motor_compare_normal_transport"
    )
    bounds = marked_section(text, begin, end)
    if not bounds:
        raise PatchError(f"Missing control section {begin}")
    block = text[bounds[0] : bounds[1]]
    for token in (
        f'{{"{trigger}"',
        '{"delay" {time 45}}',
        f'{{tag {package}}}',
        f'{{tag {hull}}}',
        '{action advance}',
        '{tag flag}',
        '{control AI}',
        '{ai_move {mode enable}}',
    ):
        if token not in block:
            raise PatchError(f"{trigger} missing {token}")
    for forbidden in (
        '{"emit"',
        '{mode passengers}',
        'motor_leaving',
        'exit_motor_to_origin',
        'motor_cleanup',
        '{"delete"',
    ):
        if forbidden in block:
            raise PatchError(f"{trigger} must not contain {forbidden}")
    if block.count('{action advance}') != 1:
        raise PatchError(f"{trigger} must issue exactly one ordinary advance order")


def validate(root: Path) -> None:
    multi = root / "resource/map/multi"
    templates = (multi / "faction_support_templates.inc").read_text(encoding="utf-8-sig")
    defense = (multi / "defense_support_waves.inc").read_text(encoding="utf-8-sig")
    enemy = (multi / "enemy_attack_support.inc").read_text(encoding="utf-8-sig")
    variables = (multi / "dcg_vars.inc").read_text(encoding="utf-8-sig")

    for marker in (
        TEMPLATE_PACKAGE_BEGIN,
        TEMPLATE_PACKAGE_END,
        TEMPLATE_TAG_BEGIN,
        TEMPLATE_TAG_END,
    ):
        if templates.count(marker) != 1:
            raise PatchError(f"Template comparison marker count invalid: {marker}")

    for entity in ('{Entity "fmtv" 0xb500', '{Entity "ural" 0xb510'):
        if templates.count(entity) != 1:
            raise PatchError(f"Control entity missing or duplicated: {entity}")
    for mid in range(9900, 9911):
        if templates.count(f'{{MID {mid}}}') != 1:
            raise PatchError(f"Friendly control MID {mid} missing or duplicated")
    for mid in range(9920, 9931):
        if templates.count(f'{{MID {mid}}}') != 1:
            raise PatchError(f"Enemy control MID {mid} missing or duplicated")
    for seat in range(1, 9):
        if f'{{0xb500 "seat{seat}"}}' not in templates:
            raise PatchError(f"Friendly control seat{seat} link missing")
        if f'{{0xb510 "seat{seat}"}}' not in templates:
            raise PatchError(f"Enemy control seat{seat} link missing")

    validate_control(defense, friendly=True)
    validate_control(enemy, friendly=False)

    for token in (
        '{"motor_compare_friend_done"}',
        '{"motor_compare_enemy_done"}',
    ):
        if variables.count(token) != 1:
            raise PatchError(f"dcg_vars missing or duplicating {token}")

    # The scripted experiment remains present beside the control paths.
    for text, finisher in (
        (defense, '(define "ds_finish_motor"'),
        (enemy, '(define "ea_finish_motor"'),
    ):
        for token in (
            finisher,
            '{mode passengers}',
            '; TIMED DROP ALIGNMENT — BEGIN ORIGIN TURN BEFORE PASSENGER EMIT',
            '; KEEP DISEMBARKED INFANTRY ATTACKING AFTER HULL WITHDRAWS',
        ):
            if token not in text:
                raise PatchError(f"Scripted comparison path missing {token}")


def apply(root: Path, *, check_only: bool = False) -> list[Path]:
    multi = root / "resource/map/multi"
    paths = {name: multi / name for name in FILES}
    for path in paths.values():
        if not path.is_file():
            raise PatchError(f"Missing required file: {path}")

    results: dict[str, tuple[str, bool]] = {}
    changed: list[Path] = []
    for name, path in paths.items():
        text, bom = read_text(path)
        if name == "faction_support_templates.inc":
            patched = patch_templates(text)
        elif name == "defense_support_waves.inc":
            patched = patch_support(text, friendly=True)
        elif name == "enemy_attack_support.inc":
            patched = patch_support(text, friendly=False)
        else:
            patched = patch_vars(text)
        results[name] = (patched, bom)
        if patched != text:
            changed.append(path)

    if not check_only:
        for name, (patched, bom) in results.items():
            write_text(paths[name], patched, bom)
        validate(root)
    return changed


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    if args.check:
        validate(args.root)
        print("Transport comparison validated: scripted inserts plus two normal controls.")
    else:
        changed = apply(args.root)
        print(f"Transport comparison patched {len(changed)} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
