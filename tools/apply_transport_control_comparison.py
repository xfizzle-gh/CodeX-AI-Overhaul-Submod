#!/usr/bin/env python3
"""Add two ordinary attack-only transports beside the scripted insert paths.

The player-defense comparison is intentionally restricted to NATO versus Russia:
- DefenderBot receives one linked NATO FMTV package.
- The attacking enemy AI receives one linked Russian Ural package.

Each control receives one ordinary advance order toward an active flag. It has
no scripted passenger emit, turnaround, withdrawal, or cleanup.
"""
from __future__ import annotations

import argparse
from pathlib import Path


class PatchError(RuntimeError):
    pass


PACKAGE_BEGIN = "; BEGIN TRANSPORT CONTROL COMPARISON PACKAGES"
PACKAGE_END = "; END TRANSPORT CONTROL COMPARISON PACKAGES"
TAG_BEGIN = "; BEGIN TRANSPORT CONTROL COMPARISON TAGS"
TAG_END = "; END TRANSPORT CONTROL COMPARISON TAGS"
FRIEND_BEGIN = "; BEGIN FRIENDLY NORMAL-COMBAT TRANSPORT CONTROL"
FRIEND_END = "; END FRIENDLY NORMAL-COMBAT TRANSPORT CONTROL"
ENEMY_BEGIN = "; BEGIN ENEMY NORMAL-COMBAT TRANSPORT CONTROL"
ENEMY_END = "; END ENEMY NORMAL-COMBAT TRANSPORT CONTROL"

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


def marked_bounds(text: str, begin: str, end: str) -> tuple[int, int] | None:
    begin_at = text.find(begin)
    if begin_at < 0:
        return None
    end_at = text.find(end, begin_at)
    if end_at < 0:
        raise PatchError(f"Existing section {begin} has no end marker")
    start = text.rfind("\n", 0, begin_at) + 1
    finish = text.find("\n", end_at)
    finish = len(text) if finish < 0 else finish + 1
    return start, finish


def upsert_before(text: str, begin: str, end: str, section: str, anchor: str) -> str:
    rendered = section.rstrip() + "\n"
    bounds = marked_bounds(text, begin, end)
    if bounds:
        return text[: bounds[0]] + rendered + text[bounds[1] :]
    anchor_at = text.find(anchor)
    if anchor_at < 0:
        raise PatchError(f"Missing insertion anchor for {begin}: {anchor}")
    line_start = text.rfind("\n", 0, anchor_at) + 1
    return text[:line_start] + rendered + "\n" + text[line_start:]


def source_section(text: str, start_marker: str, end_marker: str) -> str:
    start = text.find(start_marker)
    if start < 0:
        raise PatchError(f"Missing source marker {start_marker}")
    end = text.find(end_marker, start)
    if end < 0:
        raise PatchError(f"Missing source end marker {end_marker}")
    return text[start:end].rstrip()


def remap_package(section: str, old_ids: list[int], new_ids: list[int], old_mids: list[int], new_mids: list[int]) -> str:
    if len(old_ids) != len(new_ids) or len(old_mids) != len(new_mids):
        raise PatchError("Package remap lengths differ")
    patched = section
    # Replace longest textual IDs first so no short token can alter another token.
    for old, new in zip(reversed(old_ids), reversed(new_ids)):
        patched = patched.replace(f"0x{old:x}", f"0x{new:x}")
    for old, new in zip(old_mids, new_mids):
        patched = patched.replace(f"{{MID {old}}}", f"{{MID {new}}}")
    return patched


def render_links() -> str:
    lines: list[str] = []
    for hull, first in ((0xB500, 0xB501), (0xB510, 0xB511)):
        lines.append(f'\t{{Link 0x{first:x} {{0x{hull:x} "driver"}}}}')
        lines.append(f'\t{{Link 0x{first + 1:x} {{0x{hull:x} "commander"}}}}')
        for seat in range(1, 9):
            human = first + 1 + seat
            lines.append(f'\t{{Link 0x{human:x} {{0x{hull:x} "seat{seat}"}}}}')
    return "\n".join(lines)


