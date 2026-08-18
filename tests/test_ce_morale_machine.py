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
        self.assertIn("recover_panic=", lua)
        self.assertIn("recover_clear=", lua)
        self.assertIn("RECOVER_FAIL", lua)
        self.assertIn("RECOVER_PANIC_FAIL", lua)
        text = MACHINE.read_text(encoding="utf-8")
        self.assertIn("observe_ai", text)
        self.assertIn("shaken_entry", text)
        self.assertIn("refresh_pressure", text)
        self.assertIn("age_tick", text)
        self.assertIn("escalate_panic", text)

    def test_pressure_refresh_keeps_just_shaken_and_strips_recover_ages(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        entry = text.split("shaken_entry", 1)[1].split("refresh_pressure", 1)[0]
        refresh = text.split("refresh_pressure", 1)[1].split("age_tick", 1)[0]
        self.assertIn("{state suppressed}", entry)
        self.assertIn("aio_morale_just_shaken", entry)
        self.assertIn("aio_morale_j0", entry)
        self.assertNotIn("see_actors", entry)
        self.assertNotIn("see_enemy", entry)
        self.assertIn("{state suppressed}", refresh)
        self.assertIn("aio_morale_recent_pressure", refresh)
        self.assertIn("aio_morale_p0", refresh)
        self.assertNotIn("aio_morale_just_shaken", refresh)
        self.assertNotIn("aio_morale_j0", refresh)
        for tag in (
            "aio_morale_rp0",
            "aio_morale_rp1",
            "aio_morale_rp2",
            "aio_morale_rp3",
            "aio_morale_rs0",
            "aio_morale_rs1",
            "aio_morale_rs2",
            "aio_morale_rs3",
        ):
            self.assertIn(tag, refresh)
        escalate = text.split("escalate_panic", 1)[1].split("recover_arm_panic", 1)[0]
        self.assertIn("{state suppressed}", escalate)
        self.assertIn("aio_morale_just_shaken", escalate)

    def test_age_ticks_are_concurrent_and_oldest_first(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        tick = text.split("age_tick", 1)[1].split("escalate_panic", 1)[0]
        self.assertIn("aio_morale_p3", tick)
        self.assertIn("aio_morale_p0", tick)
        self.assertIn("aio_morale_j2", tick)
        self.assertIn("aio_morale_j0", tick)
        self.assertIn("aio_morale_rp3", tick)
        self.assertIn("aio_morale_rp0", tick)
        self.assertIn("aio_morale_rs3", tick)
        self.assertIn("aio_morale_rs0", tick)
        self.assertLess(tick.find("aio_morale_p3"), tick.find("aio_morale_p0"))
        self.assertLess(tick.find("aio_morale_j2"), tick.find("aio_morale_j0"))
        self.assertLess(tick.find("aio_morale_rp3"), tick.find("aio_morale_rp0"))
        self.assertLess(tick.find("aio_morale_rs3"), tick.find("aio_morale_rs0"))
        self.assertNotIn("{amount 1}", tick)
        self.assertNotIn("just_shaken_busy", tick)
        self.assertNotIn("pressure_busy", tick)

    def test_recovery_latches_after_transition_and_excludes_later_phases(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        tick = text.split("age_tick", 1)[1].split("escalate_panic", 1)[0]
        self.assertLess(
            tick.find("{tag_remove aio_morale_panic}"),
            tick.find("ce_morale_diag_recover_panic$"),
        )
        self.assertLess(
            tick.find("{tag_remove aio_morale_shaken}"),
            tick.find("ce_morale_diag_recover_clear$"),
        )
        self.assertNotIn('{time 8}', text)
        self.assertNotIn("aio_morale_broken", text)
        self.assertNotIn("aio_morale_retreating", text)
        self.assertNotIn("aio_morale_surrendering", text)
        self.assertNotIn("{name aio_morale_broken}", MOD.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
