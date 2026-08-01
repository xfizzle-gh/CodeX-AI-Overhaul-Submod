#!/usr/bin/env python3
"""Preserve linked motor crew/passenger seats during placement and drive startup.

The all-quadrants production overlay correctly claims and owns each complete
numbered package, but it then targets the full ``*_deploy`` set with placement
and pre-drive ``actor_state``. On the newer four-engine architecture that set
contains the hull, driver, commander, and every linked passenger. Teleporting
linked humans independently overwrites their seat transforms; enabling normal
infantry AI on cab crew then lets the driver/commander abandon the vehicle.

The engine files already document the correct contract: linked occupants are
carried by their hull. Therefore this hotfix:

* keeps full-package claiming, activation, and ownership transfer unchanged;
* places only the selected hull at the base-entry waypoint;
* applies pre-drive vehicle AI state only to the hull;
* leaves passenger AI promotion until the existing post-emit block;
* validates driver/commander links for all 16 numbered packages.
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
    finisher: str
    placer_macro: str
    deploy_tag: str
    hull_tag: str
    passenger_tag: str


ENGINES = (
    Engine(
        "resource/map/multi/attack_support_waves.inc",
        "as_finish_motor",
        "as_place_motor_package",
        "attack_support_deploy",
        "attack_support_motor_hull",
        "attack_support_motor_pax",
    ),
    Engine(
        "resource/map/multi/defense_support_waves.inc",
        "ds_finish_motor",
        "ds_place_motor_package",
        "def_sup_deploy",
        "def_sup_motor_hull",
        "def_sup_motor_pax",
    ),
    Engine(
        "resource/map/multi/enemy_attack_support.inc",
        "ea_finish_motor",
        "ea_place_motor_package",
        "ea_deploy",
        "ea_motor_hull",
        "ea_motor_pax",
    ),
    Engine(
        "resource/map/multi/enemy_defense_support.inc",
        "ed_finish_motor",
        "ed_place_motor_package",
        "enemy_def_deploy",
        "enemy_def_motor_hull",
        "enemy_def_motor_pax",
    ),
)

TEMPLATE_PATH = Path("resource/map/multi/faction_support_templates.inc")


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


def patch_placer(text: str, engine: Engine) -> str:
    marker = f'(define "{engine.placer_macro}"'
    start, end, block = paren_block(text, marker)
    deploy_token = f'{{tag {engine.deploy_tag}}}'
    hull_token = f'{{tag {engine.hull_tag}}}'

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

    deploy_token = f'{{tag {engine.deploy_tag}}}'
    hull_token = f'{{tag {engine.hull_tag}}}'
    if deploy_token not in actor and hull_token in actor:
        return text
    if actor.count(deploy_token) != 1 or hull_token in actor:
        raise PatchError(
            f"{engine.relative_path}: first actor_state must select exactly one "
            f"{engine.deploy_tag} before hotfix"
        )

    actor = actor.replace(deploy_token, hull_token, 1)
    finisher = finisher[:actor_start] + actor + finisher[actor_end:]
    return text[:start] + finisher + text[end:]


def validate_engine(text: str, engine: Engine) -> None:
    _, _, placer = paren_block(text, f'(define "{engine.placer_macro}"')
    deploy_token = f'{{tag {engine.deploy_tag}}}'
    hull_token = f'{{tag {engine.hull_tag}}}'
    if deploy_token in placer:
        raise PatchError(
            f"{engine.relative_path}: linked occupants are still targeted by placement"
        )
    if placer.count(hull_token) != 3:
        raise PatchError(
            f"{engine.relative_path}: expected three hull-only placement selectors"
        )

    _, _, finisher = paren_block(text, f'(define "{engine.finisher}"')
    actor_at = finisher.find('{"actor_state"')
    if actor_at < 0:
        raise PatchError(f"{engine.relative_path}: pre-drive actor_state missing")
    _, actor_end, first_actor = brace_block(
        finisher, '{"actor_state"', search_from=actor_at
    )
    if deploy_token in first_actor or hull_token not in first_actor:
        raise PatchError(
            f"{engine.relative_path}: pre-drive actor_state is not hull-only"
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
    if f'{{tag {engine.passenger_tag}}}' not in passenger_actor:
        raise PatchError(
            f"{engine.relative_path}: post-emit actor_state does not target passengers"
        )


def validate_cab_links(text: str) -> None:
    tags: dict[str, set[str]] = {}
    for line in text.splitlines():
        if "{Tags " not in line:
            continue
        entity_match = re.search(r'\s(0x[0-9a-fA-F]+)\}\s*$', line)
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
            hulls = [entity_id for entity_id, entity_tags in tags.items() if hull_tag in entity_tags]
            crew = [entity_id for entity_id, entity_tags in tags.items() if crew_tag in entity_tags]
            if len(hulls) != 1:
                raise PatchError(f"{hull_tag}: expected one hull, found {len(hulls)}")
            if len(crew) != 2:
                raise PatchError(f"{crew_tag}: expected two cab crew, found {len(crew)}")
            hull = hulls[0]
            linked_slots = {
                slot for body, target, slot in links if target == hull and body in crew
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
    for engine in ENGINES:
        path = root / engine.relative_path
        if not path.is_file():
            raise PatchError(f"Missing engine: {path}")
        original = path.read_text(encoding="utf-8-sig")
        patched = patch_placer(original, engine)
        patched = patch_pre_drive_actor_state(patched, engine)
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
    print(f"Linked-seat motor hotfix {action}: {len(changed)} engine file(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
