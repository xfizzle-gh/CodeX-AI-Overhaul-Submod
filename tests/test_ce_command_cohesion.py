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

    def test_lost_to_weak_clears_lost(self) -> None:
        text = CMD.read_text(encoding="utf-8")
        weak = text.split("{tag_add aio_cmd_weak}", 1)[1]
        self.assertIn("{tag_remove aio_cmd_lost}", weak.split("{tag_add aio_cmd_lost}", 1)[0])

    def test_discipline_does_not_skip_recovery(self) -> None:
        human = HUMAN.read_text(encoding="utf-8")
        enc = human.split('{on "aio_cmd_encouraged"', 1)[1].split('{on "', 1)[0]
        self.assertNotIn('{tags remove "aio_morale_panic"}', enc)
        self.assertNotIn('{tags remove "aio_morale_shaken"}', enc)
        machine = MACHINE.read_text(encoding="utf-8")
        entry = machine.split("shaken_entry", 1)[1].split("escalate_panic", 1)[0]
        self.assertNotIn("aio_steadfast", entry)
        self.assertNotIn("aio_cmd_encouraged", entry)
        escalate = machine.split("escalate_panic", 1)[1].split("start_recover", 1)[0]
        self.assertIn("aio_cmd_encouraged", escalate)
        self.assertNotIn("aio_steadfast", escalate)

    def test_command_changes_recovery_time(self) -> None:
        human = HUMAN.read_text(encoding="utf-8")
        shaken = human.split('{on "recovering_from_shaken"', 1)[1].split('{on "', 1)[0]
        self.assertIn("aio_cmd_encouraged", shaken)
        self.assertIn("aio_cmd_linked", shaken)
        self.assertIn("aio_cmd_lost", shaken)
        self.assertIn("aio_steadfast", shaken)
        self.assertIn("{delay 16", shaken)
        self.assertIn("{delay 28", shaken)
        hold = human.split('{on "aio_morale_just_shaken"', 1)[1].split('{on "', 1)[0]
        self.assertIn("aio_cmd_shock", hold)
        self.assertIn("{delay 2", hold)

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

    def test_icon_refresh_is_transition_only(self) -> None:
        text = CMD.read_text(encoding="utf-8")
        self_link = text.split("command/self_link", 1)[1].split("command/pulse_near", 1)[0]
        pulse = text.split("command/pulse_near", 1)[1].split("command/pulse_range", 1)[0]
        for block in (self_link, pulse):
            self.assertIn("{effect aio_morale_refresh_icons}", block)
            effect = block.split("{effect aio_morale_refresh_icons}", 1)[0]
            effect = effect[effect.rfind('{"effect"'):]
            self.assertIn("aio_icon_refresh", effect)
            self.assertNotIn("{tag aio_cmd_linked}", effect)
            before = block.split("{effect aio_morale_refresh_icons}", 1)[0]
            self.assertIn("{tag_remove aio_cmd_lost}", before)
            self.assertLess(before.find("{tag_remove aio_cmd_lost}"), before.rfind('{"effect"'))
        lost_add = text.split("{tag_add aio_cmd_lost}", 1)[1]
        lost_fx = lost_add.split("{effect aio_morale_refresh_icons}", 1)[0]
        lost_fx = lost_fx[lost_fx.rfind('{"effect"'):]
        self.assertIn("aio_icon_refresh", lost_fx)
        self.assertNotIn("{tag aio_cmd_lost}", lost_fx)


if __name__ == "__main__":
    unittest.main()
