#!/usr/bin/env python3
"""Isolate motor dispatch and staging from the infantry reinforcement lane.

Runtime showed two independent collisions in the four-engine production build:

* each motor clock reused ``*_wave_cmd$``, so an opening infantry wave could
  consume command 19 and defer the nominal +30-second truck to the next rearm;
* each motor trigger temporarily added the generic infantry deploy tag to the
  linked hull/crew/passenger package, allowing a generic infantry finisher to
  issue advance orders to occupants before the motor finisher isolated them.

The fix gives every engine a dedicated ``*_motor_dispatch$`` command bit and
stages the complete linked package under its existing motor-transfer tag from
the instant it is claimed. Generic infantry finishers can no longer select the
truck package, while hull-only placement and pre-drive AI preserve all seats.
"""

from __future__ import annotations

import argparse
import re
from dataclasses import dataclass
from pathlib import Path

FACTIONS = ("rusa", "ukr", "nato", "prc")
PACKAGES = range(1, 5)


class PatchError(RuntimeError):
    pass


@dataclass(frozen=True)
class Engine:
    relative_path: str
    namespace: str
    finisher: str
    placer_macro: str
    trigger_pattern: str
    wave_var: str
    dispatch_var: str
    deploy_tag: str
    transfer_tag: str
    hull_tag: str
    passenger_tag: str


ENGINES = (
    Engine(
        "resource/map/multi/attack_support_waves.inc",
        "attack_support",
        "as_finish_motor",
        "as_place_motor_package",
        "attack_support/ally_{faction}_motor",
        "attack_support_wave_cmd$",
        "attack_support_motor_dispatch$",
        "attack_support_deploy",
        "attack_support_motor_transfer",
        "attack_support_motor_hull",
        "attack_support_motor_pax",
    ),
    Engine(
        "resource/map/multi/defense_support_waves.inc",
        "defense_support",
        "ds_finish_motor",
        "ds_place_motor_package",
        "defense_support/ally_{faction}_motor",
        "defense_support_wave_cmd$",
        "defense_support_motor_dispatch$",
        "def_sup_deploy",
        "def_sup_motor_transfer",
        "def_sup_motor_hull",
        "def_sup_motor_pax",
    ),
    Engine(
        "resource/map/multi/enemy_attack_support.inc",
        "enemy_attack",
        "ea_finish_motor",
        "ea_place_motor_package",
        "enemy_attack/{faction}_motor",
        "enemy_attack_wave_cmd$",
        "enemy_attack_motor_dispatch$",
        "ea_deploy",
        "ea_motor_transfer",
        "ea_motor_hull",
        "ea_motor_pax",
    ),
    Engine(
        "resource/map/multi/enemy_defense_support.inc",
        "enemy_defense",
        "ed_finish_motor",
        "ed_place_motor_package",
        "enemy_defense/{faction}_motor",
        "enemy_defense_wave_cmd$",
        "enemy_defense_motor_dispatch$",
        "enemy_def_deploy",
        "enemy_def_motor_transfer",
        "enemy_def_motor_hull",
        "enemy_def_motor_pax",
    ),
)

TEMPLATE_PATH = Path("resource/map/multi/faction_support_templates.inc")
VARS_PATH = Path("resource/map/multi/dcg_vars.inc")


def balanced(
    text: str,
    marker: str,
    opener: str,
    closer: str,
    *,
    search_from: int = 0,
) -> tuple[int, int, str]:
    marker_at = text.find(marker, search_from)
    if marker_at < 0:
        raise PatchError(f"Missing marker: {marker}")
    begin = text.find(opener, marker_at)
    if begin < 0:
        raise PatchError(f"Missing opener after: {marker}")

    depth = 0
    quoted = False
    escaped = False
    for index in range(begin, len(text)):
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
                return begin, index + 1, text[begin : index + 1]
    raise PatchError(f"Unbalanced block: {marker}")


def paren_block(text: str, marker: str) -> tuple[int, int, str]:
    return balanced(text, marker, "(", ")")


def brace_block(
    text: str, marker: str, *, search_from: int = 0
) -> tuple[int, int, str]:
    return balanced(text, marker, "{", "}", search_from=search_from)


