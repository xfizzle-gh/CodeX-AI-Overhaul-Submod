from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
CMD = ROOT / "resource/map/multi/ce/ce_command_cohesion_triggers.inc"
HUMAN = ROOT / "resource/set/interaction_entity/human_ce.inc"
MACHINE = ROOT / "resource/map/multi/ce/ce_morale_machine_triggers.inc"
DCG = ROOT / "resource/map/multi/dcg_script.inc"
LUA = ROOT / "resource/script/multiplayer/modes/utility_ce.lua"
TRIGGERS = ROOT / "resource/map/multi/ce/ce_triggers.inc"
WORKFLOW = ROOT / ".github/workflows/ce-morale-runtime-guard.yml"


class CeCommandCohesionTests(unittest.TestCase):
    def test_live_stack_loads_command(self) -> None:
        self.assertIn("ce_command_cohesion_triggers.inc", DCG.read_text(encoding="utf-8"))
        self.assertIn("ce_command_cohesion_triggers.inc", TRIGGERS.read_text(encoding="utf-8"))
        lua = LUA.read_text(encoding="utf-8")
        self.assertIn("cmd_link=", lua)
        self.assertIn("cmd_lost=", lua)
        self.assertIn("cmd_shock=", lua)
        self.assertIn("cmd_encourage=", lua)
        self.assertIn("vet_live=", lua)

    def test_link_is_heartbeat_not_permanent(self) -> None:
        text = CMD.read_text(encoding="utf-8")
        self.assertIn("pulse_near", text)
        self.assertIn("pulse_range", text)
        self.assertIn("recompute", text)
        self.assertIn("aio_cmd_seen", text)
        self.assertIn("aio_cmd_miss", text)
        self.assertIn("aio_cmd_in_range", text)
        self.assertNotIn("{mode far_than}", text)
        self.assertIn("{meters 50}", text)
        self.assertIn("{meters 80}", text)

    def test_shock_is_one_shot(self) -> None:
        text = CMD.read_text(encoding="utf-8")
        self.assertIn("aio_cmd_shock_spent", text)

    def test_discipline_does_not_skip_recovery(self) -> None:
        human = HUMAN.read_text(encoding="utf-8")
        enc = human.split('{on "aio_cmd_encouraged"', 1)[1].split('{on "', 1)[0]
        self.assertNotIn('{tags remove "aio_morale_panic"}', enc)
        self.assertNotIn('{tags remove "aio_morale_shaken"}', enc)
        machine = MACHINE.read_text(encoding="utf-8")
        self.assertIn("aio_cmd_encouraged", machine)
        self.assertIn("aio_steadfast", machine)

    def test_quality_tuning_exists(self) -> None:
        human = HUMAN.read_text(encoding="utf-8")
        self.assertIn('{delay 10', human)
        self.assertIn('{delay 14', human)
        self.assertIn('{delay 24', human)
        self.assertIn('{delay 3', human)
        self.assertIn("aio_morale_elite", human)
        self.assertIn("aio_morale_low", human)

    def test_no_movement_seizure_or_surrender(self) -> None:
        text = CMD.read_text(encoding="utf-8")
        self.assertNotIn("ai_move", text)
        self.assertNotIn('{player "0"}', text)
        self.assertNotIn("{delete}", text)
        self.assertNotIn("aio_morale_surrendering", text)
        self.assertNotIn("advance_ratio", text)
        self.assertNotIn("retreat_ratio", text)

    def test_ci_runs_command_suite(self) -> None:
        wf = WORKFLOW.read_text(encoding="utf-8")
        self.assertIn("tests/test_ce_command_cohesion.py", wf)


if __name__ == "__main__":
    unittest.main()
