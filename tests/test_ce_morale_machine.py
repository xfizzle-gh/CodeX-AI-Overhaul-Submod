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
        self.assertIn("recover_panic=", lua)
        self.assertIn("RECOVER_PANIC_FAIL", lua)

    def test_pressure_and_panic_bind_matched_actor(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        pressure = text.split("pressure_contact", 1)[1].split("expire_just_shaken", 1)[0]
        self.assertIn('{"tag pair"}', pressure)
        self.assertIn('{"for selector" aio_morale_saw_enemy}', pressure)
        self.assertIn("{tag aio_morale_saw_enemy}", pressure)
        self.assertNotIn("{state user_control}", pressure)
        escalate = text.split("escalate_panic", 1)[1].split("recover_panic", 1)[0]
        self.assertIn('{"tag pair"}', escalate)
        self.assertIn('{"for selector" aio_morale_saw_panic}', escalate)
        self.assertIn("{tag aio_morale_saw_panic}", escalate)
        self.assertIn("aio_morale_just_shaken", escalate)
        self.assertIn("aio_morale_recent_pressure", escalate)

    def test_expiry_timers_are_actor_local(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        just = text.split("expire_just_shaken", 1)[1].split("expire_pressure", 1)[0]
        self.assertIn("aio_morale_just_shaken_busy", just)
        self.assertIn("{amount 1}", just)
        pressure = text.split("expire_pressure", 1)[1].split("escalate_panic", 1)[0]
        self.assertIn("aio_morale_pressure_busy", pressure)
        self.assertIn("{amount 1}", pressure)
        self.assertIn("{tag aio_morale_pressure_busy}", pressure)

    def test_recovery_latches_after_transition_and_requires_pressure_expiry(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        recover_panic = text.split("recover_panic", 1)[1].split("recover_shaken", 1)[0]
        self.assertIn("aio_morale_recent_pressure", recover_panic)
        self.assertLess(
            recover_panic.find("{tag_remove aio_morale_panic}"),
            recover_panic.find("ce_morale_diag_recover_panic$"),
        )
        recover_shaken = text.split('{"conquest_enhanced_mechanics/morale/recover_shaken"', 1)[1]
        self.assertIn("aio_morale_recent_pressure", recover_shaken)
        self.assertLess(
            recover_shaken.find("{tag_remove aio_morale_shaken}"),
            recover_shaken.find("ce_morale_diag_recover$"),
        )

    def test_pr_c_excludes_later_phases(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        self.assertNotIn("aio_morale_broken", text)
        self.assertNotIn("aio_morale_retreating", text)
        self.assertNotIn("aio_morale_surrendering", text)
        self.assertNotIn("{name aio_morale_broken}", MOD.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