def patch_vars(text: str) -> str:
    missing = [
        engine.dispatch_var.removesuffix("$")
        for engine in ENGINES
        if f'{{"{engine.dispatch_var.removesuffix("$")}"}}' not in text
    ]
    if not missing:
        return text

    anchor = '\t\t\t{"enemy_attack_motor_first"}'
    position = text.find(anchor)
    if position < 0:
        raise PatchError("dcg_vars.inc: motor variable insertion anchor missing")

    rendered = "".join(f'\t\t\t{{"{name}"}}\n' for name in missing)
    return text[:position] + rendered + text[position:]


def patch_placer(text: str, engine: Engine) -> str:
    marker = f'(define "{engine.placer_macro}"'
    start, end, block = paren_block(text, marker)
    deploy_token = f"{{tag {engine.deploy_tag}}}"
    hull_token = f"{{tag {engine.hull_tag}}}"

    deploy_count = block.count(deploy_token)
    hull_count = block.count(hull_token)
    if deploy_count == 0 and hull_count == 3:
        return text
    if deploy_count != 3 or hull_count != 0:
        raise PatchError(
            f"{engine.relative_path}: expected three deploy selectors in "
            f"{engine.placer_macro}, found deploy={deploy_count}, hull={hull_count}"
        )

    block = block.replace(deploy_token, hull_token)
    return text[:start] + block + text[end:]


def patch_pre_drive_actor_state(text: str, engine: Engine) -> str:
    marker = f'(define "{engine.finisher}"'
    start, end, finisher = paren_block(text, marker)

    actor_at = finisher.find('{"actor_state"')
    if actor_at < 0:
        raise PatchError(f"{engine.relative_path}: pre-drive actor_state missing")
    actor_start, actor_end, actor = brace_block(
        finisher, '{"actor_state"', search_from=actor_at
    )

    deploy_token = f"{{tag {engine.deploy_tag}}}"
    hull_token = f"{{tag {engine.hull_tag}}}"
    if deploy_token not in actor and hull_token in actor:
        return text
    if actor.count(deploy_token) != 1 or hull_token in actor:
        raise PatchError(
            f"{engine.relative_path}: first actor_state must select exactly one "
            f"{engine.deploy_tag} before isolation"
        )

    actor = actor.replace(deploy_token, hull_token, 1)
    finisher = finisher[:actor_start] + actor + finisher[actor_end:]
    return text[:start] + finisher + text[end:]


def patch_motor_clock(text: str, engine: Engine) -> str:
    marker = f'{{"{engine.namespace}/motor_clock"'
    start, end, block = brace_block(text, marker)

    shared_token = f'{{var "{engine.wave_var}"}}'
    dispatch_token = f'{{var "{engine.dispatch_var}"}}'
    if shared_token in block:
        block = block.replace(shared_token, dispatch_token)

    set_19 = re.compile(
        r'(\{"set_i"\s+\{var\s+"'
        + re.escape(engine.dispatch_var)
        + r'"\}\s+\{op\s+"="\}\s+\{value\s+)19(\}\})'
    )
    block, replacements = set_19.subn(r"\g<1>1\g<2>", block)
    if replacements > 1:
        raise PatchError(
            f"{engine.relative_path}: motor clock had {replacements} command-19 writes"
        )

    if shared_token in block:
        raise PatchError(f"{engine.relative_path}: motor clock still shares wave_cmd")
    if dispatch_token not in block:
        raise PatchError(f"{engine.relative_path}: motor clock lacks dedicated dispatch")
    if not re.search(
        r'\{"set_i"\s+\{var\s+"'
        + re.escape(engine.dispatch_var)
        + r'"\}\s+\{op\s+"="\}\s+\{value\s+1\}\}',
        block,
    ):
        raise PatchError(
            f"{engine.relative_path}: motor clock does not arm dedicated dispatch"
        )

    return text[:start] + block + text[end:]


