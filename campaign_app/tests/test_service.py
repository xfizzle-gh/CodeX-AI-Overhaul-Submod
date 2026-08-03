from __future__ import annotations

import unittest
from pathlib import Path
from tempfile import TemporaryDirectory

from gates_of_codex.bridge.archive import CampaignSaveArchive
from gates_of_codex.bridge.status import BattleStatusOptions
from gates_of_codex.campaign import CampaignEngine
from gates_of_codex.models import BattalionRosterEntry, Faction
from gates_of_codex.scenario import load_scenario
from gates_of_codex.service import GatesOfCodeXService, manifest_path_for
from gates_of_codex.state_io import load, save


ROOT = Path(__file__).resolve().parents[1]
SCENARIO = ROOT / "data/four_faction_test.json"
CODEX = Path(__file__).resolve().parent / "fixtures/codex"


class ServiceTests(unittest.TestCase):
    def test_export_and_import_persists_campaign(self) -> None:
        state = load_scenario(SCENARIO)
        state.battalions["nato-1"].province_id = "north_front"
        state.battalions["nato-1"].roster = [
            BattalionRosterEntry("squad_inf2_rifle(nato)", 2, category="infantry")
        ]
        state.battalions["rusa-1"].roster = [
            BattalionRosterEntry("squad_inf2_rifle(nato)", 1, category="infantry")
        ]
        CampaignEngine(state).move_or_attack("nato-1", "rusa_north")
        with TemporaryDirectory() as folder:
            state_path = Path(folder) / "state.json"
            save_path = Path(folder) / "campaign.sav"
            save(state, state_path)
            service = GatesOfCodeXService()
            manifest = service.export_pending_battle(
                state_path=state_path,
                code_x_directory=CODEX,
                save_path=save_path,
                options=BattleStatusOptions(
                    map_string="multi/test",
                    played_games=5,
                    won_games=2,
                    timestamp=1,
                    seed=2,
                ),
            )
            self.assertTrue(save_path.is_file())
            self.assertTrue(manifest_path_for(save_path).is_file())
            contents = CampaignSaveArchive().read(save_path)
            updated_status = contents.status.replace(
                "{playedGames 5}", "{playedGames 6}"
            ).replace("{wonGames 2}", "{wonGames 3}")
            CampaignSaveArchive().write(
                save_path,
                status=updated_status,
                campaign_scn=contents.campaign_scn,
            )
            result = service.import_completed_battle(
                state_path=state_path,
                save_path=save_path,
            )
            reloaded = load(state_path)
        self.assertEqual(manifest.battle_id, state.pending_battle.battle_id)
        self.assertTrue(result.player_won)
        self.assertIsNone(reloaded.pending_battle)
        self.assertEqual(Faction.NATO, reloaded.provinces["rusa_north"].owner)


if __name__ == "__main__":
    unittest.main()
