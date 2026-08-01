#!/usr/bin/env python3
"""Apply the canonical production motor lifecycle to all four support engines."""
from __future__ import annotations

import argparse
import re
from dataclasses import dataclass
from pathlib import Path

MARKER = "; CANONICAL MOTOR PRODUCTION OVERLAY"
FACTIONS = ("rusa", "ukr", "nato", "prc")


class PatchError(RuntimeError):
    pass


@dataclass(frozen=True)
class Engine:
    relative_path: str
    namespace: str
    finisher: str
    placer_call: str
    placer_macro: str
    deploy_tag: str
    hull_tag: str
    leaving_tag: str
    side_one_entry: str
    side_two_entry: str
    default_entry: str
    trigger_pattern: str


ENGINES = (
    Engine("resource/map/multi/attack_support_waves.inc", "attack_support", "as_finish_motor", "am_place_at_entry", "as_place_motor_package", "attack_support_deploy", "attack_support_motor_hull", "am_motor_leaving", "attack_support_entry_b", "attack_support_entry_a", "attack_support_entry_b", "attack_support/ally_{faction}_motor"),
    Engine("resource/map/multi/defense_support_waves.inc", "defense_support", "ds_finish_motor", "ds_place_at_entry", "ds_place_motor_package", "def_sup_deploy", "def_sup_motor_hull", "def_sup_motor_leaving", "attack_support_entry_b", "attack_support_entry_a", "attack_support_entry_b", "defense_support/ally_{faction}_motor"),
    Engine("resource/map/multi/enemy_attack_support.inc", "enemy_attack", "ea_finish_motor", "ea_place_at_entry", "ea_place_motor_package", "ea_deploy", "ea_motor_hull", "ea_motor_leaving", "attack_support_entry_a", "attack_support_entry_b", "attack_support_entry_a", "enemy_attack/{faction}_motor"),
    Engine("resource/map/multi/enemy_defense_support.inc", "enemy_defense", "ed_finish_motor", "ed_place", "ed_place_motor_package", "enemy_def_deploy", "enemy_def_motor_hull", "enemy_def_motor_leaving", "attack_support_entry_a", "attack_support_entry_b", "attack_support_entry_a", "enemy_defense/{faction}_motor"),
)

FIXED_WAIT = re.compile(
    r'(?P<indent>^[\t ]*)\{"delay"\s+\{time\s+7\}\}\s*\r?\n'
    r'(?P=indent)\{"set_i"\s+\{var\s+"[^"\r\n]*motor_drive_t\$"\}\s+\{op\s+"="\}\s+\{value\s+1\}\}\s*\r?\n'
    r'(?P=indent)\{"delay"\s+\{time\s+7\}\}\s*\r?\n'
    r'(?P=indent)\{"set_i"\s+\{var\s+"[^"\r\n]*motor_drive_t\$"\}\s+\{op\s+"="\}\s+\{value\s+2\}\}\s*\r?\n'
    r'(?P=indent)\{"delay"\s+\{time\s+7\}\}\s*\r?\n'
    r'(?P=indent)\{"set_i"\s+\{var\s+"[^"\r\n]*motor_drive_t\$"\}\s+\{op\s+"="\}\s+\{value\s+3\}\}\s*\r?\n'
    r'(?P=indent)\{"delay"\s+\{time\s+7\}\}\s*\r?\n'
    r'(?P=indent)\{"set_i"\s+\{var\s+"[^"\r\n]*motor_drive_t\$"\}\s+\{op\s+"="\}\s+\{value\s+4\}\}\s*\r?\n'
    r'(?P=indent)\("[^"\r\n]*motor_band"\)',
    re.MULTILINE,
)


def balanced(text: str, marker: str, opener: str, closer: str) -> tuple[int, int, str]:
    start = text.find(marker)
    if start < 0:
        raise PatchError(f"Missing marker: {marker}")
    begin = text.find(opener, start)
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
                return begin, index + 1, text[begin:index + 1]
    raise PatchError(f"Unbalanced block: {marker}")


def paren_block(text: str, marker: str) -> tuple[int, int, str]:
    return balanced(text, marker, "(", ")")


def brace_block(text: str, marker: str) -> tuple[int, int, str]:
    return balanced(text, marker, "{", "}")


