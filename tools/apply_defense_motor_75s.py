#!/usr/bin/env python3
"""Make both player-defense transports drive 75s, stop, then dismount.

Applied after the movement/origin-exit correction:
- friendly defender: 75 seconds of driving, explicit stop, passenger emit
- enemy attacker: 2-second drive retry + 73 seconds, explicit stop, passenger emit
- passengers are AI-movement-disabled while mounted, then enabled after emit

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

DEPLOY_TAGS = {
    "ea": "ea_deploy",
    "ds": "def_sup_deploy",
}

HULL_TAGS = {
    "ea": "ea_motor_hull",
    "ds": "def_sup_motor_hull",
}

PAX_TAGS = {
    "ea": "ea_motor_pax",
    "ds": "def_sup_motor_pax",
}

EXIT_HELPERS = {
    "ea": "ea_exit_motor_to_origin",
    "ds": "ds_exit_motor_to_origin",
}

HOLD_MARKER = "; PASSENGERS HELD IN LINKED SEATS UNTIL TIMED EMIT"
STOP_MARKER = "; TIMED DROP: STOP HULL BEFORE PASSENGER EMIT"
RELEASE_MARKER = "; PASSENGERS RELEASED TO AI AFTER EMIT"
EXIT_RESUME_MARKER = "; EMPTY HULL RESUMES NORMAL SPEED FOR ORIGIN EXIT"


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


def brace_block_containing(text: str, token: str, label: str) -> tuple[int, int, str]:
    token_at = text.find(token)
    if token_at < 0:
        raise PatchError(f"{label}: missing {token}")
    start = text.rfind('{"actor_state"', 0, token_at)
    if start < 0:
        raise PatchError(f"{label}: actor_state opener not found")
    end = balanced(text, start, "{", "}", label)
    if token_at >= end:
        raise PatchError(f"{label}: token is outside actor_state block")
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


def render_hold(prefix: str, indent: str) -> str:
    pax = PAX_TAGS[prefix]
    return (
        f'{indent}{HOLD_MARKER}\n'
        f'{indent}{{"actor_state"\n'
        f'{indent}\t{{selector {{ignore_captured_by_user 0}} {{tag {pax}}}}}\n'
        f'{indent}\t{{control AI}}\n'
        f'{indent}\t{{ai_move {{mode disable}}}}\n'
        f'{indent}\t{{movement {{speed stop}}}}\n'
        f'{indent}}}'
    )


def render_hull_crew_selector(prefix: str) -> str:
    deploy = DEPLOY_TAGS[prefix]
    pax = PAX_TAGS[prefix]
    return (
        "{selector {ignore_captured_by_user 0} {source advanced} "
        f"{{group {{select {{tag {{tag {deploy}}}}}}} "
        f"{{exclude {{tag {{tag {pax}}}}}}}}}}}"
    )


def patch_mounted_control(text: str, prefix: str) -> str:
    start, end, block = paren_block(text, FINISHERS[prefix])
    deploy = DEPLOY_TAGS[prefix]

    # Insert the passenger hold immediately before the ownership-settle delay.
    if HOLD_MARKER not in block:
        own_call = '("ea_own_to_enemy")' if prefix == "ea" else '("ds_own_to_defenderbot")'
        own_at = block.find(own_call)
        if own_at < 0:
            raise PatchError(f"{prefix}: ownership call is missing")
        delay_at = block.rfind('{"delay" {time 0.2}}', 0, own_at)
        if delay_at < 0:
            raise PatchError(f"{prefix}: pre-ownership 0.2-second delay is missing")
        line_start = block.rfind("\n", 0, delay_at) + 1
        indent = block[line_start:delay_at]
        hold = render_hold(prefix, indent)
        block = block[:line_start] + hold + "\n" + block[line_start:]

    # The pre-drive AI state must apply to hull + cab crew, never passengers.
    simple_selector = f'{{selector {{ignore_captured_by_user 0}} {{tag {deploy}}}}}'
    desired_selector = render_hull_crew_selector(prefix)
    actor_token = simple_selector + "\n"
    actor_at = block.find(actor_token)
    if actor_at >= 0:
        state_start = block.rfind('{"actor_state"', 0, actor_at)
        state_end = balanced(block, state_start, "{", "}", f"{prefix} pre-drive actor_state")
        state = block[state_start:state_end]
        if '{control AI}' not in state or '{ai_move {mode enable}}' not in state:
            raise PatchError(f"{prefix}: selected actor_state is not the pre-drive AI block")
        state = state.replace(simple_selector, desired_selector, 1)
        block = block[:state_start] + state + block[state_end:]
    elif desired_selector not in block:
        raise PatchError(f"{prefix}: pre-drive actor selector is neither original nor patched")

    return text[:start] + block + text[end:]


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


def patch_stop_before_emit(text: str, prefix: str) -> str:
    start, end, block = paren_block(text, FINISHERS[prefix])
    if STOP_MARKER in block:
        return text

    ride_token = '{"delay" {time 73}}' if prefix == "ea" else '{"delay" {time 75}}'
    ride_at = block.find(ride_token)
    if ride_at < 0:
        raise PatchError(f"{prefix}: timed ride delay is missing")
    emit_at = block.find('{"emit"', ride_at)
    if emit_at < 0:
        raise PatchError(f"{prefix}: passenger emit is missing after ride delay")
    between = block[ride_at + len(ride_token):emit_at]
    if between.strip():
        raise PatchError(f"{prefix}: unexpected actions already exist between ride and emit")

    line_start = block.rfind("\n", 0, emit_at) + 1
    indent = block[line_start:emit_at]
    stop = render_stop(prefix, indent)
    block = block[:line_start] + stop + "\n" + block[line_start:]
    return text[:start] + block + text[end:]


def render_release(prefix: str, indent: str) -> str:
    pax = PAX_TAGS[prefix]
    return (
        f'{indent}{RELEASE_MARKER}\n'
        f'{indent}{{"actor_state"\n'
        f'{indent}\t{{selector {{ignore_captured_by_user 0}} {{tag {pax}}}}}\n'
        f'{indent}\t{{control AI}}\n'
        f'{indent}\t{{ai_move {{mode enable}}}}\n'
        f'{indent}\t{{weapon_prepare on}}\n'
        f'{indent}\t{{fire_mode open}}\n'
        f'{indent}\t{{move_mode free}}\n'
        f'{indent}\t{{movement {{speed normal}} {{kind normal}} {{type normal}}}}\n'
        f'{indent}\t{{ai {{no_retreat on}} {{advance_ratio 1}} {{retreat_ratio 0}}}}\n'
        f'{indent}}}'
    )


def patch_release_after_emit(text: str, prefix: str) -> str:
    start, end, block = paren_block(text, FINISHERS[prefix])
    if RELEASE_MARKER in block:
        return text

    emit_at = block.find('{"emit"')
    if emit_at < 0:
        raise PatchError(f"{prefix}: passenger emit is missing")
    emit_end = balanced(block, emit_at, "{", "}", f"{prefix} emit")
    delay_at = block.find('{"delay" {time 3}}', emit_end)
    if delay_at < 0:
        raise PatchError(f"{prefix}: post-emit 3-second delay is missing")
    if block[emit_end:delay_at].strip():
        raise PatchError(f"{prefix}: unexpected actions already exist after emit")

    line_start = block.rfind("\n", 0, delay_at) + 1
    indent = block[line_start:delay_at]
    release = render_release(prefix, indent)
    block = block[:line_start] + release + "\n" + block[line_start:]
    return text[:start] + block + text[end:]


def render_exit_resume(prefix: str, indent: str) -> str:
    hull = HULL_TAGS[prefix]
    return (
        f'{indent}{EXIT_RESUME_MARKER}\n'
        f'{indent}{{"actor_state"\n'
        f'{indent}\t{{selector {{ignore_captured_by_user 0}} {{tag {hull}}}}}\n'
        f'{indent}\t{{control AI}}\n'
        f'{indent}\t{{ai_move {{mode enable}}}}\n'
        f'{indent}\t{{move_mode free}}\n'
        f'{indent}\t{{movement {{speed normal}} {{kind normal}} {{type normal}}}}\n'
        f'{indent}}}'
    )


def patch_exit_resume(text: str, prefix: str) -> str:
    start, end, block = paren_block(text, FINISHERS[prefix])
    if EXIT_RESUME_MARKER in block:
        return text

    helper_call = f'("{EXIT_HELPERS[prefix]}")'
    call_at = block.find(helper_call)
    if call_at < 0:
        raise PatchError(f"{prefix}: origin-exit helper call is missing")
    line_start = block.rfind("\n", 0, call_at) + 1
    indent = block[line_start:call_at]
    resume = render_exit_resume(prefix, indent)
    block = block[:line_start] + resume + "\n" + block[line_start:]
    return text[:start] + block + text[end:]


def patch_file(text: str, prefix: str) -> str:
    if prefix == "ea":
        text = replace_delay(text, prefix, 58, 73)
    else:
        text = replace_delay(text, prefix, 60, 75)
    text = patch_mounted_control(text, prefix)
    text = patch_stop_before_emit(text, prefix)
    text = patch_release_after_emit(text, prefix)
    text = patch_exit_resume(text, prefix)
    return text


def read_text(path: Path) -> tuple[str, bool]:
    raw = path.read_bytes()
    return raw.decode("utf-8-sig"), raw.startswith(b"\xef\xbb\xbf")


def write_text(path: Path, text: str, bom: bool) -> None:
    raw = text.encode("utf-8")
    path.write_bytes((b"\xef\xbb\xbf" if bom else b"") + raw)


def validate_finisher(block: str, prefix: str) -> None:
    deploy = DEPLOY_TAGS[prefix]
    pax = PAX_TAGS[prefix]
    hull = HULL_TAGS[prefix]

    for marker in (HOLD_MARKER, STOP_MARKER, RELEASE_MARKER, EXIT_RESUME_MARKER):
        if block.count(marker) != 1:
            raise PatchError(f"{prefix}: expected one marker {marker}")

    desired_selector = render_hull_crew_selector(prefix)
    if desired_selector not in block:
        raise PatchError(f"{prefix}: pre-drive hull+crew selector is missing")
    if f'{{selector {{ignore_captured_by_user 0}} {{tag {deploy}}}}}\n\t\t\t\t\t{{control AI}}' in block:
        raise PatchError(f"{prefix}: passengers still receive the pre-drive AI block")

    hold_at = block.find(HOLD_MARKER)
    drive_at = block.find('{action advance}')
    stop_at = block.find(STOP_MARKER)
    emit_at = block.find('{"emit"')
    release_at = block.find(RELEASE_MARKER)
    pax_advance_at = block.find(f'{{selector {{ignore_captured_by_user 0}} {{tag {pax}}}}}', release_at + 1)
    exit_resume_at = block.find(EXIT_RESUME_MARKER)
    exit_call_at = block.find(f'("{EXIT_HELPERS[prefix]}")')

    if not (0 <= hold_at < drive_at < stop_at < emit_at < release_at < pax_advance_at < exit_resume_at < exit_call_at):
        raise PatchError(f"{prefix}: mounted/stop/emit/release/exit order is invalid")

    if block.count('{"delay" {time 1}}') != 1:
        raise PatchError(f"{prefix}: hull must settle for exactly one second before emit")
    if block.count('{mode passengers}') != 1:
        raise PatchError(f"{prefix}: passenger-only emit contract changed")
    if block.count(f'{{tag {hull}}}') < 4:
        raise PatchError(f"{prefix}: hull lifecycle selectors are incomplete")
    if block.count(f'{{tag {pax}}}') < 4:
        raise PatchError(f"{prefix}: passenger lifecycle selectors are incomplete")


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
    for marker in (HOLD_MARKER, STOP_MARKER, RELEASE_MARKER, EXIT_RESUME_MARKER):
        if marker in attacker_finish:
            raise PatchError("Friendly-attacker path was modified by defense timing overlay")


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
        print("Player-defense motors validated: 75s drive, stop, then passenger emit.")
    else:
        changed = apply(args.root)
        print(f"75-second stop-and-dismount lifecycle patched {len(changed)} file(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
