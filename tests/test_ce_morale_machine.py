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
        self.assertIn("recover=", lua)
        self.assertIn("RECOVER_FAIL", lua)
        self.assertNotIn("broken=", lua)
        self.assertNotIn("retreat=", lua)
        self.assertNotIn("surrender=", lua)

    def test_pr_c_is_shaken_panic_recovery_only(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        self.assertIn("pressure_suppressed", text)
        self.assertIn("{state suppressed}", text)
        self.assertIn("escalate_panic", text)
        self.assertIn("recover_shaken", text)
        self.assertIn("sys_autodemo", text)
        self.assertIn("ce_morale_diag_shaken$", text)
        self.assertIn("ce_morale_diag_recover$", text)
        self.assertIn("{state user_control}", text)
        self.assertNotIn("see_enemy", text)
        self.assertNotIn("aio_morale_broken", text)
        self.assertNotIn("aio_morale_retreating", text)
        self.assertNotIn("aio_morale_surrendering", text)
        self.assertNotIn("aio_cmd_", text)
        self.assertNotIn("{player 0}", text)

    def test_modifiers_are_shaken_and_panic_only(self) -> None:
        text = MOD.read_text(encoding="utf-8")
        self.assertIn("{name aio_morale_shaken}", text)
        self.assertIn("{name aio_morale_panic}", text)
        self.assertNotIn("aio_morale_broken", text)


if __name__ == "__main__":
    unittest.main()