def upsert_define(text: str, name: str, rendered: str, before_marker: str) -> str:
    marker = f'(define "{name}"'
    if marker in text:
        start, end, _ = paren_block(text, marker)
        line_start = text.rfind("\n", 0, start) + 1
        return text[:line_start] + rendered + text[end:]
    pos = text.find(before_marker)
    if pos < 0:
        raise PatchError(f"Missing insertion anchor {before_marker}")
    line_start = text.rfind("\n", 0, pos) + 1
    return text[:line_start] + rendered + "\n\n" + text[line_start:]


def render_placer(engine: Engine) -> str:
    i = "\t\t\t"
    lines = [
        i + '(define "' + engine.placer_macro + '"',
        i + '\t{"switch"',
        i + '\t\t{"case"',
        i + '\t\t\t{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}',
        i + '\t\t\t{"placement"',
        i + '\t\t\t\t{selector {ignore_captured_by_user 0} {tag ' + engine.deploy_tag + '}}',
        i + '\t\t\t\t{target_waypoint "' + engine.side_one_entry + '"}',
        i + '\t\t\t}',
        i + '\t\t}',
        i + '\t\t{"case"',
        i + '\t\t\t{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}',
        i + '\t\t\t{"placement"',
        i + '\t\t\t\t{selector {ignore_captured_by_user 0} {tag ' + engine.deploy_tag + '}}',
        i + '\t\t\t\t{target_waypoint "' + engine.side_two_entry + '"}',
        i + '\t\t\t}',
        i + '\t\t}',
        i + '\t\t{"default"',
        i + '\t\t\t{"placement"',
        i + '\t\t\t\t{selector {ignore_captured_by_user 0} {tag ' + engine.deploy_tag + '}}',
        i + '\t\t\t\t{target_waypoint "' + engine.default_entry + '"}',
        i + '\t\t\t}',
        i + '\t\t}',
        i + '\t}',
        i + ')',
    ]
    return "\n".join(lines)


def patch_templates(text: str) -> str:
    lines = text.splitlines()
    hulls: dict[tuple[str, int], str] = {}
    pax: dict[tuple[str, int], list[str]] = {}
    for index, line in enumerate(lines):
        match = re.search(r'\s(0x[0-9a-fA-F]+)\}\s*$', line)
        if not match or "{Tags " not in line:
            continue
        entity_id = match.group(1).lower()
        for faction in FACTIONS:
            for package in range(1, 5):
                key = (faction, package)
                if f"ally_sup_{faction}_p{package}_hull" in line:
                    hulls[key] = entity_id
                if f"ally_sup_{faction}_p{package}_pax" in line:
                    pax.setdefault(key, []).append(entity_id)
                    if '"sup_linked"' not in line:
                        insert = line.rfind(" " + match.group(1))
                        if insert < 0:
                            raise PatchError(f"Could not tag linked passenger: {line}")
                        lines[index] = line[:insert] + ' "sup_linked"' + line[insert:]
    expected = {(f, p) for f in FACTIONS for p in range(1, 5)}
    if set(hulls) != expected:
        raise PatchError(f"Expected 16 numbered motor hulls, found {len(hulls)}")
    for key in expected:
        ids = sorted(set(pax.get(key, [])), key=lambda value: int(value, 16))
        if not 4 <= len(ids) <= 8:
            raise PatchError(
                f"{key}: expected a valid 4-8 passenger roster, found {len(ids)}"
            )
        pax[key] = ids
    patched = "\n".join(lines) + ("\n" if text.endswith("\n") else "")
    links = {(body.lower(), hull.lower(), slot) for body, hull, slot in re.findall(r'\{Link\s+(0x[0-9a-fA-F]+)\s+\{(0x[0-9a-fA-F]+)\s+"([^"]+)"\}\}', patched)}
    bodies_linked = {body for body, _, _ in links}
    additions: list[str] = []
    for key in sorted(expected):
        hull = hulls[key]
        for seat, body in enumerate(pax[key], start=1):
            if body in bodies_linked:
                continue
            additions.append(f'\t{{Link {body} {{{hull} "seat{seat}"}}}}')
            bodies_linked.add(body)
    if additions:
        patched = patched.rstrip() + "\n\n; CANONICAL MOTOR PASSENGER LINKS: p1-p4, all factions\n" + "\n".join(additions) + "\n"
    return patched


