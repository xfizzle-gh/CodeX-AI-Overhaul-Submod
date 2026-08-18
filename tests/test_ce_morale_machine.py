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
        self.assertIn("recover_clear=", lua)
        self.assertIn("observe_ai", MACHINE.read_text(encoding="utf-8"))

    def test_production_pressure_is_native_suppressed(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        entry = text.split("shaken_entry", 1)[1].split("refresh_pressure", 1)[0]
        refresh = text.split("refresh_pressure", 1)[1].split("age_tick", 1)[0]
        self.assertIn("{state suppressed}", entry)
        self.assertIn("aio_morale_just_shaken", entry)
        self.assertIn("aio_morale_j0", entry)
        self.assertIn("{state suppressed}", refresh)
        self.assertIn("aio_morale_recent_pressure", refresh)
        self.assertIn("aio_morale_p0", refresh)
        self.assertNotIn("aio_morale_just_shaken", refresh)
        self.assertNotIn("aio_morale_j0", refresh)
        self.assertNotIn("see_actors", entry)
        escalate = text.split("escalate_panic", 1)[1].split("recover_panic", 1)[0]
        self.assertIn("{state suppressed}", escalate)
        self.assertIn("aio_morale_just_shaken", escalate)

    def test_age_ticks_are_concurrent_and_oldest_first(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        tick = text.split("age_tick", 1)[1].split("escalate_panic", 1)[0]
        self.assertIn("aio_morale_p3", tick)
        self.assertIn("aio_morale_p0", tick)
        self.assertIn("aio_morale_j2", tick)
        self.assertIn("aio_morale_j0", tick)
        self.assertLess(tick.find("aio_morale_p3"), tick.find("aio_morale_p0"))
        self.assertLess(tick.find("aio_morale_j2"), tick.find("aio_morale_j0"))
        self.assertNotIn("{amount 1}", tick)
        self.assertNotIn("just_shaken_busy", tick)
        self.assertNotIn("pressure_busy", tick)

    def test_recovery_latches_after_transition(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        recover_panic = text.split('{"conquest_enhanced_mechanics/morale/recover_panic"', 1)[1]
        recover_panic = recover_panic.split("recover_shaken", 1)[0]
        self.assertLess(
            recover_panic.find("{tag_remove aio_morale_panic}"),
            recover_panic.find("ce_morale_diag_recover_panic$"),
        )
        self.assertNotIn("aio_morale_broken", text)


if __name__ == "__main__":
    unittest.main()
