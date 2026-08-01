#!/usr/bin/env python3
"""Restore validated numbered entry pads for motor package placement.

The production overlay used the bare ``attack_support_entry_a/b`` aliases as
``target_waypoint`` placement destinations. The base deployer reserves those
aliases for movement/patrol orders; physical placement is validated only on the
numbered pads ``attack_support_entry_a1/b1``. Runtime consequently advanced the
motor lifecycle while the hull remained at its off-map template position.

This pass changes only each dedicated motor placer. Return-to-base movement in
the finisher intentionally continues using the bare aliases.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path


class PatchError(RuntimeError):
    pass


@dataclass(frozen=True)
class Engine:
    relative_path: str
    placer_macro: str
    finisher: str


ENGINES = (
    Engine(
        "resource/map/multi/attack_support_waves.inc",
        "as_place_motor_package",
        "as_finish_motor",
    ),
    Engine(
        "resource/map/multi/defense_support_waves.inc",
        "ds_place_motor_package",
        "ds_finish_motor",
    ),
    Engine(
        "resource/map/multi/enemy_attack_support.inc",
        "ea_place_motor_package",
        "ea_finish_motor",
    ),
    Engine(
        "resource/map/multi/enemy_defense_support.inc",
        "ed_place_motor_package",
        "ed_finish_motor",
    ),
)

BARE_A = 'target_waypoint "attack_support_entry_a"'
BARE_B = 'target_waypoint "attack_support_entry_b"'
NUMBERED_A = 'target_waypoint "attack_support_entry_a1"'
NUMBERED_B = 'target_waypoint "attack_support_entry_b1"'


def balanced(text: str, marker: str, opener: str, closer: str) -> tuple[int, int, str]:
    marker_at = text.find(marker)
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


def patch_placer(text: str, engine: Engine) -> str:
    marker = f'(define "{engine.placer_macro}"'
    start, end, block = paren_block(text, marker)

    bare_count = block.count(BARE_A) + block.count(BARE_B)
    numbered_count = block.count(NUMBERED_A) + block.count(NUMBERED_B)

    if bare_count == 0 and numbered_count == 3:
        return text
    if bare_count != 3 or numbered_count != 0:
        raise PatchError(
            f"{engine.relative_path}: expected three bare motor placement aliases, "
            f"found bare={bare_count}, numbered={numbered_count}"
        )

    block = block.replace(BARE_A, NUMBERED_A).replace(BARE_B, NUMBERED_B)
    return text[:start] + block + text[end:]


def validate_engine(text: str, engine: Engine) -> None:
    _, _, placer = paren_block(text, f'(define "{engine.placer_macro}"')
    if BARE_A in placer or BARE_B in placer:
        raise PatchError(f"{engine.relative_path}: motor placer still uses bare aliases")
    if placer.count(NUMBERED_A) + placer.count(NUMBERED_B) != 3:
        raise PatchError(
            f"{engine.relative_path}: motor placer does not contain three numbered targets"
        )
    if NUMBERED_A not in placer or NUMBERED_B not in placer:
        raise PatchError(
            f"{engine.relative_path}: motor placer must support both numbered map edges"
        )

    # The finisher's withdrawal is an action/movement route, where the bare aliases
    # are valid and intentional. This pass must not rewrite that lifecycle.
    _, _, finisher = paren_block(text, f'(define "{engine.finisher}"')
    if NUMBERED_A in finisher or NUMBERED_B in finisher:
        raise PatchError(
            f"{engine.relative_path}: numbered placement pads leaked into withdrawal"
        )
    if 'waypoint "attack_support_entry_a"' not in finisher and 'waypoint "attack_support_entry_b"' not in finisher:
        raise PatchError(
            f"{engine.relative_path}: finisher lost its bare return-to-base route"
        )


def apply(root: Path, *, check_only: bool = False) -> list[str]:
    changed: list[str] = []
    for engine in ENGINES:
        path = root / engine.relative_path
        if not path.is_file():
            raise PatchError(f"Missing engine: {path}")
        original = path.read_text(encoding="utf-8-sig")
        patched = patch_placer(original, engine)
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
    print(f"Motor numbered-entry hotfix {action}: {len(changed)} engine file(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
