from __future__ import annotations

import json
import unittest
from pathlib import Path
from tempfile import TemporaryDirectory

from gates_of_codex.campaign import CampaignEngine
from gates_of_codex.models import Faction
from gates_of_codex.scenario import load_scenario
from gates_of_codex.state_io import load, save


DATA = Path(__file__).resolve().parents[1] / "data" / "four_faction_test.json"


class CampaignTests(unittest.TestCase):
    def test_scenario_validates(self) -> None:
        state = load_scenario(DATA)
        state.validate()
        self.assertEqual(12, len(state.provinces))
        self.assertEqual(4, len(state.battalions))

    def test_neutral_capture(self) -> None:
        state = load_scenario(DATA)
        engine = CampaignEngine(state)
        result = engine.move_or_attack("nato-1", "center")
        self.assertTrue(result.moved)
        self.assertEqual(Faction.NATO, state.provinces["center"].owner)
        self.assertEqual("center", state.battalions["nato-1"].province_id)

    def test_enemy_move_creates_pending_battle(self) -> None:
        state = load_scenario(DATA)
        state.battalions["nato-1"].province_id = "north_front"
        engine = CampaignEngine(state)
        result = engine.move_or_attack("nato-1", "rusa_north")
        self.assertFalse(result.moved)
        self.assertIsNotNone(result.pending_battle)
        self.assertEqual(Faction.RUSSIA, result.pending_battle.defender_faction)

    def test_auto_resolve_clears_pending_battle(self) -> None:
        state = load_scenario(DATA)
        state.battalions["nato-1"].province_id = "north_front"
        engine = CampaignEngine(state, random_seed=3)
        engine.move_or_attack("nato-1", "rusa_north")
        winner = engine.auto_resolve_pending_battle()
        self.assertIn(winner, (Faction.NATO, Faction.RUSSIA))
        self.assertIsNone(state.pending_battle)

    def test_round_trip_is_stable(self) -> None:
        state = load_scenario(DATA)
        with TemporaryDirectory() as folder:
            path = Path(folder) / "campaign.json"
            save(state, path)
            reloaded = load(path)
            self.assertEqual(state.to_dict(), reloaded.to_dict())
            json.loads(path.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
