#!/usr/bin/env python3
"""Apply the one-shot 30-second motor test to the e74ef6e known-good baseline.

This deployment/test overlay changes only the two motor paths that were
live-proven in commit e74ef6e:

* attack_support/motor_test: friendly attacker truck
* enemy_attack/motor_test: enemy attacker truck

Each path dispatches exactly one truck 30 seconds after its engine arms. The
second staggered test truck is removed. Because that one dispatch consumes the
single motor budget, later production rolls cannot add another truck.
"""

from __future__ import annotations

import argparse
import re
from pathlib import Path

BASELINE = "e74ef6e4a1977e0e7188c2f4a4f360080b7f8353"


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


def replace_two_truck_actions(
    block: str,
    *,
    done_var: str,
    left_var: str,
    cmd_var: str,
    poke: str,
) -> str:
    pattern = re.compile(
        rf'(?P<indent>^[ \t]*)\{{"set_i" \{{var "{re.escape(done_var)}\$"\}} '
        rf'\{{op "="\}} \{{value 1\}}\}}\s*\n'
        rf'^[ \t]*\{{"set_i" \{{var "{re.escape(left_var)}\$"\}} '
        rf'\{{op "="\}} \{{value 2\}}\}}\s*\n'
        rf'^[ \t]*\{{"delay" \{{time 15\}}\}}\s*\n'
        rf'^[ \t]*\{{"set_i" \{{var "{re.escape(cmd_var)}\$"\}} '
        rf'\{{op "="\}} \{{value 19\}}\}}\s*\n'
        rf'^[ \t]*\("{re.escape(poke)}"\)\s*\n'
        rf'^[ \t]*\{{"delay" \{{time 45\}}\}}\s*\n'
        rf'^[ \t]*\{{"set_i" \{{var "{re.escape(cmd_var)}\$"\}} '
        rf'\{{op "="\}} \{{value 19\}}\}}\s*\n'
        rf'^[ \t]*\("{re.escape(poke)}"\)',
        re.MULTILINE,
    )

    match = pattern.search(block)
    if not match:
        raise PatchError(f"Expected two-truck test actions not found for {cmd_var}")

    indent = match.group("indent")
    replacement = (
        f'{indent}{{"set_i" {{var "{done_var}$"}} {{op "="}} {{value 1}}}}\n'
        f'{indent}{{"set_i" {{var "{left_var}$"}} {{op "="}} {{value 1}}}}\n'
        f'{indent}{{"delay" {{time 30}}}}\n'
        f'{indent}{{"set_i" {{var "{cmd_var}$"}} {{op "="}} {{value 19}}}}\n'
        f'{indent}("{poke}")'
    )
    return block[: match.start()] + replacement + block[match.end() :]


def patch_trigger(
    text: str,
    *,
    marker: str,
    done_var: str,
    left_var: str,
    cmd_var: str,
    poke: str,
) -> str:
    start, end, block = named_block(text, marker)

    if (
        '{"delay" {time 30}}' in block
        and f'{{var "{left_var}$"}} {{op "="}} {{value 1}}' in block
        and '{"delay" {time 45}}' not in block
    ):
        return text

    patched = replace_two_truck_actions(
        block,
        done_var=done_var,
        left_var=left_var,
        cmd_var=cmd_var,
        poke=poke,
    )
    return text[:start] + patched + text[end:]


def disable_attack_air_test(text: str) -> str:
    pattern = re.compile(
        r'(\{"set_i" \{var "attack_support_air_test\$"\} '
        r'\{op "="\} \{value )1(\}\})'
    )
    patched, count = pattern.subn(r'\g<1>0\2', text, count=1)
    if count != 1 and 'attack_support_air_test$"} {op "="} {value 0}' not in text:
        raise PatchError("Could not disable the attack-side helicopter test")
    return patched


def validate_multi_root(multi_root: Path) -> None:
    checks = (
        (
            multi_root / "attack_support_waves.inc",
            '{"attack_support/motor_test"',
            "attack_support_motor_left",
            "as_poke_faction_motor",
        ),
        (
            multi_root / "enemy_attack_support.inc",
            '{"enemy_attack/motor_test"',
            "enemy_attack_motor_left",
            "ea_poke_motor",
        ),
    )

    for path, marker, left_var, poke in checks:
        text = path.read_text(encoding="utf-8-sig")
        _, _, block = named_block(text, marker)
        required = (
            f'{{var "{left_var}$"}} {{op "="}} {{value 1}}',
            '{"delay" {time 30}}',
            f'("{poke}")',
        )
        forbidden = (
            f'{{var "{left_var}$"}} {{op "="}} {{value 2}}',
            '{"delay" {time 15}}',
            '{"delay" {time 45}}',
        )
        for token in required:
            if token not in block:
                raise PatchError(f"{path.name}: missing one-shot token: {token}")
        for token in forbidden:
            if token in block:
                raise PatchError(f"{path.name}: stale two-truck token remains: {token}")
        if block.count(f'("{poke}")') != 1:
            raise PatchError(f"{path.name}: expected exactly one motor dispatch")


def patch_multi_root(multi_root: Path) -> None:
    attack_path = multi_root / "attack_support_waves.inc"
    enemy_path = multi_root / "enemy_attack_support.inc"

    for path in (attack_path, enemy_path):
        if not path.is_file():
            raise PatchError(f"Missing deployed support engine: {path}")

    attack = attack_path.read_text(encoding="utf-8-sig")
    enemy = enemy_path.read_text(encoding="utf-8-sig")

    for marker in (
        'Driver leaves the field instead of idling',
        '{"attack_support/motor_cleanup"',
    ):
        if marker not in attack:
            raise PatchError(f"Attack engine is not the {BASELINE[:8]} lifecycle: {marker}")

    for marker in (
        'Driver leaves the field instead of idling',
        '{"enemy_attack/motor_cleanup"',
    ):
        if marker not in enemy:
            raise PatchError(f"Enemy-attack engine is not the {BASELINE[:8]} lifecycle: {marker}")

    attack = disable_attack_air_test(attack)
    attack = patch_trigger(
        attack,
        marker='{"attack_support/motor_test"',
        done_var="attack_support_motor_test_done",
        left_var="attack_support_motor_left",
        cmd_var="attack_support_wave_cmd",
        poke="as_poke_faction_motor",
    )
    enemy = patch_trigger(
        enemy,
        marker='{"enemy_attack/motor_test"',
        done_var="enemy_attack_motor_test_done",
        left_var="enemy_attack_motor_left",
        cmd_var="enemy_attack_wave_cmd",
        poke="ea_poke_motor",
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

    print("Known-good motor test ready: one truck at +30s in each proven attacker path.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
