#!/usr/bin/env python3
"""Apply the motorized arrival-gated unload overlay to an installed mod tree.

The production motor package remains unchanged: real linked passengers ride in,
explicit passenger emit unloads them, infantry advances, and the empty truck
withdraws. This test overlay changes only the fixed 28-second drive wait.

New behavior:
- poll truck-to-objective distance every five seconds;
- unload as soon as the existing distance band reports <= 150 m;
- keep polling while the truck is farther away;
- force the unload after 60 seconds so pathing cannot imprison the squad.

The script is idempotent and intentionally targets a deployed Workshop tree.
"""

from __future__ import annotations

import argparse
import re
from dataclasses import dataclass
from pathlib import Path

MARKER = "; MOTOR ARRIVAL-GATED RELEASE OVERLAY: <=150m, 60s fallback"
POLL_SECONDS = 5
MAX_POLLS = 12


@dataclass(frozen=True)
class Engine:
    relative_path: str
    finisher: str


ENGINES = (
    Engine("resource/map/multi/attack_support_waves.inc", "as_finish_motor"),
    Engine("resource/map/multi/defense_support_waves.inc", "ds_finish_motor"),
    Engine("resource/map/multi/enemy_attack_support.inc", "ea_finish_motor"),
    Engine("resource/map/multi/enemy_defense_support.inc", "ed_finish_motor"),
)

FIXED_WAIT = re.compile(
    r'(?P<indent>^[\t ]*)\{"delay"\s+\{time\s+7\}\}\s*\r?\n'
    r'(?P=indent)\{"set_i"\s+\{var\s+"(?P<drive>[^"\r\n]*motor_drive_t)\$"\}\s+\{op\s+"="\}\s+\{value\s+1\}\}\s*\r?\n'
    r'(?P=indent)\{"delay"\s+\{time\s+7\}\}\s*\r?\n'
    r'(?P=indent)\{"set_i"\s+\{var\s+"(?P=drive)\$"\}\s+\{op\s+"="\}\s+\{value\s+2\}\}\s*\r?\n'
    r'(?P=indent)\{"delay"\s+\{time\s+7\}\}\s*\r?\n'
    r'(?P=indent)\{"set_i"\s+\{var\s+"(?P=drive)\$"\}\s+\{op\s+"="\}\s+\{value\s+3\}\}\s*\r?\n'
    r'(?P=indent)\{"delay"\s+\{time\s+7\}\}\s*\r?\n'
    r'(?P=indent)\{"set_i"\s+\{var\s+"(?P=drive)\$"\}\s+\{op\s+"="\}\s+\{value\s+4\}\}\s*\r?\n'
    r'(?P=indent)\("(?P<helper>[^"\r\n]*motor_band)"\)(?=\s*\r?\n(?P=indent)\{"emit")',
    re.MULTILINE,
)


def balanced_define(text: str, name: str) -> str:
    token = f'(define "{name}"'
    start = text.index(token)
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
        elif char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
            if depth == 0:
                return text[start : index + 1]
    raise RuntimeError(f"Unbalanced define: {name}")


def find_band_variable(text: str, helper: str) -> str:
    body = balanced_define(text, helper)
    match = re.search(r'\{var\s+"([^"\r\n]*motor_band)\$"\}', body)
    if not match:
        raise RuntimeError(f"Could not resolve distance-band variable from {helper}")
    return match.group(1)


def render_poll(indent: str, newline: str, drive: str, helper: str, band: str, poll: int) -> str:
    lines = [
        f'{indent}{{"delay" {{time {POLL_SECONDS}}}}}',
        f'{indent}{{"set_i" {{var "{drive}$"}} {{op "="}} {{value {poll}}}}}',
        f'{indent}("{helper}")',
    ]
    if poll == MAX_POLLS:
        lines.append(f'{indent}; 60-second fallback reached: unload regardless of pathing result.')
        return newline.join(lines)

    child = render_poll(indent + "\t\t", newline, drive, helper, band, poll + 1)
    lines.extend(
        [
            f'{indent}{{"switch"',
            f'{indent}\t{{"case"',
            f'{indent}\t\t{{condition {{type cmp_i}} {{var "{band}$"}} {{op "=="}} {{value 1}}}}',
            f'{indent}\t\t{{"set_i" {{var "{drive}$"}} {{op "="}} {{value {poll}}}}}',
            f'{indent}\t}}',
            f'{indent}\t{{"case"',
            f'{indent}\t\t{{condition {{type cmp_i}} {{var "{band}$"}} {{op "=="}} {{value 2}}}}',
            f'{indent}\t\t{{"set_i" {{var "{drive}$"}} {{op "="}} {{value {poll}}}}}',
            f'{indent}\t}}',
            f'{indent}\t{{"default"',
            child,
            f'{indent}\t}}',
            f'{indent}}}',
        ]
    )
    return newline.join(lines)


def patch_text(text: str, engine: Engine) -> str:
    finisher = balanced_define(text, engine.finisher)
    if MARKER in finisher:
        return text

    matches = list(FIXED_WAIT.finditer(finisher))
    if len(matches) != 1:
        raise RuntimeError(
            f"{engine.relative_path}: expected one fixed 28-second motor wait in "
            f"{engine.finisher}, found {len(matches)}"
        )

    match = matches[0]
    helper = match.group("helper")
    drive = match.group("drive")
    band = find_band_variable(text, helper)
    newline = "\r\n" if "\r\n" in text else "\n"
    indent = match.group("indent")
    replacement = newline.join(
        [
            f"{indent}{MARKER}",
            f"{indent}; Bands 1/2 mean the truck is within 60/150 metres of an active flag.",
            f"{indent}; Band 3 or 0 keeps the truck moving until the next poll or timeout.",
            render_poll(indent, newline, drive, helper, band, 1),
        ]
    )

    patched_finisher = finisher[: match.start()] + replacement + finisher[match.end() :]
    return text.replace(finisher, patched_finisher, 1)


def read_preserving_bom(path: Path) -> tuple[str, bool]:
    raw = path.read_bytes()
    had_bom = raw.startswith(b"\xef\xbb\xbf")
    return raw.decode("utf-8-sig"), had_bom


def write_preserving_bom(path: Path, text: str, had_bom: bool) -> None:
    data = text.encode("utf-8")
    if had_bom:
        data = b"\xef\xbb\xbf" + data
    path.write_bytes(data)


def apply(root: Path, check_only: bool = False) -> list[Path]:
    changed: list[Path] = []
    for engine in ENGINES:
        path = root / engine.relative_path
        if not path.is_file():
            raise FileNotFoundError(f"Missing motor engine: {path}")
        text, had_bom = read_preserving_bom(path)
        patched = patch_text(text, engine)
        if MARKER not in balanced_define(patched, engine.finisher):
            raise RuntimeError(f"{engine.relative_path}: overlay marker was not installed")
        if patched != text:
            changed.append(path)
            if not check_only:
                write_preserving_bom(path, patched, had_bom)
    return changed


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True, help="Installed mod root containing resource/")
    parser.add_argument("--check", action="store_true", help="Validate applicability without writing")
    args = parser.parse_args()

    root = args.root.resolve()
    changed = apply(root, check_only=args.check)
    mode = "would patch" if args.check else "patched"
    if changed:
        for path in changed:
            print(f"{mode}: {path.relative_to(root)}")
    else:
        print("motor arrival-release overlay already present")
    print("motor release contract: unload inside 150 m; hard fallback at 60 s")


if __name__ == "__main__":
    main()
