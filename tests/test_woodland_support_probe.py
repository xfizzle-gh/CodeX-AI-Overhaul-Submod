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


class CwaSupportDiagnosticProofTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.missions = {
            path.parent.name: path.read_text(encoding="utf-8") for path in MISSIONS
        }
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.templates = TEMPLATES.read_text(encoding="utf-8")

    def test_all_cwa_missions_load_support_and_have_waypoint_zero(self) -> None:
        self.assertEqual(set(self.missions), set(MAP_NAMES))
        self.assertEqual(len(self.missions), 14)
        for map_name, mission in self.missions.items():
            with self.subTest(map_name=map_name):
                self.assertEqual(mission.count("allied_support_waves.inc"), 1)
                self.assertEqual(mission.count('{"0"'), 1)

    def test_diagnostic_is_one_shot_and_hard_gated(self) -> None:
        self.assertIn('{"allied_support/diagnostic_once"', self.waves)
        self.assertIn('{var "user_is_defender$"}', self.waves)
        self.assertIn('{var "id_defenderbot$"}', self.waves)
        self.assertIn('{var "prep_inform$"}', self.waves)
        self.assertIn('{expression "1 & 2 & 3"}', self.waves)
        self.assertIn('{time 10}', self.waves)
        self.assertNotIn('{"trigger" {name "allied_support/diagnostic_once"}', self.waves)

    def test_source_and_clone_contract_is_minimal(self) -> None:
        for marker in (
            '{tag {tag cmp_def}}',
            '{prop {prop human}}',
            '{tag {tag hidden}}',
            '{zone {zone "gamezone"}}',
            '{state {state linked}}',
            '{state {state user_control}}',
            '{amount 1}',
            '{tag_add allied_support_diag_source}',
            '{target_waypoint "0"}',
            '{clone}',
        ):
            self.assertIn(marker, self.waves)
        self.assertEqual(self.waves.count('{"placement"'), 1)
        self.assertNotIn('target_waypoint "allied_support_entry"', self.waves)

    def test_promotion_and_orders_do_not_require_gamezone(self) -> None:
        self.assertIn(
            '{selector {tag allied_support_diag_source} {type human}}', self.waves
        )
        self.assertIn('{selector {tag allied_wave_fresh} {type human}}', self.waves)
        self.assertNotIn(
            '{selector {tag allied_support_diag_source} {zone "gamezone"}',
            self.waves,
        )
        self.assertNotIn(
            '{selector {tag allied_wave_fresh} {zone "gamezone"}', self.waves
        )
        self.assertIn('{tag_remove hidden}', self.waves)
        self.assertIn('{inactive off}', self.waves)
        self.assertIn('{control AI}', self.waves)
        self.assertNotIn('{control user}', self.waves)
        self.assertIn('{remove select}', self.waves)
        self.assertIn('{action advance}', self.waves)
        self.assertIn('{target {tag fpc1}}', self.waves)

    def test_diagnostics_cover_every_runtime_stage(self) -> None:
        for title in (
            "SUPPORT 1 ARMED",
            "SUPPORT 2 SOURCE CHECK RAN",
            "SUPPORT 2A SOURCE FOUND",
            "SUPPORT 3 CLONE ACTION RAN",
            "SUPPORT 3A CLONE FOUND",
            "SUPPORT 4 PROMOTE ACTION RAN",
            "SUPPORT 4A PROMOTED FOUND",
            "SUPPORT 5 OWNER ACTION RAN",
            "SUPPORT 5A OWNER FOUND",
            "SUPPORT 6 AI ORDER SENT",
        ):
            self.assertIn(f'{{title "{title}"}}', self.waves)
        self.assertIn('{count {op ">="} {value 1}}', self.waves)
        self.assertGreaterEqual(
            self.waves.count('{count {op ">="} {value 2}}'), 3
        )

    def test_defenderbot_ids_are_literal_and_common_owned_tag_is_added(self) -> None:
        self.assertIn('{operation set}', self.waves)
        self.assertIn('{tag_add allied_support_diag_owned}', self.waves)
        for player_id in range(1, 17):
            self.assertIn(f'{{value {player_id}}}', self.waves)
            self.assertIn(f'{{player "{player_id}"}}', self.waves)
            self.assertIn(f'allied_support_owner_{player_id}', self.waves)

    def test_dead_template_experiment_remains_disabled(self) -> None:
        self.assertNotIn('Human ""', self.templates)
        self.assertNotIn("allied_support_template", self.waves)

    def test_delimiters_balance(self) -> None:
        for text in (*self.missions.values(), self.waves, self.templates):
            self.assertEqual(text.count("{"), text.count("}"))
            self.assertEqual(text.count("("), text.count(")"))


if __name__ == "__main__":
    unittest.main()
