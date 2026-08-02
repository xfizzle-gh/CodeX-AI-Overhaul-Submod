#!/usr/bin/env python3
"""Preserve the tested player-defense motor lifecycle and align its drop point.

Applied after the movement/origin-exit correction that passed runtime:
- friendly defender: 60 -> 75 seconds before the turnaround sequence
- enemy attacker: retry remains at 2 seconds, remaining ride 58 -> 73 seconds
- at 75 seconds, issue the tested origin-return order first
- pause the hull, emit the existing passenger group, then resume the return
- after the hull's final return command, reassert the infantry attack order

No passenger AI, ownership, seating, placement, or emit selector is changed.
The friendly-attacker runtime-proven path remains at 60 seconds.
"""
from __future__ import annotations

import argparse
from pathlib import Path


class PatchError(RuntimeError):
    pass


FILES = {
    "ea": "enemy_attack_support.inc",
    "ds": "defense_support_waves.inc",
}

FINISHERS = {
    "ea": "ea_finish_motor",
    "ds": "ds_finish_motor",
}

HULL_TAGS = {
    "ea": "ea_motor_hull",
    "ds": "def_sup_motor_hull",
}

PAX_TAGS = {
    "ea": "ea_motor_pax",
    "ds": "def_sup_motor_pax",
}

FLAG_TAGS = {
    "ea": "ea_flag1",
    "ds": "def_sup_motor_flag",
}

EXIT_HELPERS = {
    "ea": "ea_exit_motor_to_origin",
    "ds": "ds_exit_motor_to_origin",
}

PRETURN_MARKER = "; TIMED DROP ALIGNMENT — BEGIN ORIGIN TURN BEFORE PASSENGER EMIT"
STOP_MARKER = "; TIMED DROP ALIGNMENT — PAUSE HULL AT TURNAROUND"
RESUME_MARKER = "; RESTORE TESTED EMPTY-HULL SPEED BEFORE FINAL ORIGIN EXIT"
REASSERT_MARKER = "; KEEP DISEMBARKED INFANTRY ATTACKING AFTER HULL WITHDRAWS"

FORBIDDEN_MARKERS = (
    "; PASSENGERS HELD IN LINKED SEATS UNTIL TIMED EMIT",
    "; PASSENGERS RELEASED TO AI AFTER EMIT",
    "; EMPTY HULL RESUMES NORMAL SPEED FOR ORIGIN EXIT",
)


def balanced(text: str, start: int, opener: str, closer: str, label: str) -> int:
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
                return index + 1
    raise PatchError(f"Unbalanced {label}")


def paren_block(text: str, name: str) -> tuple[int, int, str]:
    marker = f'(define "{name}"'
    start = text.find(marker)
    if start < 0:
        raise PatchError(f"Missing macro {name}")
    end = balanced(text, start, "(", ")", name)
    return start, end, text[start:end]


def replace_delay(text: str, prefix: str, old: int, new: int) -> str:
    start, end, block = paren_block(text, FINISHERS[prefix])
    old_token = f'{{"delay" {{time {old}}}}}'
    new_token = f'{{"delay" {{time {new}}}}}'

    if new_token in block and old_token not in block:
        return text
    if block.count(old_token) != 1:
        raise PatchError(
            f"{prefix} finisher expected exactly one {old}-second delay, "
            f"found {block.count(old_token)}"
        )

    patched = block.replace(old_token, new_token, 1)
    return text[:start] + patched + text[end:]


def render_preturn(prefix: str, indent: str) -> str:
    helper = EXIT_HELPERS[prefix]
    return (
        f'{indent}{PRETURN_MARKER}\n'
        f'{indent}("{helper}")\n'
        f'{indent}{{"delay" {{time 0.5}}}}'
    )


def render_stop(prefix: str, indent: str) -> str:
    hull = HULL_TAGS[prefix]
    return (
        f'{indent}{STOP_MARKER}\n'
        f'{indent}{{"actor_state"\n'
        f'{indent}\t{{selector {{ignore_captured_by_user 0}} {{tag {hull}}}}}\n'
        f'{indent}\t{{movement {{speed stop}}}}\n'
        f'{indent}}}\n'
        f'{indent}{{"delay" {{time 1}}}}'
    )


