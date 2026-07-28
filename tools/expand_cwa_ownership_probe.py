from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAP_NAMES = [
    "dcg_[cwa71]_airbase",
    "dcg_[cwa71]_border",
    "dcg_[cwa71]_briges",
    "dcg_[cwa71]_crossroads",
    "dcg_[cwa71]_europe",
    "dcg_[cwa71]_factory",
    "dcg_[cwa71]_fields",
    "dcg_[cwa71]_fulda",
    "dcg_[cwa71]_grassland",
    "dcg_[cwa71]_industrial",
    "dcg_[cwa71]_monastery",
    "dcg_[cwa71]_outback",
    "dcg_[cwa71]_stasis",
    "dcg_[cwa71]_train_station",
    "dcg_[cwa71]_winds_valley",
    "dcg_[cwa71]_woodland",
]
PROBE_INCLUDE = '\t\t\t(include "../allied_support_ownership_probe.inc")\n'
DCG_INCLUDE = '\t\t\t(include "../dcg_script.inc")\n'


def top_level_entity_blocks(text: str) -> list[str]:
    lines = text.splitlines(keepends=True)
    blocks: list[str] = []
    i = 0
    while i < len(lines):
        if not lines[i].startswith('\t{Entity "map_point_conquest"'):
            i += 1
            continue
        start = i
        depth = 0
        while i < len(lines):
            depth += lines[i].count("{") - lines[i].count("}")
            i += 1
            if depth == 0:
                break
        blocks.append("".join(lines[start:i]))
    return blocks


def team_a_entry(text: str, map_name: str) -> tuple[float, float, float]:
    positions: list[tuple[float, float, float]] = []
    for block in top_level_entity_blocks(text):
        if "{team a}" not in block:
            continue
        match = re.search(
            r"\{Position\s+(-?\d+(?:\.\d+)?)\s+(-?\d+(?:\.\d+)?)(?:\s+(-?\d+(?:\.\d+)?))?\}",
            block,
        )
        if not match:
            raise RuntimeError(f"{map_name}: Team-A spawn lacks Position")
        x = float(match.group(1))
        y = float(match.group(2))
        z = float(match.group(3) or 0.0)
        positions.append((x, y, z))
    if not positions:
        raise RuntimeError(f"{map_name}: no Team-A conquest spawn points found")
    count = float(len(positions))
    return (
        sum(p[0] for p in positions) / count,
        sum(p[1] for p in positions) / count,
        sum(p[2] for p in positions) / count,
    )


def remove_existing_entry(text: str, map_name: str) -> str:
    marker = '\t\t\t{"allied_support_entry"\n'
    count = text.count(marker)
    if count == 0:
        return text
    if count != 1:
        raise RuntimeError(f"{map_name}: expected at most one support entry, found {count}")
    start = text.index(marker)
    i = start
    depth = 0
    while i < len(text):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                i += 1
                if i < len(text) and text[i] == "\n":
                    i += 1
                return text[:start] + text[i:]
        i += 1
    raise RuntimeError(f"{map_name}: unterminated support-entry waypoint")


def patch_mission(path: Path) -> None:
    map_name = path.parent.name
    text = path.read_text(encoding="utf-8")
    original = text

    if text.count(DCG_INCLUDE) != 1:
        raise RuntimeError(f"{map_name}: expected exactly one dcg_script include")
    if text.count(PROBE_INCLUDE) == 0:
        text = text.replace(DCG_INCLUDE, PROBE_INCLUDE + DCG_INCLUDE, 1)
    elif text.count(PROBE_INCLUDE) != 1:
        raise RuntimeError(f"{map_name}: duplicate ownership-probe includes")

    text = remove_existing_entry(text, map_name)
    x, y, z = team_a_entry(text, map_name)
    waypoint_anchor = '\t\t{waypoints\n\t\t\t{"0"\n'
    if text.count(waypoint_anchor) != 1:
        raise RuntimeError(f"{map_name}: expected one waypoint-0 anchor")
    entry = (
        '\t\t{waypoints\n'
        '\t\t\t{"allied_support_entry"\n'
        f'\t\t\t\t{{position {x:.2f} {y:.2f} {z:.2f}}}\n'
        '\t\t\t\t{radius 150}\n'
        '\t\t\t}\n'
        '\t\t\t{"0"\n'
    )
    text = text.replace(waypoint_anchor, entry, 1)

    if text.count("{") != text.count("}"):
        raise RuntimeError(f"{map_name}: brace balance changed")
    if text.count(PROBE_INCLUDE) != 1:
        raise RuntimeError(f"{map_name}: ownership probe not loaded exactly once")
    if text.count('{"allied_support_entry"') != 1:
        raise RuntimeError(f"{map_name}: support entry not defined exactly once")
    if text == original and map_name != "dcg_[cwa71]_woodland":
        raise RuntimeError(f"{map_name}: patch unexpectedly made no change")
    path.write_text(text, encoding="utf-8")


for map_name in MAP_NAMES:
    mission = ROOT / "resource" / "map" / "multi" / map_name / "campaign_capture_the_flag.mi"
    if not mission.is_file():
        raise RuntimeError(f"missing CWA mission: {mission}")
    patch_mission(mission)

