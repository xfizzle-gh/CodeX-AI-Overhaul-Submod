"""Idempotently inject allied-support birth pads and includes into the CWA maps.

Birth pads are numeric waypoints 31 (side a) and 32 (side b), placed at each map's
existing attack_support_rear_a1 / _b1 coordinates. Each pad's {commands} block tags
the ARRIVING actor - a bare entity_state with no selector - which is the only way to
mark a freshly cloned entity, whose provenance no selector in this format can express.

Radius 800 mirrors the proven waypoints 21/22 already in these maps.
"""
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

from verify_waypoint_band import map_paths, waypoint_names

BIRTH_PAD_A = "31"
BIRTH_PAD_B = "32"

_POSITION = r'\{position\s+(-?[\d.]+)\s+(-?[\d.]+)\s+(-?[\d.]+)\}'


def birth_waypoint_block(name: str, x: str, y: str, z: str, side: str) -> str:
    """Waypoint source for a birth pad. The commands block runs on the arriving actor."""
    return (
        f'\t\t\t{{"{name}"\n'
        f'\t\t\t\t{{position {x} {y} {z}}}\n'
        f'\t\t\t\t{{radius 800}}\n'
        f'\t\t\t\t{{commands\n'
        f'\t\t\t\t\t{{"entity_state"\n'
        f'\t\t\t\t\t\t{{tag_add allied_support_cmd_fresh}}\n'
        f'\t\t\t\t\t\t{{tag_add allied_support_cmd_side_{side}}}\n'
        f'\t\t\t\t\t}}\n'
        f'\t\t\t\t}}\n'
        f'\t\t\t}}\n'
    )


def _rear_pad_position(text: str, pad_name: str) -> tuple[str, str, str]:
    """Read an existing waypoint's position so the birth pad lands on the same spot."""
    anchor = text.find(f'{{"{pad_name}"')
    if anchor < 0:
        raise ValueError(f"map has no {pad_name} waypoint to anchor against")
    match = re.search(_POSITION, text[anchor:anchor + 400])
    if not match:
        raise ValueError(f"could not read position for {pad_name}")
    return match.group(1), match.group(2), match.group(3)


def patch_text(text: str) -> tuple[str, list[str]]:
    changes: list[str] = []
    existing = waypoint_names(text)

    for pad, anchor_name, side in (
        (BIRTH_PAD_A, "attack_support_rear_a1", "a"),
        (BIRTH_PAD_B, "attack_support_rear_b1", "b"),
    ):
        if pad in existing:
            continue
        x, y, z = _rear_pad_position(text, anchor_name)
        block = birth_waypoint_block(pad, x, y, z, side)
        # Insert immediately after the {waypoints line so ordering is deterministic.
        head = re.search(r"\{waypoints[^\n]*\n", text)
        if not head:
            raise ValueError("map has no waypoints block")
        text = text[:head.end()] + block + text[head.end():]
        changes.append(f"waypoint {pad}")

    for include in ("allied_support_birth.inc", "allied_support_handoff.inc"):
        line = f'(include "../{include}")'
        if line in text:
            continue
        anchor = '(include "../dcg_script.inc")'
        index = text.find(anchor)
        if index < 0:
            raise ValueError("map has no dcg_script.inc include to anchor against")
        line_start = text.rfind("\n", 0, index) + 1
        indent = text[line_start:index]
        text = text[:line_start] + f"{indent}{line}\n" + text[line_start:]
        changes.append(f"include {include}")

    return text, changes


def main() -> int:
    root = pathlib.Path(__file__).resolve().parent.parent
    total = 0
    for path in map_paths(root):
        original = path.read_text(encoding="utf-8", errors="replace")
        patched, changes = patch_text(original)
        if changes:
            path.write_text(patched, encoding="utf-8")
            total += 1
            print(f"PATCHED {path.parent.name}: {', '.join(changes)}")
        else:
            print(f"OK      {path.parent.name}")
    print(f"{total} map(s) changed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