def render_resume(prefix: str, indent: str) -> str:
    hull = HULL_TAGS[prefix]
    return (
        f'{indent}{RESUME_MARKER}\n'
        f'{indent}{{"actor_state"\n'
        f'{indent}\t{{selector {{ignore_captured_by_user 0}} {{tag {hull}}}}}\n'
        f'{indent}\t{{movement {{speed normal}} {{kind normal}} {{type normal}}}}\n'
        f'{indent}}}'
    )


def render_reassert(prefix: str, indent: str) -> str:
    pax = PAX_TAGS[prefix]
    flag = FLAG_TAGS[prefix]
    return (
        f'{indent}{REASSERT_MARKER}\n'
        f'{indent}{{"delay" {{time 0.25}}}}\n'
        f'{indent}{{"action"\n'
        f'{indent}\t{{selector {{ignore_captured_by_user 0}} {{tag {pax}}}}}\n'
        f'{indent}\t{{drop orders}}\n'
        f'{indent}\t{{action advance}}\n'
        f'{indent}\t{{target {{ignore_captured_by_user 0}} {{tag {flag}}}}}\n'
        f'{indent}}}'
    )


def patch_turn_emit_and_reassert(text: str, prefix: str) -> str:
    start, end, block = paren_block(text, FINISHERS[prefix])
    helper_call = f'("{EXIT_HELPERS[prefix]}")'

    if PRETURN_MARKER not in block:
        ride_token = '{"delay" {time 73}}' if prefix == "ea" else '{"delay" {time 75}}'
        ride_at = block.find(ride_token)
        if ride_at < 0:
            raise PatchError(f"{prefix}: 75-second ride token is missing")
        emit_at = block.find('{"emit"', ride_at)
        if emit_at < 0:
            raise PatchError(f"{prefix}: existing passenger emit is missing")
        line_start = block.rfind("\n", 0, emit_at) + 1
        indent = block[line_start:emit_at]
        insertion = render_preturn(prefix, indent) + "\n" + render_stop(prefix, indent)
        block = block[:line_start] + insertion + "\n" + block[line_start:]

    if RESUME_MARKER not in block:
        helper_positions = []
        cursor = 0
        while True:
            pos = block.find(helper_call, cursor)
            if pos < 0:
                break
            helper_positions.append(pos)
            cursor = pos + len(helper_call)
        if len(helper_positions) < 2:
            raise PatchError(f"{prefix}: expected preturn and final origin-exit calls")
        final_helper_at = helper_positions[-1]
        line_start = block.rfind("\n", 0, final_helper_at) + 1
        indent = block[line_start:final_helper_at]
        block = block[:line_start] + render_resume(prefix, indent) + "\n" + block[line_start:]

    if REASSERT_MARKER not in block:
        final_helper_at = block.rfind(helper_call)
        if final_helper_at < 0:
            raise PatchError(f"{prefix}: final origin-exit helper is missing")
        helper_line_end = block.find("\n", final_helper_at)
        if helper_line_end < 0:
            helper_line_end = final_helper_at + len(helper_call)
        indent_start = block.rfind("\n", 0, final_helper_at) + 1
        indent = block[indent_start:final_helper_at]
        block = block[:helper_line_end + 1] + render_reassert(prefix, indent) + "\n" + block[helper_line_end + 1:]

    return text[:start] + block + text[end:]


def patch_file(text: str, prefix: str) -> str:
    if prefix == "ea":
        text = replace_delay(text, prefix, 58, 73)
    else:
        text = replace_delay(text, prefix, 60, 75)
    return patch_turn_emit_and_reassert(text, prefix)


def read_text(path: Path) -> tuple[str, bool]:
    raw = path.read_bytes()
    return raw.decode("utf-8-sig"), raw.startswith(b"\xef\xbb\xbf")


def write_text(path: Path, text: str, bom: bool) -> None:
    raw = text.encode("utf-8")
    path.write_bytes((b"\xef\xbb\xbf" if bom else b"") + raw)