def patch_motor_trigger(text: str, engine: Engine, faction: str) -> str:
    marker = f'{{"{engine.trigger_pattern.format(faction=faction)}"'
    start, end, block = brace_block(text, marker)

    shared_token = f'{{var "{engine.wave_var}"}}'
    dispatch_token = f'{{var "{engine.dispatch_var}"}}'
    if shared_token in block:
        block = block.replace(shared_token, dispatch_token)

    # Command 19 becomes a dedicated one-bit dispatch gate. The reset stays zero.
    cmp_19 = re.compile(
        r'(\{"[0-9]+\.cmp_i"\s+\{var\s+"'
        + re.escape(engine.dispatch_var)
        + r'"\}\s+\{op\s+"=="\}\s+\{value\s+)19(\}\})'
    )
    block, replacements = cmp_19.subn(r"\g<1>1\g<2>", block)
    if replacements > 1:
        raise PatchError(
            f"{engine.relative_path}: {marker} had {replacements} command-19 gates"
        )

    # Never expose a motor package to the generic infantry deploy selector, even
    # for the fraction of a second between claim, placement and motor finishing.
    block = block.replace(engine.deploy_tag, engine.transfer_tag)

    if shared_token in block:
        raise PatchError(f"{engine.relative_path}: {marker} still shares wave_cmd")
    if dispatch_token not in block:
        raise PatchError(f"{engine.relative_path}: {marker} lacks motor dispatch gate")
    if engine.deploy_tag in block:
        raise PatchError(f"{engine.relative_path}: {marker} still uses infantry staging")
    if engine.transfer_tag not in block:
        raise PatchError(f"{engine.relative_path}: {marker} lacks motor-only staging")

    return text[:start] + block + text[end:]


def patch_engine(text: str, engine: Engine) -> str:
    text = patch_placer(text, engine)
    text = patch_pre_drive_actor_state(text, engine)
    text = patch_motor_clock(text, engine)
    for faction in FACTIONS:
        text = patch_motor_trigger(text, engine, faction)
    return text


def validate_engine(text: str, engine: Engine) -> None:
    _, _, placer = paren_block(text, f'(define "{engine.placer_macro}"')
    deploy_token = f"{{tag {engine.deploy_tag}}}"
    hull_token = f"{{tag {engine.hull_tag}}}"
    if deploy_token in placer:
        raise PatchError(
            f"{engine.relative_path}: linked occupants are still targeted by placement"
        )
    if placer.count(hull_token) != 3:
        raise PatchError(
            f"{engine.relative_path}: expected three hull-only placement selectors"
        )

    _, _, clock = brace_block(text, f'{{"{engine.namespace}/motor_clock"')
    if f'{{var "{engine.wave_var}"}}' in clock:
        raise PatchError(f"{engine.relative_path}: motor clock still uses shared wave_cmd")
    if f'{{var "{engine.dispatch_var}"}}' not in clock:
        raise PatchError(f"{engine.relative_path}: motor clock lacks dedicated dispatch")

    for faction in FACTIONS:
        marker = f'{{"{engine.trigger_pattern.format(faction=faction)}"'
        _, _, trigger = brace_block(text, marker)
        if f'{{var "{engine.wave_var}"}}' in trigger:
            raise PatchError(f"{engine.relative_path}: {marker} still uses shared wave_cmd")
        if f'{{var "{engine.dispatch_var}"}}' not in trigger:
            raise PatchError(f"{engine.relative_path}: {marker} lacks dispatch gate")
        if engine.deploy_tag in trigger:
            raise PatchError(f"{engine.relative_path}: {marker} exposes infantry staging")
        if engine.transfer_tag not in trigger:
            raise PatchError(f"{engine.relative_path}: {marker} lacks motor staging")

    _, _, finisher = paren_block(text, f'(define "{engine.finisher}"')
    actor_at = finisher.find('{"actor_state"')
    if actor_at < 0:
        raise PatchError(f"{engine.relative_path}: pre-drive actor_state missing")
    _, _, first_actor = brace_block(
        finisher, '{"actor_state"', search_from=actor_at
    )
    if deploy_token in first_actor or hull_token not in first_actor:
        raise PatchError(
            f"{engine.relative_path}: pre-drive actor_state is not hull-only"
        )
    if engine.transfer_tag not in finisher:
        raise PatchError(
            f"{engine.relative_path}: motor finisher lacks isolated ownership staging"
        )

    emit_at = finisher.find('{"emit"')
    if emit_at < 0:
        raise PatchError(f"{engine.relative_path}: passenger emit missing")
    post_emit = finisher[emit_at:]
    next_actor = post_emit.find('{"actor_state"')
    if next_actor < 0:
        raise PatchError(
            f"{engine.relative_path}: post-emit passenger actor_state missing"
        )
    _, _, passenger_actor = brace_block(
        post_emit, '{"actor_state"', search_from=next_actor
    )
    if f"{{tag {engine.passenger_tag}}}" not in passenger_actor:
        raise PatchError(
            f"{engine.relative_path}: post-emit actor_state does not target passengers"
        )