def render_packages(text: str) -> str:
    rusa = source_section(
        text,
        "; ----- ALLY RUSA MOTORIZED (ural) -----",
        "; ----- ALLY UKR MOTORIZED (ural_vsu) -----",
    )
    nato = source_section(
        text,
        "; ----- ALLY NATO MOTORIZED (fmtv) -----",
        "; ----- ALLY PRC MOTORIZED",
    )
    friend = remap_package(
        nato,
        list(range(0xB3B6, 0xB3C1)),
        list(range(0xB500, 0xB50B)),
        list(range(9762, 9773)),
        list(range(9900, 9911)),
    ).replace(
        "; ----- ALLY NATO MOTORIZED (fmtv) -----",
        "; ----- CONTROL FRIENDLY NATO FMTV -----",
        1,
    )
    enemy = remap_package(
        rusa,
        list(range(0xB3A0, 0xB3AB)),
        list(range(0xB510, 0xB51B)),
        list(range(9740, 9751)),
        list(range(9920, 9931)),
    ).replace(
        "; ----- ALLY RUSA MOTORIZED (ural) -----",
        "; ----- CONTROL ENEMY RUSSIAN URAL -----",
        1,
    )
    return "\n".join((PACKAGE_BEGIN, friend, enemy, render_links(), PACKAGE_END))


def render_tags() -> str:
    lines = [TAG_BEGIN]
    for entity in range(0xB500, 0xB50B):
        role = FRIEND_HULL if entity == 0xB500 else (
            "motor_compare_friend_crew" if entity <= 0xB502 else FRIEND_PAX
        )
        lines.append(
            f'\t{{Tags "motor_compare_tpl" "{FRIEND_PACKAGE}" "{role}" "hidden" 0x{entity:x}}}'
        )
    for entity in range(0xB510, 0xB51B):
        role = ENEMY_HULL if entity == 0xB510 else (
            "motor_compare_enemy_crew" if entity <= 0xB512 else ENEMY_PAX
        )
        lines.append(
            f'\t{{Tags "motor_compare_tpl" "{ENEMY_PACKAGE}" "{role}" "hidden" 0x{entity:x}}}'
        )
    lines.append(TAG_END)
    return "\n".join(lines)


def flag_pick(flag: str) -> str:
    return '''
\t\t\t\t\t{"entity_state"
\t\t\t\t\t\t{selector {tag %(flag)s}}
\t\t\t\t\t\t{tag_remove %(flag)s}
\t\t\t\t\t}
\t\t\t\t\t{"entity_state"
\t\t\t\t\t\t{selector
\t\t\t\t\t\t\t{source advanced}
\t\t\t\t\t\t\t{group
\t\t\t\t\t\t\t\t{select {tag {tag flag}}}
\t\t\t\t\t\t\t\t{exclude {state {state inactive}}}
\t\t\t\t\t\t\t}
\t\t\t\t\t\t\t{sort {type shuffle}}
\t\t\t\t\t\t\t{amount 1}
\t\t\t\t\t\t}
\t\t\t\t\t\t{tag_add %(flag)s}
\t\t\t\t\t}''' % {"flag": flag}


