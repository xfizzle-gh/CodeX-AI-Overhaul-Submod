from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAP_NAMES = [
    "dcg_[cwa71]_airbase", "dcg_[cwa71]_border", "dcg_[cwa71]_europe",
    "dcg_[cwa71]_factory", "dcg_[cwa71]_fields", "dcg_[cwa71]_fulda",
    "dcg_[cwa71]_grassland", "dcg_[cwa71]_industrial", "dcg_[cwa71]_monastery",
    "dcg_[cwa71]_outback", "dcg_[cwa71]_stasis", "dcg_[cwa71]_train_station",
    "dcg_[cwa71]_winds_valley", "dcg_[cwa71]_woodland",
]
BREED = "mp/nato/2022s/inf2_rifleman"
TAG = "allied_support_explicit_template"
MISSIONS = [ROOT / "resource/map/multi" / name / "campaign_capture_the_flag.mi" for name in MAP_NAMES]
WAVES = ROOT / "resource/map/multi/allied_support_waves.inc"


class ExplicitCwaSupportProofTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.missions = {path.parent.name: path.read_text(encoding="utf-8") for path in MISSIONS}
        cls.waves = WAVES.read_text(encoding="utf-8")

    def test_all_fourteen_maps_embed_one_named_template(self) -> None:
        self.assertEqual(len(self.missions), 14)
        handles = []
        mids = []
        for name, mission in self.missions.items():
            with self.subTest(map=name):
                self.assertEqual(mission.count(f'Human "{BREED}"'), 1)
                self.assertEqual(mission.count(TAG), 1)
                self.assertEqual(mission.count("allied_support_waves.inc"), 1)
                match = re.search(rf'Human "{re.escape(BREED)}" (0x[0-9a-f]+).*?\{{Position -35000 -35000\}}.*?\{{Player 0\}}.*?\{{MID (\d+)\}}', mission, re.S)
                self.assertIsNotNone(match)
                handles.append(match.group(1))
                mids.append(match.group(2))
                self.assertIn(f'{{Tags "{TAG}" "not_delete" "hidden" {match.group(1)}}}', mission)
        self.assertEqual(len(set(handles)), 14)
        self.assertEqual(len(set(mids)), 14)

    def test_shared_trigger_uses_only_explicit_actor(self) -> None:
        for marker in (
            'allied_support/explicit_actor_once',
            'allied_support_clone_one',
            f'{{tag {{tag {TAG}}}}}',
            '{clone}',
            '{waypoint "allied_support_entry"}',
            '{approach "safe teleport & rotate"}',
            '{tag fpc1}',
            '{zone {zone "gamezone"}}',
            '{tag_add allied_wave_fresh}',
            '{tag_add allied_support_explicit_clone}',
            f'{{tag_remove {TAG}}}',
            '{tag_remove not_delete}',
            '{tag_remove hidden}',
            '{inactive off}',
            '{impregnability disabled}',
            '{discovered on}',
            '{control AI}',
            '{fire_mode open}',
            '{weapon_prepare on}',
            '{remove select}',
            '{action advance}',
            'fpc_inf_to_flag1',
            'PROOF 1 ARMED x12',
            'PROOF 4 COMBAT x12',
            'PROOF OWNER FAIL',
        ):
            self.assertIn(marker, self.waves)
        self.assertEqual(self.waves.count('("allied_support_clone_one")'), 12)
        self.assertNotIn('{waypoint "1"}', self.waves)
        self.assertNotIn('{target_waypoint "1"}', self.waves)
        self.assertNotIn('allied_support_template', self.waves.replace(TAG, ''))
        self.assertNotIn('allied_support_diag_source', self.waves)
        self.assertNotIn('{control user}', self.waves)
        self.assertNotIn('{"trigger" {name "allied_support/explicit_actor_once"}', self.waves)

    def test_explicit_templates_are_combat_shells(self) -> None:
        for name, mission in self.missions.items():
            with self.subTest(map=name):
                block = re.search(
                    rf'Human "{re.escape(BREED)}" 0x[0-9a-f]+\s*\{{.*?\n\t\}}',
                    mission,
                    re.S,
                )
                self.assertIsNotNone(block)
                body = block.group(0)
                self.assertIn('{Position -35000 -35000}', body)
                self.assertIn('{Player 0}', body)
                self.assertNotIn('{disabled}', body)
                self.assertNotIn('stand_noaim', body)
                self.assertNotIn('{Volume', body)

    def test_defenderbot_ownership_cases_cover_ids_1_to_16(self) -> None:
        for player_id in range(1, 17):
            self.assertIn(f'{{value {player_id}}}', self.waves)
            self.assertIn(f'{{player "{player_id}"}}', self.waves)
            self.assertIn(f'allied_support_owner_{player_id}', self.waves)

    def test_delimiters_balance(self) -> None:
        for text in (*self.missions.values(), self.waves):
            self.assertEqual(text.count("{"), text.count("}"))
            self.assertEqual(text.count("("), text.count(")"))


if __name__ == "__main__":
    unittest.main()
