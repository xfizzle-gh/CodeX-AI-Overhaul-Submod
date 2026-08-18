from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MACHINE = ROOT / "resource/map/multi/ce/ce_morale_machine_triggers.inc"
HUMAN = ROOT / "resource/set/interaction_entity/human_ce.inc"
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
        text = MACHINE.read_text(encoding="utf-8")
        self.assertIn("observe_ai", text)
        self.assertIn("shaken_entry", text)
        self.assertIn("escalate_panic", text)
        self.assertIn("start_recover", text)

    def test_fe_recovery_handlers_exist(self) -> None:
        human = HUMAN.read_text(encoding="utf-8")
        self.assertIn('{on "recovering_from_shaken"', human)
        self.assertIn('{on "recovering_from_panic"', human)
        self.assertIn('{on "recovered_from_shaken"', human)
        self.assertIn('{on "recovered_from_panic"', human)
        self.assertIn("{delay 20", human)
        self.assertIn('{tags remove "aio_morale_recovering"}', human)
        machine = MACHINE.read_text(encoding="utf-8")
        self.assertIn("{effect recovering_from_shaken}", machine)
        self.assertIn("{effect recovering_from_panic}", machine)
        self.assertIn("{state suppressed}", machine)
        self.assertIn("aio_morale_recovering", machine)
        self.assertIn('{tags remove "aio_morale_recovering"}', human)
        effect = machine.split("{effect aio_morale_just_shaken}", 1)[0]
        effect = effect[effect.rfind('{"effect"'):]
        self.assertIn("{state inactive}", effect)
        self.assertIn("{state linked}", effect)
        self.assertIn("{state dead}", effect)
        recover_fx = machine.split("{effect recovering_from_shaken}", 1)[0]
        recover_fx = recover_fx[recover_fx.rfind('{"effect"'):]
        self.assertIn("{state inactive}", recover_fx)

    def test_pressure_is_suppressed_and_cancels_recovery(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        entry = text.split("shaken_entry", 1)[1].split("escalate_panic", 1)[0]
        self.assertIn("{state suppressed}", entry)
        self.assertNotIn("see_actors", entry)
        cancel = text.split("refresh_cancel_recover", 1)[1].split("start_recover", 1)[0]
        self.assertIn("{tag_remove aio_morale_recovering}", cancel)
        self.assertIn("{state suppressed}", cancel)

    def test_pr_c_excludes_later_phases(self) -> None:
        text = MACHINE.read_text(encoding="utf-8")
        self.assertNotIn("aio_morale_broken", text)
        self.assertNotIn("aio_morale_retreating", text)
        self.assertNotIn("aio_morale_surrendering", text)
        self.assertNotIn("{name aio_morale_broken}", MOD.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