def control_section(*, friendly: bool) -> str:
    values = {
        "begin": FRIEND_BEGIN if friendly else ENEMY_BEGIN,
        "end": FRIEND_END if friendly else ENEMY_END,
        "trigger": (
            "defense_support/motor_compare_normal_transport"
            if friendly
            else "enemy_attack/motor_compare_normal_transport"
        ),
        "done": "motor_compare_friend_done" if friendly else "motor_compare_enemy_done",
        "armed": "defense_support_armed" if friendly else "enemy_attack_armed",
        "army": "faction_support_army" if friendly else "enemy_attack_army",
        "army_value": 3 if friendly else 1,
        "owner_id": "id_defenderbot" if friendly else "id_1st_enemy",
        "package": FRIEND_PACKAGE if friendly else ENEMY_PACKAGE,
        "hull": FRIEND_HULL if friendly else ENEMY_HULL,
        "shared": "def_sup_deploy" if friendly else "ea_deploy",
        "placer": "ds_place_motor_visible" if friendly else "ea_place_motor_visible",
        "owner": "ds_own_to_defenderbot" if friendly else "ea_own_to_enemy",
        "flag": "motor_compare_friend_flag" if friendly else "motor_compare_enemy_flag",
        "title": (
            "NORMAL CONTROL: NATO FMTV ATTACK-ONLY"
            if friendly
            else "NORMAL CONTROL: RUSSIAN URAL ATTACK-ONLY"
        ),
    }
    body = '''%(begin)s
\t\t\t; +45s control: whole linked package, one ordinary advance order.
\t\t\t; No scripted emit, turnaround, withdrawal, or cleanup.
\t\t\t{"%(trigger)s"
\t\t\t\t{condition
\t\t\t\t\t{expression "1 & 2 & 3 & 4 & 5 & 6"}
\t\t\t\t\t{terms
\t\t\t\t\t\t{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
\t\t\t\t\t\t{"2.cmp_i" {var "%(armed)s$"} {op "=="} {value 1}}
\t\t\t\t\t\t{"3.cmp_i" {var "%(army)s$"} {op "=="} {value %(army_value)d}}
\t\t\t\t\t\t{"4.cmp_i" {var "%(owner_id)s$"} {op ">"} {value 0}}
\t\t\t\t\t\t{"5.cmp_i" {var "%(done)s$"} {op "=="} {value 0}}
\t\t\t\t\t\t{"6.entities" {selector {tag %(hull)s}} {count {op ">="} {value 1}}}
\t\t\t\t\t}
\t\t\t\t}
\t\t\t\t{actions
\t\t\t\t\t{"set_i" {var "%(done)s$"} {op "="} {value 1}}
\t\t\t\t\t{"delay" {time 45}}
\t\t\t\t\t{"switch"
\t\t\t\t\t\t{"case"
\t\t\t\t\t\t\t{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
\t\t\t\t\t\t\t{"timer" {time 8} {title "%(title)s"}}
\t\t\t\t\t\t}
\t\t\t\t\t\t{"default"}
\t\t\t\t\t}
\t\t\t\t\t{"entity_state"
\t\t\t\t\t\t{selector {source advanced} {group {select {tag {tag %(package)s}}}}}
\t\t\t\t\t\t{tag_add %(shared)s}
\t\t\t\t\t}
\t\t\t\t\t("%(placer)s")
\t\t\t\t\t{"entity_state"
\t\t\t\t\t\t{selector {source advanced} {group {select {tag {tag %(package)s}}}}}
\t\t\t\t\t\t{tag_remove motor_compare_tpl}
\t\t\t\t\t\t{tag_remove hidden}
\t\t\t\t\t\t{inactive off}
\t\t\t\t\t\t{impregnability disabled}
\t\t\t\t\t\t{discovered on}
\t\t\t\t\t}
\t\t\t\t\t{"delay" {time 0.2}}
\t\t\t\t\t("%(owner)s")
\t\t\t\t\t{"delay" {time 0.4}}
\t\t\t\t\t{"actor_state"
\t\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag %(package)s}}
\t\t\t\t\t\t{control AI}
\t\t\t\t\t\t{ai_move {mode enable}}
\t\t\t\t\t\t{weapon_prepare on}
\t\t\t\t\t\t{fire_mode open}
\t\t\t\t\t\t{move_mode free}
\t\t\t\t\t\t{movement {speed normal} {kind normal} {type normal}}
\t\t\t\t\t\t{ai {no_retreat on} {advance_ratio 1} {retreat_ratio 0}}
\t\t\t\t\t}
%(flag_pick)s
\t\t\t\t\t{"action"
\t\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag %(hull)s}}
\t\t\t\t\t\t{drop orders}
\t\t\t\t\t\t{action advance}
\t\t\t\t\t\t{target {ignore_captured_by_user 0} {tag %(flag)s}}
\t\t\t\t\t}
\t\t\t\t\t{"entity_state"
\t\t\t\t\t\t{selector {tag %(package)s}}
\t\t\t\t\t\t{tag_remove %(shared)s}
\t\t\t\t\t}
\t\t\t\t}
\t\t\t}
%(end)s'''
    values["flag_pick"] = flag_pick(values["flag"])
    return body % values


