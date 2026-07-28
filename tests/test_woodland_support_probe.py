from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
WOODLAND = ROOT / "resource/map/multi/dcg_[cwa71]_woodland/campaign_capture_the_flag.mi"
PROBE = ROOT / "resource/map/multi/allied_support_ownership_probe.inc"


class WoodlandSupportOwnershipProbeTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.mission = WOODLAND.read_text(encoding="utf-8")
        cls.probe = PROBE.read_text(encoding="utf-8")

    def test_probe_is_woodland_only_and_loaded_before_shared_dcg_logic(self) -> None:
        self.assertEqual(self.mission.count("allied_support_ownership_probe.inc"), 1)
        self.assertLess(
            self.mission.index("allied_support_ownership_probe.inc"),
            self.mission.index("dcg_script.inc"),
        )
        cwa_root = ROOT / "resource/map/multi"
        other_hits = []
        for path in cwa_root.rglob("campaign_capture_the_flag.mi"):
            if "dcg_[cwa71]_" not in path.parent.name:
                continue
            if path != WOODLAND and "allied_support_ownership_probe.inc" in path.read_text(encoding="utf-8"):
                other_hits.append(path)
        self.assertEqual(other_hits, [])

    def test_woodland_has_explicit_rear_entry_waypoint(self) -> None:
        self.assertIn('{"allied_support_entry"', self.mission)
        self.assertIn("{position -6250.83 -572.56 53.84}", self.mission)
        self.assertIn("{radius 150}", self.mission)

    def test_probe_is_hard_gated_to_human_defense_after_prep(self) -> None:
        self.assertIn('{var "user_is_defender$"}', self.probe)
        self.assertIn('{var "id_defenderbot$"}', self.probe)
        self.assertIn('{var "prep_inform$"}', self.probe)
        self.assertIn('{expression "1 & 2 & 3"}', self.probe)
        self.assertIn('{time 60}', self.probe)

    def test_probe_clones_exactly_five_hidden_cmp_def_infantry(self) -> None:
        self.assertIn('{tag cmp_def}', self.probe)
        self.assertIn('{tag hidden}', self.probe)
        self.assertIn('{amount 5}', self.probe)
        self.assertIn('{target_waypoint "allied_support_entry"}', self.probe)
        self.assertIn('{clone}', self.probe)
        self.assertIn('{tag_add allied_support_probe_source}', self.probe)
        self.assertIn('{tag_remove allied_support_probe_source}', self.probe)

    def test_probe_covers_every_real_woodland_player_slot(self) -> None:
        self.assertIn('{count 17}', self.mission)
        for player_id in range(1, 17):
            self.assertIn(f'{{value {player_id}}}', self.probe)
            self.assertIn(f'{{player "{player_id}"}}', self.probe)
            self.assertIn(f'{{tag_add allied_support_probe_owner_{player_id}}}', self.probe)
        self.assertIn("allied_support_probe_owner_unsupported", self.probe)

    def test_units_remain_ai_owned_and_ce_defender_compatible(self) -> None:
        self.assertIn('{operation set}', self.probe)
        self.assertIn('{control AI}', self.probe)
        self.assertNotIn('{control user}', self.probe)
        self.assertIn('{tag_add _def}', self.probe)
        self.assertIn('{tag_add _ai_defender}', self.probe)
        self.assertIn('{tag_add allied_support_probe}', self.probe)
        self.assertNotIn('{tag_add _bot}', self.probe)
        self.assertNotIn('{tag _bot}', self.probe)

    def test_mission_and_probe_delimiters_are_balanced(self) -> None:
        for text in (self.mission, self.probe):
            self.assertEqual(text.count("{"), text.count("}"))
            self.assertEqual(text.count("("), text.count(")"))

    def test_clones_are_unhidden_before_runtime_selection(self) -> None:
        self.assertIn("{tag_remove hidden}", self.probe)
        promote = self.probe.index("{tag_add allied_wave_fresh}")
        unhide = self.probe.index("{tag_remove hidden}", promote)
        ownership = self.probe.index("{operation set}", unhide)
        self.assertLess(promote, unhide)
        self.assertLess(unhide, ownership)

    def test_probe_is_one_shot_not_the_final_wave_loop(self) -> None:
        self.assertEqual(self.probe.count('{"placement"'), 1)
        self.assertNotIn('4 * 60', self.probe)
        self.assertNotIn('8 * 60', self.probe)
        self.assertNotIn('{"trigger"', self.probe)
        self.assertNotIn('{"loop"', self.probe)


if __name__ == "__main__":
    unittest.main()
