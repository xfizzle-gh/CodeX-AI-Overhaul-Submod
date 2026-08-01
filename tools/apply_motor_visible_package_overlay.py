#!/usr/bin/env python3
"""Place test motor packages as one linked unit at the base entry waypoint.

The live +30 second test proved that the motor package was claimed and the FMTV
actor existed, but it never entered the visible battlefield. The previous
"visible" overlay still targeted generated rear_a1/rear_b1 pads. Those pads are
outside the usable map boundary on some CWA maps, so a valid truck can exist and
remain permanently off-map.

This deployment-only overlay gives the two proven attacker-side motor engines
a dedicated placement helper that moves the complete linked package in one
operation to the original attack_support_entry_a/entry_b waypoint on the
correct side. Those waypoints are the map's real spawn-centroid entries, not
the outward rear-pad projections. The normal motor drive, emit, infantry order,
withdrawal, and cleanup code remains unchanged.
"""

from __future__ import annotations

import argparse
from pathlib import Path


class PatchError(RuntimeError):
    pass


def named_block(text: str, marker: str) -> tuple[int, int, str]:
    start = text.find(marker)
    if start < 0:
        raise PatchError(f"Missing block marker: {marker}")
    brace = text.find("{", start)
    if brace < 0:
        raise PatchError(f"Missing opening brace after: {marker}")

    depth = 0
    for index in range(brace, len(text)):
        char = text[index]
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return brace, index + 1, text[brace : index + 1]
    raise PatchError(f"Unbalanced block: {marker}")


ATTACK_MACRO = r'''

			; TEST OVERLAY: place the complete linked truck package in one operation.
			; Use the original spawn-centroid entry waypoint. The generated rear_a1/b1
			; pads can lie beyond the usable map edge and strand a valid truck off-map.
			(define "as_place_motor_visible"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"placement"
							{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
							{target_waypoint "attack_support_entry_b"}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"placement"
							{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
							{target_waypoint "attack_support_entry_a"}
						}
					}
					{"default"
						{"placement"
							{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
							{target_waypoint "attack_support_entry_b"}
						}
					}
				}
			)
'''

ENEMY_MACRO = r'''

			; TEST OVERLAY: place the complete linked enemy truck package in one operation
			; at the attacker's original spawn-centroid entry waypoint.
			(define "ea_place_motor_visible"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"placement"
							{selector {ignore_captured_by_user 0} {tag ea_deploy}}
							{target_waypoint "attack_support_entry_a"}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"placement"
							{selector {ignore_captured_by_user 0} {tag ea_deploy}}
							{target_waypoint "attack_support_entry_b"}
						}
					}
					{"default"
						{"placement"
							{selector {ignore_captured_by_user 0} {tag ea_deploy}}
							{target_waypoint "attack_support_entry_a"}
						}
					}
				}
			)
'''


def patch_motor_blocks(text: str, *, namespace: str, old_call: str, new_call: str) -> str:
    markers = []
    cursor = 0
    needle = f'{{"{namespace}/'
    while True:
        start = text.find(needle, cursor)
        if start < 0:
            break
        name_end = text.find('"', start + 2)
        if name_end < 0:
            break
        marker = text[start : name_end + 1]
        if marker.endswith('_motor"'):
            markers.append(marker)
        cursor = name_end + 1

    if len(markers) < 3:
        raise PatchError(f"Expected at least three {namespace} motor triggers, found {len(markers)}")

    patched = text
    changed = 0
    for marker in markers:
        start, end, block = named_block(patched, marker)
        if new_call in block:
            continue
        if block.count(old_call) != 1:
            raise PatchError(f"{marker}: expected exactly one {old_call}")
        block = block.replace(old_call, new_call, 1)
        patched = patched[:start] + block + patched[end:]
        changed += 1

    if changed == 0 and not all(new_call in named_block(patched, marker)[2] for marker in markers):
        raise PatchError(f"No {namespace} motor placement calls were changed")
    return patched


