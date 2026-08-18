from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MACHINE = ROOT / "resource/map/multi/ce/ce_morale_machine_triggers.inc"
DCG = ROOT / "resource/map/multi/dcg_script.inc"
LUA = ROOT / "resource/script/multiplayer/modes/utility_ce.lua"


class CeMoraleMachineTests(unittest.TestCase):
    def test_live_stack_loads_machine(self) -> None:
        self.assertIn("ce_morale_machine_triggers.inc", DCG.read_text(encoding="utf-8"))
        lua = LUA.read_text(encoding="utf-8")
        self.assertIn("recover_clear=", lua)
        self.assertIn("observe_ai", MACHINE.read_text(encoding="utf-8"))

    def test_recovery_uses_per_actor_age_ticks(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        self.assertIn("recover_arm_panic", text)
        self.assertIn("recover_arm_shaken", text)
        self.assertIn("aio_morale_rp0", text)
        self.assertIn("aio_morale_rp3", text)
        self.assertIn("aio_morale_rs0", text)
        self.assertIn("aio_morale_rs3", text)
        self.assertNotIn('{time 8}', text)
        tick = text.split("age_tick", 1)[1].split("escalate_panic", 1)[0]
        self.assertLess(tick.find("aio_morale_rp3"), tick.find("aio_morale_rp0"))
        self.assertLess(tick.find("aio_morale_rs3"), tick.find("aio_morale_rs0"))
        refresh = text.split("refresh_pressure", 1)[1].split("age_tick", 1)[0]
        self.assertIn("aio_morale_rp0", refresh)
        self.assertIn("aio_morale_rs0", refresh)

    def test_production_pressure_is_native_suppressed(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        entry = text.split("shaken_entry", 1)[1].split("refresh_pressure", 1)[0]
        self.assertIn("{state suppressed}", entry)
        self.assertNotIn("see_actors", entry)


if __name__ == "__main__":
    unittest.main()
