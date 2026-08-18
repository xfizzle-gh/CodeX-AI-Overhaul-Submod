from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
CMD = ROOT / "resource/map/multi/ce/ce_command_cohesion_triggers.inc"
HUMAN = ROOT / "resource/set/interaction_entity/human_ce.inc"
DCG = ROOT / "resource/map/multi/dcg_script.inc"
LUA = ROOT / "resource/script/multiplayer/modes/utility_ce.lua"
TRIGGERS = ROOT / "resource/map/multi/ce/ce_triggers.inc"


class CeCommandCohesionTests(unittest.TestCase):
    def test_live_stack_loads_command(self) -> None:
        self.assertIn("ce_command_cohesion_triggers.inc", DCG.read_text(encoding="utf-8"))
        self.assertIn("ce_command_cohesion_triggers.inc", TRIGGERS.read_text(encoding="utf-8"))
        lua = LUA.read_text(encoding="utf-8")
        self.assertIn("cmd_link=", lua)
        self.assertIn("cmd_lost=", lua)
        self.assertIn("cmd_shock=", lua)
        self.assertIn("cmd_encourage=", lua)

    def test_link_lost_and_discipline_exist(self) -> None:
        text = CMD.read_text(encoding="utf-8")
        self.assertIn("mark_linked", text)
        self.assertIn("mark_weak", text)
        self.assertIn("mark_lost", text)
        self.assertIn("self_link", text)
        self.assertIn("discipline_aura", text)
        self.assertIn("casualty_shock", text)
        self.assertIn("{meters 50}", text)
        self.assertIn("{meters 80}", text)
        self.assertIn("{meters 25}", text)
        self.assertIn("{meters 30}", text)
        self.assertIn("aio_cmd_linked", text)
        self.assertIn("aio_cmd_weak", text)
        self.assertIn("aio_cmd_lost", text)
        self.assertIn("aio_discipline", text)

    def test_no_movement_seizure_or_surrender(self) -> None:
        text = CMD.read_text(encoding="utf-8")
        self.assertNotIn("ai_move", text)
        self.assertNotIn('{player "0"}', text)
        self.assertNotIn("{delete}", text)
        self.assertNotIn("aio_morale_surrendering", text)
        self.assertNotIn("advance_ratio", text)
        self.assertNotIn("retreat_ratio", text)

    def test_entity_handlers_exist(self) -> None:
        human = HUMAN.read_text(encoding="utf-8")
        self.assertIn('{on "aio_cmd_encouraged"', human)
        self.assertIn('{on "aio_cmd_shock"', human)
        self.assertIn('{tags remove "aio_morale_panic"}', human.split('{on "aio_cmd_encouraged"', 1)[1])


if __name__ == "__main__":
    unittest.main()