TEST = ROOT / "tests" / "test_woodland_support_probe.py"
test_text = TEST.read_text(encoding="utf-8")
test_text = re.sub(
    r'MAP_NAMES = \[.*?\]\nPROBE =',
    'MAP_NAMES = [\n' + ''.join(f'    "{name}",\n' for name in MAP_NAMES) + ']\nPROBE =',
    test_text,
    flags=re.S,
) if "MAP_NAMES = [" in test_text else test_text.replace(
    'WOODLAND = ROOT / "resource/map/multi/dcg_[cwa71]_woodland/campaign_capture_the_flag.mi"\nPROBE =',
    'MAP_NAMES = [\n' + ''.join(f'    "{name}",\n' for name in MAP_NAMES) + ']\nMISSIONS = [ROOT / "resource" / "map" / "multi" / name / "campaign_capture_the_flag.mi" for name in MAP_NAMES]\nWOODLAND = ROOT / "resource/map/multi/dcg_[cwa71]_woodland/campaign_capture_the_flag.mi"\nPROBE =',
)
old_setup = '''        cls.mission = WOODLAND.read_text(encoding="utf-8")
        cls.probe = PROBE.read_text(encoding="utf-8")
'''
new_setup = '''        cls.missions = {path.parent.name: path.read_text(encoding="utf-8") for path in MISSIONS}
        cls.mission = cls.missions["dcg_[cwa71]_woodland"]
        cls.probe = PROBE.read_text(encoding="utf-8")
'''
if old_setup not in test_text:
    raise RuntimeError("test setup marker not found")
test_text = test_text.replace(old_setup, new_setup, 1)
start = test_text.index("    def test_probe_is_woodland_only_and_loaded_before_shared_dcg_logic")
end = test_text.index("\n    def test_woodland_has_explicit_rear_entry_waypoint", start)
replacement = '''    def test_probe_is_loaded_by_every_cwa_conquest_mission(self) -> None:
        self.assertEqual(len(self.missions), 16)
        for map_name, mission in self.missions.items():
            with self.subTest(map_name=map_name):
                self.assertEqual(mission.count("allied_support_ownership_probe.inc"), 1)
                self.assertLess(
                    mission.index("allied_support_ownership_probe.inc"),
                    mission.index("dcg_script.inc"),
                )
                self.assertEqual(mission.count('{"allied_support_entry"'), 1)
                self.assertIn("{radius 150}", mission)
'''
test_text = test_text[:start] + replacement + test_text[end:]
start = test_text.index("    def test_woodland_has_explicit_rear_entry_waypoint")
end = test_text.index("\n    def test_probe_is_hard_gated_to_human_defense_after_prep", start)
replacement = '''    def test_each_entry_is_centered_on_existing_team_a_conquest_spawns(self) -> None:
        position_re = re.compile(r'\\{"allied_support_entry".*?\\{position (-?\\d+(?:\\.\\d+)?) (-?\\d+(?:\\.\\d+)?) (-?\\d+(?:\\.\\d+)?)\\}', re.S)
        for map_name, mission in self.missions.items():
            with self.subTest(map_name=map_name):
                match = position_re.search(mission)
                self.assertIsNotNone(match)
                self.assertEqual(len(top_level_team_a_positions(mission)), 4)
                expected = centroid(top_level_team_a_positions(mission))
                actual = tuple(float(match.group(i)) for i in range(1, 4))
                for lhs, rhs in zip(actual, expected):
                    self.assertAlmostEqual(lhs, rhs, places=1)
'''
test_text = test_text[:start] + replacement + test_text[end:]
if "import re\n" not in test_text:
    test_text = test_text.replace("import unittest\n", "import re\nimport unittest\n", 1)
helpers = '''\n\ndef top_level_team_a_positions(text: str) -> list[tuple[float, float, float]]:
    lines = text.splitlines(keepends=True)
    positions: list[tuple[float, float, float]] = []
    i = 0
    while i < len(lines):
        if not lines[i].startswith('\\t{Entity "map_point_conquest"'):
            i += 1
            continue
        start = i
        depth = 0
        while i < len(lines):
            depth += lines[i].count("{") - lines[i].count("}")
            i += 1
            if depth == 0:
                break
        block = "".join(lines[start:i])
        if "{team a}" not in block:
            continue
        match = re.search(r"\\{Position\\s+(-?\\d+(?:\\.\\d+)?)\\s+(-?\\d+(?:\\.\\d+)?)(?:\\s+(-?\\d+(?:\\.\\d+)?))?\\}", block)
        if match:
            positions.append((float(match.group(1)), float(match.group(2)), float(match.group(3) or 0.0)))
    return positions


def centroid(points: list[tuple[float, float, float]]) -> tuple[float, float, float]:
    count = float(len(points))
    return tuple(sum(point[index] for point in points) / count for index in range(3))
'''
insert_at = test_text.index("\n\nclass WoodlandSupportOwnershipProbeTests")
test_text = test_text[:insert_at] + helpers + test_text[insert_at:]
test_text = test_text.replace(
    '        for text in (self.mission, self.probe):\n',
    '        for text in (*self.missions.values(), self.probe):\n',
    1,
)
TEST.write_text(test_text, encoding="utf-8")
