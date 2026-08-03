from __future__ import annotations

import unittest
from pathlib import Path
from tempfile import TemporaryDirectory

from gates_of_codex.bridge.archive import CampaignSaveArchive
from gates_of_codex.bridge.result import BattleResultImporter
from gates_of_codex.bridge.scn import CampaignScnBuilder, CampaignScnParser
from gates_of_codex.bridge.status import BattleStatusOptions, StatusBuilder, StatusResult
from gates_of_codex.campaign import CampaignEngine
from gates_of_codex.codex.catalog import CodeXCatalogScanner
from gates_of_codex.models import BattalionRosterEntry, Faction
from gates_of_codex.scenario import load_scenario


ROOT = Path(__file__).resolve().parents[1]
SCENARIO = ROOT / "data/four_faction_test.json"
CODEX = Path(__file__).resolve().parent / "fixtures/codex"


class BridgeTests(unittest.TestCase):
    def _pending(self):
        state = load_scenario(SCENARIO)
        state.battalions["nato-1"].province_id = "north_front"
        state.battalions["rusa-1"].faction = Faction.RUSSIA
        engine = CampaignEngine(state)
        engine.move_or_attack("nato-1", "rusa_north")
        return engine

    def test_status_uses_codex_armies_and_research(self) -> None:
        engine = self._pending()
        pending = engine.state.pending_battle
        assert pending is not None
        status = StatusBuilder().build(
            pending,
            BattleStatusOptions(
                map_string="multi/4x4/test_map",
                timestamp=100,
                seed=200,
                unlocked_research=["modern_nato_us", "modern_nato_us"],
            ),
        )
        self.assertIn("{army nato}", status)
        self.assertIn("{enemyArmy rusa}", status)
        self.assertIn('{"modern_nato_us"}', status)
        self.assertNotIn("%", status)

    def test_campaign_scn_materializes_and_validates(self) -> None:
        state = load_scenario(SCENARIO)
        state.battalions["nato-1"].province_id = "north_front"
        state.battalions["nato-1"].roster = [
            BattalionRosterEntry("squad_inf2_rifle(nato)", quantity=3, category="infantry"),
            BattalionRosterEntry("squad_tank1_m1a2_sep(nato)", quantity=1, category="tank"),
        ]
        state.battalions["rusa-1"].roster = [
            BattalionRosterEntry("squad_inf2_rifle(nato)", quantity=1, category="infantry")
        ]
        engine = CampaignEngine(state)
        engine.move_or_attack("nato-1", "rusa_north")
        pending = state.pending_battle
        assert pending is not None
        catalog = CodeXCatalogScanner().scan(CODEX)
        text = CampaignScnBuilder(catalog, CODEX).build(state, pending)
        squads = CampaignScnParser().parse_squads(text)
        self.assertEqual(5, len(squads))
        self.assertIn("{CampaignSquads", text)
        CampaignScnBuilder.validate(text)

    def test_save_round_trip_and_result_import(self) -> None:
        state = load_scenario(SCENARIO)
        state.battalions["nato-1"].province_id = "north_front"
        state.battalions["nato-1"].roster = [
            BattalionRosterEntry("squad_inf2_rifle(nato)", quantity=3, category="infantry"),
            BattalionRosterEntry("squad_tank1_m1a2_sep(nato)", quantity=1, category="tank"),
        ]
        state.battalions["rusa-1"].roster = [
            BattalionRosterEntry("squad_inf2_rifle(nato)", quantity=1, category="infantry")
        ]
        engine = CampaignEngine(state)
        engine.move_or_attack("nato-1", "rusa_north")
        pending = state.pending_battle
        assert pending is not None
        catalog = CodeXCatalogScanner().scan(CODEX)
        scn = CampaignScnBuilder(catalog, CODEX).build(state, pending)
        status = StatusBuilder().build(
            pending,
            BattleStatusOptions(
                map_string="multi/4x4/test_map",
                played_games=1,
                won_games=0,
                timestamp=100,
                seed=200,
            ),
        ).replace("{playedGames 1}", "{playedGames 2}").replace(
            "{wonGames 0}", "{wonGames 1}"
        )
        with TemporaryDirectory() as folder:
            save = Path(folder) / "campaign.sav"
            CampaignSaveArchive().write(save, status=status, campaign_scn=scn)
            result = BattleResultImporter().import_save(
                engine, save, previous_status=StatusResult(played_games=1, won_games=0)
            )
        self.assertTrue(result.player_won)
        self.assertEqual(Faction.NATO, result.winner)
        self.assertIsNone(state.pending_battle)
        self.assertEqual(Faction.NATO, state.provinces["rusa_north"].owner)


if __name__ == "__main__":
    unittest.main()
