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
WAYPOINT_ANCHOR = '\t\t{waypoints\n\t\t\t{"0"\n'


def conquest_blocks(text: str) -> list[str]:
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


def team_a_centroid(text: str, name: str) -> tuple[float, float, float]:
    positions: list[tuple[float, float, float]] = []
    pattern = re.compile(r"\{Position\s+(-?\d+(?:\.\d+)?)\s+(-?\d+(?:\.\d+)?)(?:\s+(-?\d+(?:\.\d+)?))?\}")
    for block in conquest_blocks(text):
        if "{team a}" not in block:
            continue
        match = pattern.search(block)
        if not match:
            raise RuntimeError(f"{name}: Team-A spawn has no Position")
        positions.append((float(match.group(1)), float(match.group(2)), float(match.group(3) or 0)))
    if not positions:
        raise RuntimeError(f"{name}: no Team-A conquest spawns")
    n = float(len(positions))
    return tuple(sum(point[index] for point in positions) / n for index in range(3))


def remove_named_waypoint(text: str, name: str) -> str:
    marker = f'\t\t\t{{"{name}"\n'
    if marker not in text:
        return text
    if text.count(marker) != 1:
        raise RuntimeError(f"duplicate waypoint: {name}")
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
    raise RuntimeError(f"unterminated waypoint: {name}")


def patch(path: Path) -> None:
    name = path.parent.name
    text = path.read_text(encoding="utf-8")
    if text.count(DCG_INCLUDE) != 1:
        raise RuntimeError(f"{name}: expected one dcg_script include")
    if PROBE_INCLUDE not in text:
        text = text.replace(DCG_INCLUDE, PROBE_INCLUDE + DCG_INCLUDE, 1)
    if text.count(PROBE_INCLUDE) != 1:
        raise RuntimeError(f"{name}: expected one probe include")
    text = remove_named_waypoint(text, "allied_support_entry")
    x, y, z = team_a_centroid(text, name)
    if text.count(WAYPOINT_ANCHOR) != 1:
        raise RuntimeError(f"{name}: expected one waypoint-0 anchor")
    entry = (
        '\t\t{waypoints\n'
        '\t\t\t{"allied_support_entry"\n'
        f'\t\t\t\t{{position {x:.2f} {y:.2f} {z:.2f}}}\n'
        '\t\t\t\t{radius 150}\n'
        '\t\t\t}\n'
        '\t\t\t{"0"\n'
    )
    text = text.replace(WAYPOINT_ANCHOR, entry, 1)
    if text.count("{") != text.count("}"):
        raise RuntimeError(f"{name}: unbalanced braces")
    path.write_text(text, encoding="utf-8")


for map_name in MAP_NAMES:
    mission = ROOT / "resource" / "map" / "multi" / map_name / "campaign_capture_the_flag.mi"
    if not mission.is_file():
        raise RuntimeError(f"missing mission: {mission}")
    patch(mission)

maps_literal = "\n".join(f'    "{name}",' for name in MAP_NAMES)
test_content = f'''from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAP_NAMES = [
{maps_literal}
]
MISSIONS = [ROOT / "resource" / "map" / "multi" / name / "campaign_capture_the_flag.mi" for name in MAP_NAMES]
PROBE = ROOT / "resource/map/multi/allied_support_ownership_probe.inc"


def team_a_positions(text: str) -> list[tuple[float, float, float]]:
    lines = text.splitlines(keepends=True)
    result: list[tuple[float, float, float]] = []
    pattern = re.compile(r"\\{{Position\\s+(-?\\d+(?:\\.\\d+)?)\\s+(-?\\d+(?:\\.\\d+)?)(?:\\s+(-?\\d+(?:\\.\\d+)?))?\\}}")
    i = 0
    while i < len(lines):
        if not lines[i].startswith('\\t{{Entity "map_point_conquest"'):
            i += 1
            continue
        start = i
        depth = 0
        while i < len(lines):
            depth += lines[i].count("{{") - lines[i].count("}}")
            i += 1
            if depth == 0:
                break
        block = "".join(lines[start:i])
        if "{{team a}}" not in block:
            continue
        match = pattern.search(block)
        if match:
            result.append((float(match.group(1)), float(match.group(2)), float(match.group(3) or 0)))
    return result


class CwaSupportOwnershipProbeTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.missions = {{path.parent.name: path.read_text(encoding="utf-8") for path in MISSIONS}}
        cls.probe = PROBE.read_text(encoding="utf-8")

    def test_every_cwa_mission_loads_the_shared_probe(self) -> None:
        self.assertEqual(len(self.missions), 16)
        for name, mission in self.missions.items():
            with self.subTest(name=name):
                self.assertEqual(mission.count("allied_support_ownership_probe.inc"), 1)
                self.assertLess(mission.index("allied_support_ownership_probe.inc"), mission.index("dcg_script.inc"))
                self.assertEqual(mission.count('{{"allied_support_entry"'), 1)

    def test_each_entry_matches_its_team_a_spawn_centroid(self) -> None:
        entry_re = re.compile(r'\\{{"allied_support_entry".*?\\{{position (-?\\d+(?:\\.\\d+)?) (-?\\d+(?:\\.\\d+)?) (-?\\d+(?:\\.\\d+)?)\\}}', re.S)
        for name, mission in self.missions.items():
            with self.subTest(name=name):
                points = team_a_positions(mission)
                self.assertGreater(len(points), 0)
                expected = tuple(sum(point[i] for point in points) / len(points) for i in range(3))
                match = entry_re.search(mission)
                self.assertIsNotNone(match)
                actual = tuple(float(match.group(i)) for i in range(1, 4))
                for lhs, rhs in zip(actual, expected):
                    self.assertAlmostEqual(lhs, rhs, places=1)

    def test_probe_is_hard_gated_and_one_shot(self) -> None:
        self.assertIn('{{var "user_is_defender$"}}', self.probe)
        self.assertIn('{{var "id_defenderbot$"}}', self.probe)
        self.assertIn('{{var "prep_inform$"}}', self.probe)
        self.assertIn('{{time 60}}', self.probe)
        self.assertEqual(self.probe.count('{{"placement"'), 1)
        self.assertNotIn('{{"loop"', self.probe)

    def test_probe_preserves_ai_ownership_contract(self) -> None:
        self.assertIn('{{operation set}}', self.probe)
        self.assertIn('{{control AI}}', self.probe)
        self.assertNotIn('{{control user}}', self.probe)
        self.assertIn('{{tag_add _def}}', self.probe)
        self.assertIn('{{tag_add _ai_defender}}', self.probe)
        self.assertNotIn('{{tag_add _bot}}', self.probe)
        for player_id in range(1, 17):
            self.assertIn(f'{{{{value {{player_id}}}}}', self.probe)
            self.assertIn(f'{{{{player "{{player_id}}"}}}}', self.probe)

    def test_all_mission_delimiters_balance(self) -> None:
        for text in (*self.missions.values(), self.probe):
            self.assertEqual(text.count("{{"), text.count("}}"))
            self.assertEqual(text.count("("), text.count(")"))


if __name__ == "__main__":
    unittest.main()
'''
(ROOT / "tests/test_woodland_support_probe.py").write_text(test_content, encoding="utf-8")
