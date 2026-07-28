from __future__ import annotations

import re
import unittest
from pathlib import Path

# Covers every CWA Dynamic Conquest mission currently owned by this repository.
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
MISSIONS = [
    ROOT / "resource" / "map" / "multi" / name / "campaign_capture_the_flag.mi"
    for name in MAP_NAMES
]
PROBE = ROOT / "resource/map/multi/allied_support_ownership_probe.inc"
POSITION_RE = re.compile(
    r"\{Position\s+(-?\d+(?:\.\d+)?)\s+(-?\d+(?:\.\d+)?)(?:\s+(-?\d+(?:\.\d+)?))?\}"
)
ENTRY_RE = re.compile(
    r'\{"allied_support_entry".*?\{position (-?\d+(?:\.\d+)?) (-?\d+(?:\.\d+)?) (-?\d+(?:\.\d+)?)\}',
    re.S,
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


def team_a_positions(text: str) -> list[tuple[float, float, float]]:
    result: list[tuple[float, float, float]] = []
    for block in conquest_blocks(text):
        if "{team a}" not in block:
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
        cls.missions = {
            path.parent.name: path.read_text(encoding="utf-8") for path in MISSIONS
        }
        cls.probe = PROBE.read_text(encoding="utf-8")

    def test_all_repository_owned_cwa_missions_are_covered(self) -> None:
        self.assertEqual(set(self.missions), set(MAP_NAMES))
        self.assertEqual(len(self.missions), 14)
        for map_name, mission in self.missions.items():
            with self.subTest(map_name=map_name):
                self.assertEqual(mission.count("allied_support_ownership_probe.inc"), 1)
                self.assertEqual(mission.count('{"allied_support_entry"'), 1)
                self.assertIn("{radius 150}", mission)

    def test_shared_include_precedes_dcg_logic(self) -> None:
        for map_name, mission in self.missions.items():
            with self.subTest(map_name=map_name):
                probe_index = mission.index("allied_support_ownership_probe.inc")
                if "dcg_script.inc" in mission:
                    self.assertLess(probe_index, mission.index("dcg_script.inc"))
                else:
                    self.assertEqual(map_name, "dcg_[cwa71]_border")
                    self.assertGreater(probe_index, mission.index("{triggers"))

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

    def test_support_test_is_hard_gated_and_repeats_every_minute(self) -> None:
        self.assertIn('{var "user_is_defender$"}', self.probe)
        self.assertIn('{var "id_defenderbot$"}', self.probe)
        self.assertIn('{var "prep_inform$"}', self.probe)
        self.assertIn('{expression "1 & 2 & 3"}', self.probe)
        self.assertIn('{time 60}', self.probe)
        self.assertEqual(self.probe.count('{"placement"'), 1)
        self.assertNotIn('{"loop"', self.probe)
        self.assertEqual(self.probe.count('{"trigger"'), 1)
        self.assertIn('{name "allied_support/test_cwa_one_minute_waves"}', self.probe)
        self.assertNotIn("probe_cwa_ownership", self.probe)
        self.assertNotIn("probe_woodland_ownership", self.probe)

    def test_probe_preserves_ai_ownership_contract(self) -> None:
        self.assertIn('{amount 5}', self.probe)
        self.assertIn('{target_waypoint "allied_support_entry"}', self.probe)
        self.assertIn('{tag fpc1}', self.probe)
        self.assertIn('{operation set}', self.probe)
        self.assertIn('{control AI}', self.probe)
        self.assertNotIn('{control user}', self.probe)
        self.assertIn('{tag_add _def}', self.probe)
        self.assertIn('{tag_add _ai_defender}', self.probe)
        self.assertNotIn('{tag_add _bot}', self.probe)
        for player_id in range(1, 17):
            self.assertIn("{value " + str(player_id) + "}", self.probe)
            self.assertIn('{player "' + str(player_id) + '"}', self.probe)

    def test_all_mission_delimiters_balance(self) -> None:
        for text in (*self.missions.values(), self.probe):
            self.assertEqual(text.count("{"), text.count("}"))
            self.assertEqual(text.count("("), text.count(")"))


if __name__ == "__main__":
    unittest.main()
