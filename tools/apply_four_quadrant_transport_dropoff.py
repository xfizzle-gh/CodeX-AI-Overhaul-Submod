#!/usr/bin/env python3
"""Replace rotating transport patrols with a single perimeter drop-off.

The linked package and four-quadrant/four-faction coverage come from the existing
normal-transport generator. This overlay changes only the runtime lifecycle:

* passengers are prevented from independently moving while linked;
* the truck drives to one generated flag-perimeter waypoint;
* arrival near an active flag stops the truck and emits passengers;
* passenger AI is enabled and ordered toward an active flag;
* the truck receives no further route, withdrawal, deletion, or cleanup order.

The previous rotating-patrol implementation is archived on:
archive/four-quadrant-patrol-before-dropoff-2026-08-01
"""
from __future__ import annotations

import argparse
import importlib.util
import sys
from pathlib import Path

BASE_PATH = Path(__file__).with_name("apply_four_quadrant_transport_patrol_fixed.py")


def _load_base():
    spec = importlib.util.spec_from_file_location("_transport_patrol_base", BASE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Could not load {BASE_PATH}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


base = _load_base()

HOLD_MARKER = "; PASSENGERS HELD UNTIL PERIMETER DROPOFF"
DROPOFF_MARKER = "; PERIMETER ARRIVAL: STOP, DISEMBARK, INFANTRY ADVANCE"
ARRIVAL_DISTANCE = 500
DROPPED_STEP = 99


def _render(template: str, **values: object) -> str:
    for key, value in values.items():
        template = template.replace(f"@{key}@", str(value))
    return template


def render_issue_dropoff(engine) -> str:
    return _render(
        '''\t\t\t(define "transport_@KEY@_issue_dropoff"
\t\t\t\t{"action"
\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag @HULL@}}
\t\t\t\t\t{drop orders}
\t\t\t\t\t{action move}
\t\t\t\t\t{waypoint "transport_patrol_flag_@STEP@"}
\t\t\t\t}
\t\t\t)''',
        KEY=engine.key,
        HULL=engine.hull_tag,
        STEP=engine.start_step,
    )


def render_dispatch(engine, faction: str) -> str:
    cfg = base.FACTIONS[faction]
    package, source_hull, source_pax, template_tag = base.source_tags(faction, engine)
    return _render(
        '''\t\t\t{"@NS@/normal_transport_@FACTION@"
\t\t\t\t{condition
\t\t\t\t\t{expression "1 & 2 & 3 & 4 & 5 & 6"}
\t\t\t\t\t{terms
\t\t\t\t\t\t{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value @USER_DEF@}}
\t\t\t\t\t\t{"2.cmp_i" {var "@ARMED@$"} {op "=="} {value 1}}
\t\t\t\t\t\t{"3.cmp_i" {var "@ARMY_VAR@$"} {op "=="} {value @ARMY@}}
\t\t\t\t\t\t{"4.cmp_i" {var "@OWNER_ID@$"} {op ">"} {value 0}}
\t\t\t\t\t\t{"5.cmp_i" {var "@DONE@$"} {op "=="} {value 0}}
\t\t\t\t\t\t{"6.entities" {selector {tag @SOURCE_HULL@}} {count {op ">="} {value 1}}}
\t\t\t\t\t}
\t\t\t\t}
\t\t\t\t{actions
\t\t\t\t\t{"set_i" {var "@DONE@$"} {op "="} {value 1}}
\t\t\t\t\t{"delay" {time 45}}
\t\t\t\t\t{"entity_state"
\t\t\t\t\t\t{selector {source advanced} {group {select {tag {tag @PACKAGE@}}}}}
\t\t\t\t\t\t{tag_add @DEPLOY@}
\t\t\t\t\t}
\t\t\t\t\t{"entity_state"
\t\t\t\t\t\t{selector {source advanced} {group {select {tag {tag @SOURCE_HULL@}}}}}
\t\t\t\t\t\t{tag_add @HULL@}
\t\t\t\t\t}
\t\t\t\t\t{"entity_state"
\t\t\t\t\t\t{selector {source advanced} {group {select {tag {tag @SOURCE_PAX@}}}}}
\t\t\t\t\t\t{tag_add @PAX@}
\t\t\t\t\t}
\t\t\t\t\t("transport_@KEY@_place")
\t\t\t\t\t{"entity_state"
\t\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag @DEPLOY@}}
\t\t\t\t\t\t{tag_remove @TEMPLATE_TAG@}
\t\t\t\t\t\t{tag_remove @PACKAGE@}
\t\t\t\t\t\t{tag_remove hidden}
\t\t\t\t\t\t{inactive off}
\t\t\t\t\t\t{impregnability disabled}
\t\t\t\t\t\t{discovered on}
\t\t\t\t\t}
\t\t\t\t\t{"delay" {time 0.2}}
\t\t\t\t\t("@OWNER_MACRO@")
\t\t\t\t\t{"delay" {time 0.4}}
\t\t\t\t\t; PASSENGERS HELD UNTIL PERIMETER DROPOFF
\t\t\t\t\t{"actor_state"
\t\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag @PAX@}}
\t\t\t\t\t\t{ai_move {mode disable}}
\t\t\t\t\t\t{movement {speed stop} {kind normal} {type normal}}
\t\t\t\t\t}
\t\t\t\t\t{"actor_state"
\t\t\t\t\t\t{selector
\t\t\t\t\t\t\t{ignore_captured_by_user 0}
\t\t\t\t\t\t\t{group
\t\t\t\t\t\t\t\t{select {tag {tag @DEPLOY@}}}
\t\t\t\t\t\t\t\t{exclude {tag {tag @PAX@}}}
\t\t\t\t\t\t\t}
\t\t\t\t\t\t}
\t\t\t\t\t\t{control AI}
\t\t\t\t\t\t{ai_move {mode enable}}
\t\t\t\t\t\t{weapon_prepare on}
\t\t\t\t\t\t{fire_mode open}
\t\t\t\t\t\t{move_mode free}
\t\t\t\t\t\t{movement {speed normal} {kind normal} {type normal}}
\t\t\t\t\t\t{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}
\t\t\t\t\t}
\t\t\t\t\t{"set_i" {var "@STEP_VAR@$"} {op "="} {value @START_STEP@}}
\t\t\t\t\t("transport_@KEY@_issue_dropoff")
\t\t\t\t\t{"entity_state"
\t\t\t\t\t\t{selector {tag @DEPLOY@}}
\t\t\t\t\t\t{tag_remove @DEPLOY@}
\t\t\t\t\t}
\t\t\t\t}
\t\t\t}''',
        NS=engine.trigger_ns,
        FACTION=faction,
        USER_DEF=engine.user_is_defender,
        ARMED=engine.armed_var,
        ARMY_VAR=engine.army_var,
        ARMY=cfg["army"],
        OWNER_ID=engine.owner_id_var,
        DONE=engine.done_var,
        SOURCE_HULL=source_hull,
        PACKAGE=package,
        DEPLOY=engine.deploy_tag,
        HULL=engine.hull_tag,
        SOURCE_PAX=source_pax,
        PAX=engine.pax_tag,
        KEY=engine.key,
        TEMPLATE_TAG=template_tag,
        OWNER_MACRO=engine.owner_macro,
        STEP_VAR=engine.step_var,
        START_STEP=engine.start_step,
    )


def render_dropoff_trigger(engine) -> str:
    return _render(
        '''\t\t\t; PERIMETER ARRIVAL: STOP, DISEMBARK, INFANTRY ADVANCE
\t\t\t{"@NS@/normal_transport_dropoff"
\t\t\t\t{condition
\t\t\t\t\t{expression "1 & 2 & 3 & 4"}
\t\t\t\t\t{terms
\t\t\t\t\t\t{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value @USER_DEF@}}
\t\t\t\t\t\t{"2.cmp_i" {var "@DONE@$"} {op "=="} {value 1}}
\t\t\t\t\t\t{"3.cmp_i" {var "@STEP_VAR@$"} {op "=="} {value @START_STEP@}}
\t\t\t\t\t\t{"4.near"
\t\t\t\t\t\t\t{units {ignore_captured_by_user 0} {tag @HULL@} {state operatable}}
\t\t\t\t\t\t\t{near_to
\t\t\t\t\t\t\t\t{ignore_captured_by_user 0}
\t\t\t\t\t\t\t\t{group
\t\t\t\t\t\t\t\t\t{select {tag {tag flag}}}
\t\t\t\t\t\t\t\t\t{exclude {state {state inactive}}}
\t\t\t\t\t\t\t\t}
\t\t\t\t\t\t\t}
\t\t\t\t\t\t\t{distance @ARRIVAL_DISTANCE@}
\t\t\t\t\t\t}
\t\t\t\t\t}
\t\t\t\t}
\t\t\t\t{actions
\t\t\t\t\t{"set_i" {var "@STEP_VAR@$"} {op "="} {value @DROPPED_STEP@}}
\t\t\t\t\t{"action"
\t\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag @HULL@}}
\t\t\t\t\t\t{drop orders}
\t\t\t\t\t\t{action move}
\t\t\t\t\t\t{waypoint "transport_patrol_flag_@START_STEP@"}
\t\t\t\t\t}
\t\t\t\t\t{"actor_state"
\t\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag @HULL@}}
\t\t\t\t\t\t{movement {speed stop} {kind normal} {type normal}}
\t\t\t\t\t}
\t\t\t\t\t{"delay" {time 0.5}}
\t\t\t\t\t{"emit"
\t\t\t\t\t\t{selector
\t\t\t\t\t\t\t{ignore_captured_by_user 0}
\t\t\t\t\t\t\t{tag @HULL@}
\t\t\t\t\t\t\t{type vehicle}
\t\t\t\t\t\t\t{state inhabited}
\t\t\t\t\t\t}
\t\t\t\t\t\t{drop orders}
\t\t\t\t\t\t{emit {mode passengers}}
\t\t\t\t\t}
\t\t\t\t\t{"delay" {time 0.5}}
\t\t\t\t\t{"actor_state"
\t\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag @PAX@}}
\t\t\t\t\t\t{control AI}
\t\t\t\t\t\t{ai_move {mode enable}}
\t\t\t\t\t\t{weapon_prepare on}
\t\t\t\t\t\t{fire_mode open}
\t\t\t\t\t\t{move_mode free}
\t\t\t\t\t\t{movement {speed normal} {kind normal} {type normal}}
\t\t\t\t\t\t{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}
\t\t\t\t\t}
\t\t\t\t\t{"action"
\t\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag @PAX@}}
\t\t\t\t\t\t{drop orders}
\t\t\t\t\t\t{action advance}
\t\t\t\t\t\t{target
\t\t\t\t\t\t\t{ignore_captured_by_user 0}
\t\t\t\t\t\t\t{group
\t\t\t\t\t\t\t\t{select {tag {tag flag}}}
\t\t\t\t\t\t\t\t{exclude {state {state inactive}}}
\t\t\t\t\t\t\t}
\t\t\t\t\t\t}
\t\t\t\t\t}
\t\t\t\t}
\t\t\t}''',
        NS=engine.trigger_ns,
        USER_DEF=engine.user_is_defender,
        DONE=engine.done_var,
        STEP_VAR=engine.step_var,
        START_STEP=engine.start_step,
        HULL=engine.hull_tag,
        PAX=engine.pax_tag,
        ARRIVAL_DISTANCE=ARRIVAL_DISTANCE,
        DROPPED_STEP=DROPPED_STEP,
    )


def render_engine(engine) -> str:
    begin = base.ENGINE_BEGIN.format(key=engine.key.upper())
    end = base.ENGINE_END.format(key=engine.key.upper())
    parts = [
        begin,
        "\t\t\t; Single perimeter drop-off: passengers held while linked, truck stops",
        "\t\t\t; near an active flag, passengers advance, truck remains parked.",
        base.render_place(engine),
        render_issue_dropoff(engine),
    ]
    parts.extend(render_dispatch(engine, faction) for faction in base.FACTIONS)
    parts.extend((render_dropoff_trigger(engine), end))
    return "\n\n".join(parts)


def validate_engine(text: str, engine) -> None:
    begin = base.ENGINE_BEGIN.format(key=engine.key.upper())
    end = base.ENGINE_END.format(key=engine.key.upper())
    bounds = base.marked_bounds(text, begin, end)
    if not bounds:
        raise base.PatchError(f"{engine.key}: drop-off section missing")
    block = text[bounds[0] : bounds[1]]

    for faction in base.FACTIONS:
        trigger = f'{{"{engine.trigger_ns}/normal_transport_{faction}"'
        if block.count(trigger) != 1:
            raise base.PatchError(f"{engine.key}: {faction} dispatch missing or duplicated")

    required = (
        HOLD_MARKER,
        DROPOFF_MARKER,
        f'{{"{engine.trigger_ns}/normal_transport_dropoff"',
        f'("transport_{engine.key}_issue_dropoff")',
        f'{{waypoint "transport_patrol_flag_{engine.start_step}"}}',
        f'{{distance {ARRIVAL_DISTANCE}}}',
        f'{{value {DROPPED_STEP}}}',
        '{ai_move {mode disable}}',
        '{ai_move {mode enable}}',
        '{emit {mode passengers}}',
        '{movement {speed stop}',
        '{action advance}',
    )
    for token in required:
        if token not in block:
            raise base.PatchError(f"{engine.key}: drop-off section missing {token}")

    for forbidden in (
        "/normal_transport_patrol",
        "exit_motor_to_origin",
        "motor_leaving",
        '{"delete"',
        '{"delay" {time 75}}',
    ):
        if forbidden in block:
            raise base.PatchError(f"{engine.key}: drop-off section contains {forbidden}")


def validate(root: Path) -> None:
    multi = root / "resource/map/multi"
    templates = (multi / "faction_support_templates.inc").read_text(encoding="utf-8-sig")
    variables = (multi / "dcg_vars.inc").read_text(encoding="utf-8-sig")

    for marker in (base.PACKAGE_BEGIN, base.PACKAGE_END, base.TAG_BEGIN, base.TAG_END):
        if templates.count(marker) != 1:
            raise base.PatchError(f"Template marker count invalid: {marker}")

    for faction, cfg in base.FACTIONS.items():
        start = int(cfg["enemy_start"])
        for seat in range(1, 9):
            if f'{{0x{start:x} "seat{seat}"}}' not in templates:
                raise base.PatchError(f"Enemy {faction} seat{seat} link is missing")

    for engine in base.ENGINES:
        text = (multi / engine.filename).read_text(encoding="utf-8-sig")
        validate_engine(text, engine)
        for var in (engine.done_var, engine.step_var):
            if variables.count(f'{{"{var}"}}') != 1:
                raise base.PatchError(f"Transport var missing or duplicated: {var}")

    attack = (multi / "attack_support_waves.inc").read_text(encoding="utf-8-sig")
    defense = (multi / "defense_support_waves.inc").read_text(encoding="utf-8-sig")
    enemy_attack = (multi / "enemy_attack_support.inc").read_text(encoding="utf-8-sig")
    for text, trigger in (
        (attack, '{"attack_support/motor_test"'),
        (defense, '{"defense_support/motor_test"'),
        (enemy_attack, '{"enemy_attack/motor_test"'),
    ):
        if trigger in text:
            raise base.PatchError(f"Old timed motor trigger is still active: {trigger}")


base.render_engine = render_engine
base.validate_engine = validate_engine
base.validate = validate

# Export the corrected implementation after monkey-patching its runtime globals.
apply = base.apply
ENGINES = base.ENGINES
FACTIONS = base.FACTIONS
FILES = base.FILES
PatchError = base.PatchError
marked_bounds = base.marked_bounds


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if args.check:
        validate(args.root)
        print("Four-quadrant perimeter drop-off transports validated.")
    else:
        changed = apply(args.root)
        print(f"Four-quadrant perimeter drop-off patched {len(changed)} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