def validate_finisher(block: str, prefix: str) -> None:
    hull = HULL_TAGS[prefix]
    pax = PAX_TAGS[prefix]
    flag = FLAG_TAGS[prefix]
    ride_token = '{"delay" {time 73}}' if prefix == "ea" else '{"delay" {time 75}}'
    helper_call = f'("{EXIT_HELPERS[prefix]}")'

    for marker in FORBIDDEN_MARKERS:
        if marker in block:
            raise PatchError(f"{prefix}: abandoned passenger-AI rewrite marker is present")

    for marker in (PRETURN_MARKER, STOP_MARKER, RESUME_MARKER, REASSERT_MARKER):
        if block.count(marker) != 1:
            raise PatchError(f"{prefix}: expected exactly one marker {marker}")
    if block.count(helper_call) != 2:
        raise PatchError(f"{prefix}: expected exactly two origin helper calls")
    if block.count('{"delay" {time 0.5}}') != 1:
        raise PatchError(f"{prefix}: expected one half-second turn dwell")
    if block.count('{"delay" {time 1}}') != 1:
        raise PatchError(f"{prefix}: expected one one-second stop dwell")
    if block.count('{mode passengers}') != 1:
        raise PatchError(f"{prefix}: passenger-only emit contract changed")

    ride_at = block.find(ride_token)
    preturn_at = block.find(PRETURN_MARKER)
    stop_at = block.find(STOP_MARKER)
    emit_at = block.find('{"emit"', stop_at)
    resume_at = block.find(RESUME_MARKER)
    final_helper_at = block.rfind(helper_call)
    reassert_at = block.find(REASSERT_MARKER)
    if not (0 <= ride_at < preturn_at < stop_at < emit_at < resume_at < final_helper_at < reassert_at):
        raise PatchError(f"{prefix}: ride/turn/stop/emit/exit/reassert order is invalid")

    stop_block = block[stop_at:emit_at]
    if f'{{tag {hull}}}' not in stop_block or '{movement {speed stop}}' not in stop_block:
        raise PatchError(f"{prefix}: hull stop state is incomplete")

    reassert_block = block[reassert_at:]
    for token in (
        f'{{tag {pax}}}',
        '{drop orders}',
        '{action advance}',
        f'{{tag {flag}}}',
    ):
        if token not in reassert_block:
            raise PatchError(f"{prefix}: post-exit infantry reassert missing {token}")


def validate(root: Path) -> None:
    multi = root / "resource/map/multi"
    enemy = (multi / FILES["ea"]).read_text(encoding="utf-8-sig")
    defender = (multi / FILES["ds"]).read_text(encoding="utf-8-sig")
    attacker = (multi / "attack_support_waves.inc").read_text(encoding="utf-8-sig")

    _, _, enemy_finish = paren_block(enemy, FINISHERS["ea"])
    _, _, defender_finish = paren_block(defender, FINISHERS["ds"])
    _, _, attacker_finish = paren_block(attacker, "as_finish_motor")

    if enemy_finish.count('{"delay" {time 2}}') != 1:
        raise PatchError("Enemy movement retry must remain at 2 seconds")
    if enemy_finish.count('{"delay" {time 73}}') != 1:
        raise PatchError("Enemy remaining ride must be exactly 73 seconds")
    if '{"delay" {time 58}}' in enemy_finish:
        raise PatchError("Enemy 58-second remainder was not replaced")

    if defender_finish.count('{"delay" {time 75}}') != 1:
        raise PatchError("Friendly defender ride must be exactly 75 seconds")
    if '{"delay" {time 60}}' in defender_finish:
        raise PatchError("Friendly defender 60-second delay was not replaced")

    validate_finisher(enemy_finish, "ea")
    validate_finisher(defender_finish, "ds")

    if attacker_finish.count('{"delay" {time 60}}') != 1:
        raise PatchError("Friendly-attacker validated 60-second timing changed")
    for marker in (PRETURN_MARKER, STOP_MARKER, RESUME_MARKER, REASSERT_MARKER):
        if marker in attacker_finish:
            raise PatchError("Friendly-attacker path was modified")


def apply(root: Path, *, check_only: bool = False) -> list[Path]:
    multi = root / "resource/map/multi"
    changed: list[Path] = []
    results: list[tuple[Path, str, bool]] = []

    for prefix, filename in FILES.items():
        path = multi / filename
        if not path.is_file():
            raise PatchError(f"Missing deployed support engine: {path}")
        text, bom = read_text(path)
        patched = patch_file(text, prefix)
        results.append((path, patched, bom))
        if patched != text:
            changed.append(path)

    if not check_only:
        for path, patched, bom in results:
            write_text(path, patched, bom)
        validate(root)
    return changed


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    if args.check:
        validate(args.root)
        print("Player-defense motors validated: turn, pause, emit, withdraw, infantry reassert.")
    else:
        changed = apply(args.root)
        print(f"Turnaround-aligned passenger drop patched {len(changed)} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
