"""Waypoint-name verification for the CWA campaign maps.

Waypoint names live in a {waypoints ...} block inside each map's
campaign_capture_the_flag.mi. A name collision crashes every map at load, so a new
name must be proven unused across all fourteen maps before injection.
"""
import pathlib
import re
import sys

MAP_NAMES = [
    "airbase", "border", "europe", "factory", "fields", "fulda", "grassland",
    "industrial", "monastery", "outback", "stasis", "train_station",
    "winds_valley", "woodland",
]

_WAYPOINTS_HEAD = re.compile(r"\{waypoints\b")
_ENTRY = re.compile(r'\{"([^"]+)"')


def _waypoints_block(text: str) -> str:
    """Return the source of the {waypoints ...} block, brace-balanced."""
    match = _WAYPOINTS_HEAD.search(text)
    if not match:
        return ""
    start = match.start()
    depth = 0
    for index in range(start, len(text)):
        char = text[index]
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return text[start:index + 1]
    return text[start:]


def waypoint_names(text: str) -> set[str]:
    """Every waypoint name declared in the map's waypoints block."""
    block = _waypoints_block(text)
    if not block:
        return set()
    # Skip the {waypoints head itself, then take every quoted entry key. Nested
    # keys inside {commands ...} are action names like "entity_state", so restrict
    # to entries at the block's first nesting level.
    names: set[str] = set()
    depth = 0
    index = 0
    while index < len(block):
        char = block[index]
        if char == "{":
            depth += 1
            if depth == 2:
                entry = _ENTRY.match(block, index)
                if entry:
                    names.add(entry.group(1))
        elif char == "}":
            depth -= 1
        index += 1
    return names


def map_paths(root: pathlib.Path) -> list[pathlib.Path]:
    return [
        root / "resource" / "map" / "multi" / f"dcg_[cwa71]_{name}"
        / "campaign_capture_the_flag.mi"
        for name in MAP_NAMES
    ]


def band_is_free(root: pathlib.Path, names: list[str]) -> dict[str, list[str]]:
    """Map each requested name to the map stems that already declare it."""
    occupied: dict[str, list[str]] = {name: [] for name in names}
    for path in map_paths(root):
        declared = waypoint_names(path.read_text(encoding="utf-8", errors="replace"))
        for name in names:
            if name in declared:
                occupied[name].append(path.parent.name)
    return occupied


def main(argv: list[str]) -> int:
    if len(argv) < 2:
        print("usage: verify_waypoint_band.py NAME [NAME ...]")
        return 2
    root = pathlib.Path(__file__).resolve().parent.parent
    result = band_is_free(root, argv[1:])
    failed = False
    for name, maps in sorted(result.items()):
        if maps:
            failed = True
            print(f"OCCUPIED {name}: {', '.join(maps)}")
        else:
            print(f"FREE     {name}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
