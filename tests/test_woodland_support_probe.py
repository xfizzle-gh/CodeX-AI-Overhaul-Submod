from __future__ import annotations

import unittest
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
MISSIONS = [
    ROOT / "resource" / "map" / "multi" / name / "campaign_capture_the_flag.mi"
    for name in MAP_NAMES
]
WAVES = ROOT / "resource/map/multi/allied_support_waves.inc"
TEMPLATES = ROOT / "resource/map/multi/allied_support_templates.inc"


class CwaLiveSupportProofTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.missions = {
            path.parent.name: path.read_text(encoding="utf-8") for path in MISSIONS
        }
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.templates = TEMPLATES.read_text(encoding="utf-8")

    def test_all_cwa_missions_load_one_live_support_include_and_entry(self) -> None:
        self.assertEqual(set(self.missions), set(MAP_NAMES))
        self.assertEqual(len(self.missions), 14)
        for map_name, mission in self.missions.items():
            with self.subTest(map_name=map_name):
                self.assertEqual(mission.count("allied_support_waves.inc"), 1)
                self.assertEqual(mission.count('{"allied_support_entry"'), 1)
                self.assertIn("{radius 150}", mission)
                self.assertNotIn("allied_support_templates.inc", mission)

    def test_test_loop_is_hard_gated_and_repeats_every_minute(self) -> None:
        self.assertIn('{name "allied_support/live_clone_test"}', self.waves)
        self.assertIn('{var "user_is_defender$"}', self.waves)
        self.assertIn('{var "id_defenderbot$"}', self.waves)
        self.assertIn('{var "prep_inform$"}', self.waves)
        self.assertIn('{expression "1 & 2 & 3"}', self.waves)
        self.assertIn('{time 60}', self.waves)
        self.assertEqual(self.waves.count('{"placement"'), 1)
        self.assertEqual(self.waves.count('{"trigger"'), 1)

    def test_sources_are_real_equipped_live_defenders(self) -> None:
        self.assertIn('{tag {tag cmp_def}}', self.waves)
        self.assertIn('{prop {prop human}}', self.waves)
        self.assertIn('{state {state operatable}}', self.waves)
        self.assertIn('{zone {zone "gamezone"}}', self.waves)
        for fpc in range(1, 6):
            self.assertIn(f'{{zone {{zone "fpc{fpc}"}}}}', self.waves)
        self.assertIn('{amount 5}', self.waves)
        self.assertIn('{tag_add allied_support_source}', self.waves)
        self.assertIn('{target_waypoint "allied_support_entry"}', self.waves)
        self.assertIn('{clone}', self.waves)
        self.assertNotIn("allied_support_template", self.waves)
        self.assertNotIn('Human ""', self.waves)
        self.assertNotIn('Human ""', self.templates)

    def test_only_fresh_clones_receive_hidden_ai_ownership(self) -> None:
        self.assertIn('{tag_add allied_wave_fresh}', self.waves)
        self.assertIn('{tag_add allied_support_wave_issued}', self.waves)
        self.assertIn('{tag_add _def}', self.waves)
        self.assertIn('{tag_add _ai_defender}', self.waves)
        self.assertIn('{tag_remove hidden}', self.waves)
        self.assertIn('{control AI}', self.waves)
        self.assertNotIn('{control user}', self.waves)
        self.assertIn('{action advance}', self.waves)
        self.assertIn('{target {tag fpc1}}', self.waves)
        for player_id in range(1, 17):
            self.assertIn(f'{{value {player_id}}}', self.waves)
            self.assertIn(f'{{player "{player_id}"}}', self.waves)
            self.assertIn(f'allied_support_owner_{player_id}', self.waves)

    def test_old_ambiguous_fsm_is_removed(self) -> None:
        for marker in (
            "allied_support_initialized$",
            "allied_support_waves_left$",
            "allied_support_busy$",
            "near-cap defer",
            "live-clone fallback",
            "pool bootstrap",
        ):
            self.assertNotIn(marker, self.waves)

    def test_delimiters_balance(self) -> None:
        for text in (*self.missions.values(), self.waves, self.templates):
            self.assertEqual(text.count("{"), text.count("}"))
            self.assertEqual(text.count("("), text.count(")"))


if __name__ == "__main__":
    unittest.main()