def validate_vars(text: str) -> None:
    for engine in ENGINES:
        declaration = f'{{"{engine.dispatch_var.removesuffix("$")}"}}'
        if text.count(declaration) != 1:
            raise PatchError(
                f"dcg_vars.inc: expected one declaration for {engine.dispatch_var}"
            )


def validate_cab_links(text: str) -> None:
    tags: dict[str, set[str]] = {}
    for line in text.splitlines():
        if "{Tags " not in line:
            continue
        entity_match = re.search(r"\s(0x[0-9a-fA-F]+)\}\s*$", line)
        if not entity_match:
            continue
        entity_id = entity_match.group(1).lower()
        tags[entity_id] = set(re.findall(r'"([^"]+)"', line))

    links = {
        (body.lower(), hull.lower(), slot)
        for body, hull, slot in re.findall(
            r'\{Link\s+(0x[0-9a-fA-F]+)\s+'
            r'\{(0x[0-9a-fA-F]+)\s+"([^"]+)"\}\}',
            text,
        )
    }

    for faction in FACTIONS:
        for package in PACKAGES:
            hull_tag = f"ally_sup_{faction}_p{package}_hull"
            crew_tag = f"ally_sup_{faction}_p{package}_crew"
            hulls = [
                entity_id
                for entity_id, entity_tags in tags.items()
                if hull_tag in entity_tags
            ]
            crew = [
                entity_id
                for entity_id, entity_tags in tags.items()
                if crew_tag in entity_tags
            ]
            if len(hulls) != 1:
                raise PatchError(f"{hull_tag}: expected one hull, found {len(hulls)}")
            if len(crew) != 2:
                raise PatchError(f"{crew_tag}: expected two cab crew, found {len(crew)}")
            hull = hulls[0]
            linked_slots = {
                slot
                for body, target, slot in links
                if target == hull and body in crew
            }
            if linked_slots != {"driver", "commander"}:
                raise PatchError(
                    f"{faction} p{package}: cab links are {sorted(linked_slots)}, "
                    "expected driver+commander"
                )
            for body in crew:
                if "sup_linked" not in tags[body]:
                    raise PatchError(
                        f"{faction} p{package}: cab crew {body} lacks sup_linked"
                    )


def apply(root: Path, *, check_only: bool = False) -> list[str]:
    template_path = root / TEMPLATE_PATH
    if not template_path.is_file():
        raise PatchError(f"Missing template pool: {template_path}")
    validate_cab_links(template_path.read_text(encoding="utf-8-sig"))

    changed: list[str] = []

    vars_path = root / VARS_PATH
    if not vars_path.is_file():
        raise PatchError(f"Missing variable declarations: {vars_path}")
    original_vars = vars_path.read_text(encoding="utf-8-sig")
    patched_vars = patch_vars(original_vars)
    validate_vars(patched_vars)
    if patched_vars != original_vars:
        changed.append(VARS_PATH.as_posix())
        if not check_only:
            vars_path.write_text(patched_vars, encoding="utf-8")

    for engine in ENGINES:
        path = root / engine.relative_path
        if not path.is_file():
            raise PatchError(f"Missing engine: {path}")
        original = path.read_text(encoding="utf-8-sig")
        patched = patch_engine(original, engine)
        validate_engine(patched, engine)
        if patched != original:
            changed.append(engine.relative_path)
            if not check_only:
                path.write_text(patched, encoding="utf-8")
    return changed


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    changed = apply(args.root, check_only=args.check)
    action = "would patch" if args.check else "patched"
    print(f"Motor dispatch isolation {action}: {len(changed)} file(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
