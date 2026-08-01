#!/usr/bin/env python3
"""Generate five safe transport patrol waypoints around campaign flags.

A truck never targets the flag entity or sandbag post directly. Each waypoint is
centred 320 map units (about 32 metres) from its source flag and has a 140-unit
arrival radius. The closest possible requested destination is therefore about 18
metres from the flag centre.

Maps contain two to five campaign flags. Five route slots are always generated;
when a map has fewer flags, later slots revisit those flags from rotated perimeter
angles so every script can use one fixed five-step route.
"""
from __future__ import annotations

import argparse
import math
import re
from dataclasses import dataclass
from pathlib import Path


class PatchError(RuntimeError):
    pass


WAYPOINT_PREFIX = "transport_patrol_flag_"
WAYPOINT_COUNT = 5
OFFSET = 320.0
RADIUS = 140
CLOSEST_TO_FLAG = OFFSET - RADIUS
MAP_DIR_PATTERN = re.compile(r"^dcg_\[cwa71\]_")


@dataclass(frozen=True)
class Flag:
    x: float
    y: float
    z: float


def balanced_end(text: str, start: int, opener: str, closer: str, label: str) -> int:
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
    raise PatchError(f"Unbalanced block: {label}")


def extract_flags(text: str, label: str) -> list[Flag]:
    flags: list[Flag] = []
    pattern = re.compile(r'\{Entity\s+"flag_point_campaign_\d+"\s+0x[0-9a-fA-F]+')
    for match in pattern.finditer(text):
        end = balanced_end(text, match.start(), "{", "}", label)
        block = text[match.start() : end]
        position = re.search(
            r'\{Position\s+(-?\d+(?:\.\d+)?)\s+(-?\d+(?:\.\d+)?)(?:\s+(-?\d+(?:\.\d+)?))?\s*\}',
            block,
            flags=re.IGNORECASE,
        )
        if not position:
            raise PatchError(f"Campaign flag has no readable Position in {label}")
        flags.append(
            Flag(
                float(position.group(1)),
                float(position.group(2)),
                float(position.group(3) or 0.0),
            )
        )
    if not 2 <= len(flags) <= 5:
        raise PatchError(f"Expected 2-5 campaign flags in {label}, found {len(flags)}")
    return flags


def waypoint_block(name: str, x: float, y: float, z: float, newline: str) -> str:
    return (
        f'{newline}\t\t\t{{"{name}"{newline}'
        f'\t\t\t\t{{position {x:.2f} {y:.2f} {z:.2f}}}{newline}'
        f'\t\t\t\t{{radius {RADIUS}}}{newline}'
        f'\t\t\t}}'
    )


def remove_named_waypoint(text: str, name: str) -> str:
    marker = f'{{"{name}"'
    start = text.find(marker)
    if start < 0:
        return text
    block_start = start
    while block_start > 0 and text[block_start - 1] in "\t ":
        block_start -= 1
    if block_start > 0 and text[block_start - 1] == "\n":
        block_start -= 1
        if block_start > 0 and text[block_start - 1] == "\r":
            block_start -= 1
    end = balanced_end(text, start, "{", "}", name)
    while end < len(text) and text[end] in "\r\n":
        end += 1
    return text[:block_start] + text[end:]


def route_points(flags: list[Flag]) -> list[Flag]:
    points: list[Flag] = []
    repeats: dict[int, int] = {}
    for slot in range(WAYPOINT_COUNT):
        flag_index = slot % len(flags)
        flag = flags[flag_index]
        repeat = repeats.get(flag_index, 0)
        repeats[flag_index] = repeat + 1

        # Prefer the map-centre-facing side of the flag, which keeps the truck in
        # playable ground. Repeated route slots rotate around that flag to avoid
        # producing duplicate destinations on two- and three-flag maps.
        length = math.hypot(flag.x, flag.y)
        if length >= 1.0:
            base_angle = math.atan2(-flag.y, -flag.x)
        else:
            base_angle = (2.0 * math.pi * flag_index) / max(1, len(flags))
        rotation = repeat * math.radians(70.0)
        angle = base_angle + rotation
        points.append(
            Flag(
                flag.x + OFFSET * math.cos(angle),
                flag.y + OFFSET * math.sin(angle),
                flag.z,
            )
        )
    return points