def patch_trigger(text: str, engine: Engine, faction: str) -> str:
    marker = '{"' + engine.trigger_pattern.format(faction=faction) + '"'
    start, end, block = brace_block(text, marker)
    actions = block.find("{actions")
    if actions < 0:
        raise PatchError(f"{marker}: actions block missing")
    condition = block[:actions]
    old_available = f"ally_sup_{faction}_p1_hull"
    new_available = f"ally_sup_{faction}_motor_hull"
    if new_available not in condition:
        if condition.count(old_available) != 1:
            raise PatchError(f"{marker}: expected one p1 availability gate")
        condition = condition.replace(old_available, new_available, 1)
    block = condition + block[actions:]
    old_call = f'(\"{engine.placer_call}\")'
    new_call = f'(\"{engine.placer_macro}\")'
    if new_call not in block:
        if block.count(old_call) != 1:
            raise PatchError(f"{marker}: expected one generic placement call")
        block = block.replace(old_call, new_call, 1)
    return text[:start] + block + text[end:]


def patch_finisher(text: str, engine: Engine) -> str:
    marker = f'(define "{engine.finisher}"'
    start, end, block = paren_block(text, marker)
    if "; CANONICAL MOTOR RIDE: 60 seconds" not in block:
        matches = list(FIXED_WAIT.finditer(block))
        if len(matches) != 1:
            raise PatchError(f"{engine.relative_path}: expected one staged 28-second wait")
        match = matches[0]
        indent = match.group("indent")
        replacement = f'{indent}; CANONICAL MOTOR RIDE: 60 seconds before passenger-only emit.\n{indent}{{"delay" {{time 60}}}}'
        block = block[:match.start()] + replacement + block[match.end():]
    block = block.replace('waypoint "attack_support_entry_a1"', 'waypoint "attack_support_entry_a"')
    block = block.replace('waypoint "attack_support_entry_b1"', 'waypoint "attack_support_entry_b"')
    if '{"emit"' not in block or "{emit {mode passengers}}" not in block:
        raise PatchError(f"{engine.relative_path}: passenger-only emit missing")
    return text[:start] + block + text[end:]


def patch_clock(text: str, engine: Engine) -> str:
    marker = '{"' + engine.namespace + '/motor_clock"'
    start, end, block = brace_block(text, marker)
    command = block.find('{"set_i" {var "' + engine.namespace + '_wave_cmd$"}')
    if command < 0:
        raise PatchError(f"{marker}: wave command marker missing")
    prefix = block[:command]
    delays = list(re.finditer(r'\{"delay"\s+\{time\s+([0-9]+)\}\}', prefix))
    if len(delays) != 6:
        raise PatchError(f"{marker}: expected six schedule delays before dispatch, found {len(delays)}")
    values = (30, 30, 30, 180, 240, 300)
    for match, value in reversed(list(zip(delays, values))):
        prefix = prefix[:match.start()] + f'{{"delay" {{time {value}}}}}' + prefix[match.end():]
    block = prefix + block[command:]
    return text[:start] + block + text[end:]


def patch_cleanup(text: str, engine: Engine) -> str:
    marker = '{"' + engine.namespace + '/motor_cleanup"'
    start, end, block = brace_block(text, marker)
    old = '{"delay" {time 45}}'
    new = '{"delay" {time 90}}'
    if new not in block:
        if block.count(old) != 1:
            raise PatchError(f"{marker}: expected one 45-second cleanup delay")
        block = block.replace(old, new, 1)
    return text[:start] + block + text[end:]


def patch_engine(text: str, engine: Engine) -> str:
    for faction in FACTIONS:
        text = patch_trigger(text, engine, faction)
    text = upsert_define(text, engine.placer_macro, render_placer(engine), f'(define "{engine.finisher}"')
    text = patch_finisher(text, engine)
    text = patch_clock(text, engine)
    text = patch_cleanup(text, engine)
    return text


