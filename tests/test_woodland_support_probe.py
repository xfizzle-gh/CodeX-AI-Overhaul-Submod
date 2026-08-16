from __future__ import annotations

import unittest
from pathlib import Path

# The old allied_support_ownership_probe.inc experiment was removed from current
# main after its ownership hypothesis was superseded. Keep this file as a CWA
# mission-integrity guard so stale probe references cannot silently return.
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
LEGACY_PROBE = ROOT / "resource/map/multi/allied_support_ownership_probe.inc"


class CwaMissionIntegrityTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.missions = {
            path.parent.name: path.read_text(encoding="utf-8") for path in MISSIONS
        }

    def test_all_repository_owned_cwa_missions_are_covered(self) -> None:
        self.assertEqual(set(self.missions), set(MAP_NAMES))
        self.assertEqual(len(self.missions), 14)

    def test_removed_ownership_probe_cannot_reappear(self) -> None:
        self.assertFalse(LEGACY_PROBE.exists())
        for map_name, mission in self.missions.items():
            with self.subTest(map_name=map_name):
                self.assertNotIn("allied_support_ownership_probe.inc", mission)
                self.assertNotIn("probe_cwa_ownership", mission)
                self.assertNotIn("probe_woodland_ownership", mission)

    def test_each_mission_keeps_both_conquest_spawn_sides(self) -> None:
        for map_name, mission in self.missions.items():
            with self.subTest(map_name=map_name):
                self.assertIn('Entity "map_point_conquest"', mission)
                self.assertIn("{team a}", mission)
                self.assertIn("{team b}", mission)

    def test_all_mission_delimiters_balance(self) -> None:
        for map_name, mission in self.missions.items():
            with self.subTest(map_name=map_name):
                self.assertEqual(mission.count("{"), mission.count("}"))
                self.assertEqual(mission.count("("), mission.count(")"))


if __name__ == "__main__":
    unittest.main()