def insert_macro(text: str, *, macro_name: str, macro: str, anchor: str) -> str:
    marker = f'(define "{macro_name}"'
    if marker in text:
        start = text.find(marker)
        brace = text.find("(", start)
        if brace < 0:
            raise PatchError(f"Missing macro opening: {macro_name}")
        depth = 0
        end = None
        in_string = False
        escaped = False
        for index in range(brace, len(text)):
            char = text[index]
            if in_string:
                if escaped:
                    escaped = False
                elif char == "\\":
                    escaped = True
                elif char == '"':
                    in_string = False
                continue
            if char == '"':
                in_string = True
            elif char == "(":
                depth += 1
            elif char == ")":
                depth -= 1
                if depth == 0:
                    end = index + 1
                    break
        if end is None:
            raise PatchError(f"Unbalanced macro: {macro_name}")
        return text[:start] + macro.strip("\n") + text[end:]

    pos = text.find(anchor)
    if pos < 0:
        raise PatchError(f"Missing insertion anchor: {anchor}")
    return text[:pos] + macro + "\n" + text[pos:]


def validate_multi_root(multi_root: Path) -> None:
    attack = (multi_root / "attack_support_waves.inc").read_text(encoding="utf-8-sig")
    enemy = (multi_root / "enemy_attack_support.inc").read_text(encoding="utf-8-sig")

    required_attack = (
        '(define "as_place_motor_visible"',
        'target_waypoint "attack_support_entry_b"',
        'target_waypoint "attack_support_entry_a"',
    )
    required_enemy = (
        '(define "ea_place_motor_visible"',
        'target_waypoint "attack_support_entry_a"',
        'target_waypoint "attack_support_entry_b"',
    )
    forbidden = (
        'target_waypoint "attack_support_rear_a1"',
        'target_waypoint "attack_support_rear_b1"',
    )

    for token in required_attack:
        if token not in attack:
            raise PatchError(f"attack_support_waves.inc missing {token}")
    for token in required_enemy:
        if token not in enemy:
            raise PatchError(f"enemy_attack_support.inc missing {token}")
    for token in forbidden:
        if token in named_block(attack, '(define "as_place_motor_visible"')[2]:
            raise PatchError(f"attack motor helper still uses off-map rear pad: {token}")
        if token in named_block(enemy, '(define "ea_place_motor_visible"')[2]:
            raise PatchError(f"enemy motor helper still uses off-map rear pad: {token}")

    for text, namespace, call in (
        (attack, "attack_support", '("as_place_motor_visible")'),
        (enemy, "enemy_attack", '("ea_place_motor_visible")'),
    ):
        cursor = 0
        count = 0
        needle = f'{{"{namespace}/'
        while True:
            start = text.find(needle, cursor)
            if start < 0:
                break
            name_end = text.find('"', start + 2)
            marker = text[start : name_end + 1]
            cursor = name_end + 1
            if not marker.endswith('_motor"'):
                continue
            _, _, block = named_block(text, marker)
            if call not in block:
                raise PatchError(f"{marker} does not use whole-package base-entry placement")
            count += 1
        if count < 3:
            raise PatchError(f"Expected at least three validated {namespace} motor triggers")


def patch_multi_root(multi_root: Path) -> None:
    attack_path = multi_root / "attack_support_waves.inc"
    enemy_path = multi_root / "enemy_attack_support.inc"
    for path in (attack_path, enemy_path):
        if not path.is_file():
            raise PatchError(f"Missing deployed support engine: {path}")

    attack = attack_path.read_text(encoding="utf-8-sig")
    enemy = enemy_path.read_text(encoding="utf-8-sig")

    attack = insert_macro(
        attack,
        macro_name="as_place_motor_visible",
        macro=ATTACK_MACRO,
        anchor='\t\t\t; ===== MOTORIZED INSERT (cmd 19)',
    )
    enemy = insert_macro(
        enemy,
        macro_name="ea_place_motor_visible",
        macro=ENEMY_MACRO,
        anchor='\t\t\t; ===== MOTORIZED INSERT (cmd 19)',
    )

    attack = patch_motor_blocks(
        attack,
        namespace="attack_support",
        old_call='("am_place_at_entry")',
        new_call='("as_place_motor_visible")',
    )
    enemy = patch_motor_blocks(
        enemy,
        namespace="enemy_attack",
        old_call='("ea_place_at_entry")',
        new_call='("ea_place_motor_visible")',
    )

    attack_path.write_text(attack, encoding="utf-8")
    enemy_path.write_text(enemy, encoding="utf-8")
    validate_multi_root(multi_root)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--multi-root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    if args.check:
        validate_multi_root(args.multi_root)
    else:
        patch_multi_root(args.multi_root)

    print("Motor package placement ready: whole linked truck at base entry waypoint.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