def validate_templates(text: str) -> None:
    for faction in FACTIONS:
        for package in range(1, 5):
            hull_match = re.search(rf'\{{Tags[^\n]*ally_sup_{faction}_p{package}_hull[^\n]*\s(0x[0-9a-fA-F]+)\}}', text)
            if not hull_match:
                raise PatchError(f"Missing {faction} p{package} hull")
            hull = hull_match.group(1).lower()
            pax_ids = [m.group(1).lower() for m in re.finditer(rf'\{{Tags[^\n]*ally_sup_{faction}_p{package}_pax[^\n]*\s(0x[0-9a-fA-F]+)\}}', text)]
            if not 4 <= len(pax_ids) <= 8:
                raise PatchError(
                    f"{faction} p{package}: expected a valid 4-8 passenger roster"
                )
            for seat, body in enumerate(sorted(pax_ids, key=lambda value: int(value, 16)), start=1):
                token = f'{{Link {body} {{{hull} "seat{seat}"}}}}'
                if token.lower() not in text.lower():
                    raise PatchError(f"{faction} p{package}: missing {token}")


def validate_engine(text: str, engine: Engine) -> None:
    placer = paren_block(text, f'(define "{engine.placer_macro}"')[2]
    for entry in (engine.side_one_entry, engine.side_two_entry, engine.default_entry):
        if f'target_waypoint "{entry}"' not in placer:
            raise PatchError(f"{engine.relative_path}: placer missing {entry}")
    for faction in FACTIONS:
        marker = '{"' + engine.trigger_pattern.format(faction=faction) + '"'
        block = brace_block(text, marker)[2]
        condition = block[:block.find("{actions")]
        if f"ally_sup_{faction}_motor_hull" not in condition:
            raise PatchError(f"{marker}: packages 2-4 not enabled")
        if f'(\"{engine.placer_macro}\")' not in block:
            raise PatchError(f"{marker}: canonical placer not used")
        if f'(\"{engine.placer_call}\")' in block:
            raise PatchError(f"{marker}: generic placer remains")
    finisher = paren_block(text, f'(define "{engine.finisher}"')[2]
    if finisher.count('{"delay" {time 60}}') != 1:
        raise PatchError(f"{engine.relative_path}: expected one 60-second ride")
    for stale in ('{"delay" {time 7}}', 'waypoint "attack_support_entry_a1"', 'waypoint "attack_support_entry_b1"'):
        if stale in finisher:
            raise PatchError(f"{engine.relative_path}: stale lifecycle token {stale}")
    clock = brace_block(text, '{"' + engine.namespace + '/motor_clock"')[2]
    command = clock.index('{"set_i" {var "' + engine.namespace + '_wave_cmd$"}')
    schedule = [int(v) for v in re.findall(r'\{"delay"\s+\{time\s+([0-9]+)\}\}', clock[:command])]
    if schedule != [30, 30, 30, 180, 240, 300]:
        raise PatchError(f"{engine.relative_path}: bad schedule {schedule}")
    cleanup = brace_block(text, '{"' + engine.namespace + '/motor_cleanup"')[2]
    if cleanup.count('{"delay" {time 90}}') != 1:
        raise PatchError(f"{engine.relative_path}: 90-second cleanup missing")
    if f"{{tag {engine.leaving_tag}}}" not in cleanup:
        raise PatchError(f"{engine.relative_path}: cleanup lost leaving tag")


def read_text(path: Path) -> tuple[str, bool]:
    raw = path.read_bytes()
    return raw.decode("utf-8-sig"), raw.startswith(b"\xef\xbb\xbf")


def write_text(path: Path, text: str, bom: bool) -> None:
    raw = text.encode("utf-8")
    if bom:
        raw = b"\xef\xbb\xbf" + raw
    path.write_bytes(raw)


def apply(root: Path, check_only: bool = False) -> list[Path]:
    changed: list[Path] = []
    template_path = root / "resource/map/multi/faction_support_templates.inc"
    template, bom = read_text(template_path)
    patched_template = patch_templates(template)
    validate_templates(patched_template)
    if patched_template != template:
        changed.append(template_path)
        if not check_only:
            write_text(template_path, patched_template, bom)
    for engine in ENGINES:
        path = root / engine.relative_path
        text, bom = read_text(path)
        patched = patch_engine(text, engine)
        validate_engine(patched, engine)
        if patched != text:
            changed.append(path)
            if not check_only:
                write_text(path, patched, bom)
    return changed


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    root = args.root.resolve()
    changed = apply(root, check_only=args.check)
    mode = "would patch" if args.check else "patched"
    for path in changed:
        print(f"{mode}: {path.relative_to(root)}")
    print("canonical motor contract: first 30s; recurring 180/240/300s; ride 60s; cleanup 90s")


if __name__ == "__main__":
    main()