def patch_templates(text: str) -> str:
    text = upsert_before(
        text,
        PACKAGE_BEGIN,
        PACKAGE_END,
        render_packages(text),
        "; ===== E2 AIR PACKAGE POOLS",
    )
    return upsert_before(
        text,
        TAG_BEGIN,
        TAG_END,
        render_tags(),
        '\t{Tags "ally_sup_tpl" "support_e2_tpl"',
    )


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
    return upsert_before(
        text,
        FRIEND_BEGIN if friendly else ENEMY_BEGIN,
        FRIEND_END if friendly else ENEMY_END,
        control_section(friendly=friendly),
        (
            '\t\t\t{"defense_support/motor_cleanup"'
            if friendly
            else '\t\t\t{"enemy_attack/motor_cleanup"'
        ),
    )


def read_text(path: Path) -> tuple[str, bool]:
    raw = path.read_bytes()
    return raw.decode("utf-8-sig"), raw.startswith(b"\xef\xbb\xbf")


def write_text(path: Path, text: str, bom: bool) -> None:
    raw = text.encode("utf-8")
    path.write_bytes((b"\xef\xbb\xbf" if bom else b"") + raw)


def validate_control(text: str, *, friendly: bool) -> None:
    begin = FRIEND_BEGIN if friendly else ENEMY_BEGIN
    end = FRIEND_END if friendly else ENEMY_END
    bounds = marked_bounds(text, begin, end)
    if not bounds:
        raise PatchError(f"Missing control section {begin}")
    block = text[bounds[0] : bounds[1]]
    package = FRIEND_PACKAGE if friendly else ENEMY_PACKAGE
    hull = FRIEND_HULL if friendly else ENEMY_HULL
    for token in (
        '{"delay" {time 45}}',
        f'{{tag {package}}}',
        f'{{tag {hull}}}',
        '{control AI}',
        '{ai_move {mode enable}}',
        '{action advance}',
        '{tag flag}',
    ):
        if token not in block:
            raise PatchError(f"Control path missing {token}")
    if block.count('{action advance}') != 1:
        raise PatchError("Control path must issue exactly one advance order")
    for forbidden in (
        '{"emit"',
        '{mode passengers}',
        'motor_leaving',
        'exit_motor_to_origin',
        'motor_cleanup',
        '{"delete"',
    ):
        if forbidden in block:
            raise PatchError(f"Control path must not contain {forbidden}")


def validate(root: Path) -> None:
    multi = root / "resource/map/multi"
    templates = (multi / "faction_support_templates.inc").read_text(encoding="utf-8-sig")
    defense = (multi / "defense_support_waves.inc").read_text(encoding="utf-8-sig")
    enemy = (multi / "enemy_attack_support.inc").read_text(encoding="utf-8-sig")
    variables = (multi / "dcg_vars.inc").read_text(encoding="utf-8-sig")

    for marker in (PACKAGE_BEGIN, PACKAGE_END, TAG_BEGIN, TAG_END):
        if templates.count(marker) != 1:
            raise PatchError(f"Template marker count invalid: {marker}")
    for token in ('{Entity "fmtv" 0xb500', '{Entity "ural" 0xb510'):
        if templates.count(token) != 1:
            raise PatchError(f"Control entity missing or duplicated: {token}")
    for mid in (*range(9900, 9911), *range(9920, 9931)):
        if templates.count(f'{{MID {mid}}}') != 1:
            raise PatchError(f"Control MID missing or duplicated: {mid}")
    for seat in range(1, 9):
        if f'{{0xb500 "seat{seat}"}}' not in templates:
            raise PatchError(f"Friendly seat{seat} link missing")
        if f'{{0xb510 "seat{seat}"}}' not in templates:
            raise PatchError(f"Enemy seat{seat} link missing")

    validate_control(defense, friendly=True)
    validate_control(enemy, friendly=False)

    for token in ('{"motor_compare_friend_done"}', '{"motor_compare_enemy_done"}'):
        if variables.count(token) != 1:
            raise PatchError(f"Variable missing or duplicated: {token}")

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
