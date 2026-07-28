from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAP_NAMES = [
    "dcg_[cwa71]_airbase",
    "dcg_[cwa71]_border",
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
TRIGGERS_ANCHOR = '\t\t{triggers\n'
WAYPOINT_ANCHOR = '\t\t{waypoints\n\t\t\t{"0"\n'
POSITION_RE = re.compile(
    r"\{Position\s+(-?\d+(?:\.\d+)?)\s+(-?\d+(?:\.\d+)?)(?:\s+(-?\d+(?:\.\d+)?))?\}"
)


def conquest_blocks(text: str) -> list[str]:
    lines = text.splitlines(keepends=True)
    result: list[str] = []
    index = 0
    while index < len(lines):
        if not lines[index].startswith('\t{Entity "map_point_conquest"'):
            index += 1
            continue
        start = index
        depth = 0
        while index < len(lines):
            depth += lines[index].count("{") - lines[index].count("}")
            index += 1
            if depth == 0:
                break
        result.append("".join(lines[start:index]))
    return result


def team_a_positions(text: str, map_name: str) -> list[tuple[float, float, float]]:
    positions: list[tuple[float, float, float]] = []
    for block in conquest_blocks(text):
        if "{team a}" not in block:
            continue
        match = POSITION_RE.search(block)
        if not match:
            raise RuntimeError(f"{map_name}: Team-A spawn lacks Position")
        positions.append(
            (
                float(match.group(1)),
                float(match.group(2)),
                float(match.group(3) or 0.0),
            )
        )
    if not positions:
        raise RuntimeError(f"{map_name}: no Team-A conquest spawns")
    return positions


def centroid(points: list[tuple[float, float, float]]) -> tuple[float, float, float]:
    count = float(len(points))
    return tuple(sum(point[axis] for point in points) / count for axis in range(3))


def remove_named_waypoint(text: str, waypoint_name: str, map_name: str) -> str:
    marker = f'\t\t\t{{"{waypoint_name}"\n'
    occurrences = text.count(marker)
    if occurrences == 0:
        return text
    if occurrences != 1:
        raise RuntimeError(
            f"{map_name}: expected at most one {waypoint_name} waypoint, found {occurrences}"
        )
    start = text.index(marker)
    depth = 0
    for index in range(start, len(text)):
        if text[index] == "{":
            depth += 1
        elif text[index] == "}":
            depth -= 1
            if depth == 0:
                end = index + 1
                if end < len(text) and text[end] == "\n":
                    end += 1
                return text[:start] + text[end:]
    raise RuntimeError(f"{map_name}: unterminated {waypoint_name} waypoint")


def insert_probe_include(text: str, map_name: str) -> str:
    if text.count(PROBE_INCLUDE) == 1:
        return text
    if text.count(PROBE_INCLUDE) > 1:
        raise RuntimeError(f"{map_name}: duplicate ownership-probe includes")

    dcg_count = text.count(DCG_INCLUDE)
    if dcg_count == 1:
        return text.replace(DCG_INCLUDE, PROBE_INCLUDE + DCG_INCLUDE, 1)
    if dcg_count > 1:
        raise RuntimeError(f"{map_name}: duplicate dcg_script includes")

    # Border embeds its DCG trigger body rather than including dcg_script.inc.
    if map_name != "dcg_[cwa71]_border":
        raise RuntimeError(f"{map_name}: no verified probe insertion anchor")
    if text.count(TRIGGERS_ANCHOR) != 1:
        raise RuntimeError(f"{map_name}: expected one embedded triggers block")
    return text.replace(TRIGGERS_ANCHOR, TRIGGERS_ANCHOR + PROBE_INCLUDE, 1)


def patch_mission(path: Path) -> None:
    map_name = path.parent.name
    text = path.read_text(encoding="utf-8")
    text = insert_probe_include(text, map_name)
    text = remove_named_waypoint(text, "allied_support_entry", map_name)

    points = team_a_positions(text, map_name)
    x, y, z = centroid(points)
    if text.count(WAYPOINT_ANCHOR) != 1:
        raise RuntimeError(f"{map_name}: expected one waypoint-0 anchor")
    entry = (
        '\t\t{waypoints\n'
        '\t\t\t{"allied_support_entry"\n'
        f'\t\t\t\t{{position {x:.2f} {y:.2f} {z:.2f}}}\n'
        '\t\t\t\t{radius 150}\n'
        '\t\t\t}\n'
        '\t\t\t{"0"\n'
    )
    text = text.replace(WAYPOINT_ANCHOR, entry, 1)

    if text.count(PROBE_INCLUDE) != 1:
        raise RuntimeError(f"{map_name}: probe include count is not one")
    if text.count('{"allied_support_entry"') != 1:
        raise RuntimeError(f"{map_name}: support-entry count is not one")
    if text.count("{") != text.count("}"):
        raise RuntimeError(f"{map_name}: braces are unbalanced")
    if text.count("(") != text.count(")"):
        raise RuntimeError(f"{map_name}: include delimiters are unbalanced")
    path.write_text(text, encoding="utf-8")


missions: list[Path] = []
for map_name in MAP_NAMES:
    mission = (
        ROOT
        / "resource"
        / "map"
        / "multi"
        / map_name
        / "campaign_capture_the_flag.mi"
    )
    if not mission.is_file():
        raise RuntimeError(f"missing repository-owned CWA mission: {mission}")
    patch_mission(mission)
    missions.append(mission)

probe_path = ROOT / "resource/map/multi/allied_support_ownership_probe.inc"
probe = probe_path.read_text(encoding="utf-8")
probe = probe.replace(
    "; Woodland-only ownership proof for engine-created DefenderBot.",
    "; CWA Dynamic Conquest ownership proof for the engine-created DefenderBot.",
    1,
)
probe = probe.replace(
    '{"allied_support/probe_woodland_ownership"',
    '{"allied_support/probe_cwa_ownership"',
    1,
)
if "probe_woodland_ownership" in probe:
    raise RuntimeError("stale woodland-only trigger name remains")
probe_path.write_text(probe, encoding="utf-8")

map_rows = "\n".join(f'    "{name}",' for name in MAP_NAMES)
test_path = ROOT / "tests/test_woodland_support_probe.py"
test_path.write_text(
    f'''from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAP_NAMES = [
{map_rows}
]
MISSIONS = [
    ROOT / "resource" / "map" / "multi" / name / "campaign_capture_the_flag.mi"
    for name in MAP_NAMES
]
PROBE = ROOT / "resource/map/multi/allied_support_ownership_probe.inc"
POSITION_RE = re.compile(
    r"\\{{Position\\s+(-?\\d+(?:\\.\\d+)?)\\s+(-?\\d+(?:\\.\\d+)?)(?:\\s+(-?\\d+(?:\\.\\d+)?))?\\}}"
)
ENTRY_RE = re.compile(
    r'\\{{"allied_support_entry".*?\\{{position (-?\\d+(?:\\.\\d+)?) (-?\\d+(?:\\.\\d+)?) (-?\\d+(?:\\.\\d+)?)\\}}',
    re.S,
)


def conquest_blocks(text: str) -> list[str]:
    lines = text.splitlines(keepends=True)
    result: list[str] = []
    index = 0
    while index < len(lines):
        if not lines[index].startswith('\\t{{Entity "map_point_conquest"'):
            index += 1
            continue
        start = index
        depth = 0
        while index < len(lines):
            depth += lines[index].count("{{") - lines[index].count("}}")
            index += 1
            if depth == 0:
                break
        result.append("".join(lines[start:index]))
    return result


def team_a_positions(text: str) -> list[tuple[float, float, float]]:
    result: list[tuple[float, float, float]] = []
    for block in conquest_blocks(text):
        if "{{team a}}" not in block:
            continue
        match = POSITION_RE.search(block)
        if match:
            result.append(
                (
                    float(match.group(1)),
                    float(match.group(2)),
                    float(match.group(3) or 0.0),
                )
            )
    return result


class CwaSupportOwnershipProbeTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.missions = {{
            path.parent.name: path.read_text(encoding="utf-8") for path in MISSIONS
        }}
        cls.probe = PROBE.read_text(encoding="utf-8")

    def test_all_repository_owned_cwa_missions_are_covered(self) -> None:
        self.assertEqual(set(self.missions), set(MAP_NAMES))
        self.assertEqual(len(self.missions), 14)
        for map_name, mission in self.missions.items():
            with self.subTest(map_name=map_name):
                self.assertEqual(mission.count("allied_support_ownership_probe.inc"), 1)
                self.assertEqual(mission.count('{{"allied_support_entry"'), 1)
                self.assertIn("{{radius 150}}", mission)

    def test_shared_include_precedes_dcg_logic(self) -> None:
        for map_name, mission in self.missions.items():
            with self.subTest(map_name=map_name):
                probe_index = mission.index("allied_support_ownership_probe.inc")
                if "dcg_script.inc" in mission:
                    self.assertLess(probe_index, mission.index("dcg_script.inc"))
                else:
                    self.assertEqual(map_name, "dcg_[cwa71]_border")
                    self.assertGreater(probe_index, mission.index("{{triggers"))

    def test_each_entry_matches_its_team_a_spawn_centroid(self) -> None:
        for map_name, mission in self.missions.items():
            with self.subTest(map_name=map_name):
                points = team_a_positions(mission)
                self.assertGreater(len(points), 0)
                expected = tuple(
                    sum(point[axis] for point in points) / len(points)
                    for axis in range(3)
                )
                match = ENTRY_RE.search(mission)
                self.assertIsNotNone(match)
                actual = tuple(float(match.group(index)) for index in range(1, 4))
                for observed, wanted in zip(actual, expected):
                    self.assertAlmostEqual(observed, wanted, places=1)

    def test_probe_remains_hard_gated_and_one_shot(self) -> None:
        self.assertIn('{{var "user_is_defender$"}}', self.probe)
        self.assertIn('{{var "id_defenderbot$"}}', self.probe)
        self.assertIn('{{var "prep_inform$"}}', self.probe)
        self.assertIn('{{expression "1 & 2 & 3"}}', self.probe)
        self.assertIn('{{time 60}}', self.probe)
        self.assertEqual(self.probe.count('{{"placement"'), 1)
        self.assertNotIn('{{"loop"', self.probe)
        self.assertIn("probe_cwa_ownership", self.probe)
        self.assertNotIn("probe_woodland_ownership", self.probe)

    def test_probe_preserves_ai_ownership_contract(self) -> None:
        self.assertIn('{{amount 5}}', self.probe)
        self.assertIn('{{operation set}}', self.probe)
        self.assertIn('{{control AI}}', self.probe)
        self.assertNotIn('{{control user}}', self.probe)
        self.assertIn('{{tag_add _def}}', self.probe)
        self.assertIn('{{tag_add _ai_defender}}', self.probe)
        self.assertNotIn('{{tag_add _bot}}', self.probe)
        for player_id in range(1, 17):
            self.assertIn("{{value " + str(player_id) + "}}", self.probe)
            self.assertIn('{{player "' + str(player_id) + '"}}', self.probe)

    def test_all_mission_delimiters_balance(self) -> None:
        for text in (*self.missions.values(), self.probe):
            self.assertEqual(text.count("{{"), text.count("}}"))
            self.assertEqual(text.count("("), text.count(")"))


if __name__ == "__main__":
    unittest.main()
''',
    encoding="utf-8",
)
