#!/usr/bin/env python3
"""Insert transport_patrol_flag_1-5 on overlaid vanilla/DLC CTF maps.

CWA maps already have these. Support trucks, arty, smoke, and para scripts
move to them. Positions come from flag_point_campaign_N, then waypoints 1-5.
"""
from __future__ import annotations

import argparse
import re
from pathlib import Path

FLAG_HEAD = re.compile(
    r'\{Entity "flag_point_campaign_(\d+)"(?P<body>.*?)(?=\n\t\{(?:Entity|Human|Vehicle)|'
    r'\n\t\{Link|\n\t\(include)',
    re.S,
)
POS_RE = re.compile(r"\{Position\s+(-?[\d.]+)\s+(-?[\d.]+)(?:\s+(-?[\d.]+))?\}")
WP_NUM_RE = re.compile(
    r'\{\s*"([1-5])"\s*\n\s*\{position\s+(-?[\d.]+)\s+(-?[\d.]+)(?:\s+(-?[\d.]+))?\}',
    re.I,
)
AIR_B2_RE = re.compile(
    r'(\{\s*"attack_support_air_b2"\s*\n\s*\{position[^\n]+\}\s*\n\s*\{radius[^\n]+\}\s*\n\s*\})',
    re.I,
)


def flag_positions(text: str) -> dict[int, tuple[float, float]]:
    found: dict[int, tuple[float, float]] = {}
    for m in FLAG_HEAD.finditer(text):
        n = int(m.group(1))
        pm = POS_RE.search(m.group("body"))
        if not pm:
            continue
        found[n] = (float(pm.group(1)), float(pm.group(2)))
    return found


def wp_positions(text: str) -> dict[int, tuple[float, float]]:
    found: dict[int, tuple[float, float]] = {}
    for m in WP_NUM_RE.finditer(text):
        found[int(m.group(1))] = (float(m.group(2)), float(m.group(3)))
    return found


def five_points(text: str) -> list[tuple[float, float]] | None:
    flags = flag_positions(text)
    wps = wp_positions(text)
    pts: list[tuple[float, float]] = []
    for i in range(1, 6):
        if i in flags:
            pts.append(flags[i])
        elif i in wps:
            pts.append(wps[i])
    if len(pts) < 2:
        return None
    while len(pts) < 5:
        pts.append(pts[len(pts) % max(1, len(flags) or len(wps))])
    return pts[:5]


def block_for(pts: list[tuple[float, float]]) -> str:
    lines = []
    for i, (x, y) in enumerate(pts, 1):
        lines.append(f'\t\t\t{{"transport_patrol_flag_{i}"')
        lines.append(f"\t\t\t\t{{position {x:.2f} {y:.2f} 0.00}}")
        lines.append("\t\t\t\t{radius 140}")
        lines.append("\t\t\t}")
    return "\n".join(lines) + "\n"


def patch_file(path: Path) -> str:
    text = path.read_text(encoding="utf-8", errors="replace")
    if "attack_support_entry_b1" not in text:
        return "skip-no-overlay"
    if "transport_patrol_flag_1" in text:
        return "skip-has-wp"
    pts = five_points(text)
    if not pts:
        return "skip-no-flags"
    m = AIR_B2_RE.search(text)
    if not m:
        return "skip-no-anchor"
    insert_at = m.end()
    nl = "\r\n" if "\r\n" in text else "\n"
    updated = text[:insert_at] + nl + block_for(pts).replace("\n", nl) + text[insert_at:]
    path.write_bytes(updated.encode("utf-8"))
    return "patched"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("roots", nargs="+", type=Path)
    args = parser.parse_args()
    counts: dict[str, int] = {}
    for root in args.roots:
        multi = root / "resource" / "map" / "multi"
        if not multi.is_dir():
            multi = root
        for mi in sorted(multi.glob("*/campaign_capture_the_flag.mi")):
            status = patch_file(mi)
            counts[status] = counts.get(status, 0) + 1
            if status == "patched":
                print(f"patched {mi.parent.name}")
            elif status.startswith("skip-no"):
                print(f"{status} {mi.parent.name}")
    print("summary", counts)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