def patch_text(text: str, label: str) -> str:
    flags = extract_flags(text, label)
    for slot in range(1, WAYPOINT_COUNT + 1):
        text = remove_named_waypoint(text, f"{WAYPOINT_PREFIX}{slot}")

    anchor_match = re.search(r'\{waypoints(?:\r?\n|\s)', text)
    if not anchor_match:
        raise PatchError(f"Map is missing the waypoints anchor: {label}")
    newline = "\r\n" if "\r\n" in text else "\n"
    anchor_end = anchor_match.end()
    # If the regex consumed the newline, insert immediately before it so generated
    # blocks begin on the first line inside the waypoints container.
    if text[anchor_end - 1 : anchor_end] == "\n":
        anchor_end -= 2 if text[anchor_end - 2 : anchor_end] == "\r\n" else 1

    blocks = "".join(
        waypoint_block(f"{WAYPOINT_PREFIX}{slot}", point.x, point.y, point.z, newline)
        for slot, point in enumerate(route_points(flags), start=1)
    )
    return text[:anchor_end] + blocks + text[anchor_end:]


def parse_waypoints(text: str, label: str) -> list[Flag]:
    points: list[Flag] = []
    for slot in range(1, WAYPOINT_COUNT + 1):
        name = f"{WAYPOINT_PREFIX}{slot}"
        pattern = re.compile(
            r'\{"'
            + re.escape(name)
            + r'"\s*\{position\s+(-?\d+(?:\.\d+)?)\s+(-?\d+(?:\.\d+)?)\s+(-?\d+(?:\.\d+)?)\s*\}\s*\{radius\s+(\d+)\s*\}\s*\}',
            flags=re.IGNORECASE,
        )
        matches = list(pattern.finditer(text))
        if len(matches) != 1:
            raise PatchError(f"Expected one {name} in {label}, found {len(matches)}")
        match = matches[0]
        if int(match.group(4)) != RADIUS:
            raise PatchError(f"{name} has the wrong radius in {label}")
        points.append(Flag(float(match.group(1)), float(match.group(2)), float(match.group(3))))
    return points


def validate_text(text: str, label: str) -> None:
    flags = extract_flags(text, label)
    points = parse_waypoints(text, label)
    expected = route_points(flags)
    for slot, (actual, target) in enumerate(zip(points, expected), start=1):
        if math.hypot(actual.x - target.x, actual.y - target.y) > 0.1:
            raise PatchError(f"Waypoint {slot} is not at its generated perimeter point in {label}")
        source = flags[(slot - 1) % len(flags)]
        centre_distance = math.hypot(actual.x - source.x, actual.y - source.y)
        if centre_distance < OFFSET - 0.1:
            raise PatchError(f"Waypoint {slot} is too close to its flag in {label}")
        if centre_distance - RADIUS < CLOSEST_TO_FLAG - 0.1:
            raise PatchError(f"Waypoint {slot} arrival radius can touch the flag in {label}")


def map_files(root: Path) -> list[Path]:
    multi = root / "resource/map/multi"
    files = [
        directory / "campaign_capture_the_flag.mi"
        for directory in multi.iterdir()
        if directory.is_dir() and MAP_DIR_PATTERN.match(directory.name)
        and (directory / "campaign_capture_the_flag.mi").is_file()
    ]
    files.sort()
    if len(files) != 14:
        raise PatchError(f"Expected 14 CWA campaign maps, found {len(files)}")
    return files


def apply(root: Path, *, check_only: bool = False) -> list[Path]:
    changed: list[Path] = []
    results: list[tuple[Path, str, bool]] = []
    for path in map_files(root):
        raw = path.read_bytes()
        bom = raw.startswith(b"\xef\xbb\xbf")
        text = raw.decode("utf-8-sig")
        patched = patch_text(text, str(path))
        results.append((path, patched, bom))
        if patched != text:
            changed.append(path)

    if not check_only:
        for path, patched, bom in results:
            raw = patched.encode("utf-8")
            path.write_bytes((b"\xef\xbb\xbf" if bom else b"") + raw)
            validate_text(patched, str(path))
    return changed


def validate(root: Path) -> None:
    for path in map_files(root):
        validate_text(path.read_text(encoding="utf-8-sig"), str(path))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if args.check:
        validate(args.root)
        print("Transport flag-perimeter waypoints validated on all 14 maps.")
    else:
        changed = apply(args.root)
        print(f"Transport flag-perimeter waypoints patched {len(changed)} map(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
