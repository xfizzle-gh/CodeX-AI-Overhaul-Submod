from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MACHINE = ROOT / "resource/map/multi/ce/ce_morale_machine_triggers.inc"
DCG = ROOT / "resource/map/multi/dcg_script.inc"
LUA = ROOT / "resource/script/multiplayer/modes/utility_ce.lua"
MOD = ROOT / "resource/map/multi/ce/morale_system.mod"


class CeMoraleMachineTests(unittest.TestCase):
    def test_live_stack_loads_machine(self) -> None:
        self.assertIn("ce_morale_machine_triggers.inc", DCG.read_text(encoding="utf-8"))
        lua = LUA.read_text(encoding="utf-8")
        self.assertIn("CE_MORALE_SYS", lua)
        self.assertIn("ai=", lua)
        self.assertIn("broken=", lua)
        self.assertIn("retreat=", lua)
        self.assertIn("surrender=", lua)
        self.assertIn("AI_ABSENT", lua)

    def test_machine_is_ai_only_and_surrender_is_last(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        self.assertIn("pressure_see_enemy", text)
        self.assertIn("escalate_panic", text)
        self.assertIn("escalate_broken", text)
        self.assertIn("broken_retreat", text)
        self.assertIn("retreat_to_surrender", text)
        self.assertIn("recover_shaken", text)
        self.assertIn("sys_autodemo", text)
        self.assertIn("{state user_control}", text)
        self.assertIn("{tag player}", text)
        self.assertNotIn("{player 0}", text)
        self.assertNotIn("{action delete}", text)
        retreat_idx = text.find("broken_retreat")
        surrender_idx = text.find("retreat_to_surrender")
        self.assertGreater(surrender_idx, retreat_idx)

    def test_broken_modifier_exists(self) -> None:
        text = MOD.read_text(encoding="utf-8")
        self.assertIn("{name aio_morale_broken}", text)
        self.assertIn("{scale 0.35}", text)


if __name__ == "__main__":
    unittest.main()
