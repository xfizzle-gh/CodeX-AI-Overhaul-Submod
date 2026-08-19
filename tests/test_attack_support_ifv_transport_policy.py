from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ATTACK_SUPPORT_LUA = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
ATTACK_SUPPORT_WAVES = ROOT / "resource/map/multi/attack_support_waves.inc"


class AttackSupportIfvTransportPolicyTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.lua = ATTACK_SUPPORT_LUA.read_text(encoding="utf-8")
        cls.waves = ATTACK_SUPPORT_WAVES.read_text(encoding="utf-8")

    def test_player_side_transport_trucks_are_hard_disabled(self) -> None:
        start = self.lua.find("local function enforceIfvOnlyTransport()")
        self.assertGreater(start, 0)
        end = self.lua.find("\nend\n", start)
        self.assertGreater(end, start)
        body = self.lua[start:end]

        self.assertIn('setVar("attack_support_motor_left", 0)', body)
        self.assertIn('setVar("attack_support_hmmwv_left", 0)', body)
        self.assertIn('setVar("attack_support_motor_test", 0)', body)
        self.assertIn('setVar("transport_as_done", 1)', body)
        self.assertNotIn('setVar("attack_support_ifv_left", 0)', body)
        self.assertNotIn("enemy_attack_motor_left", body)
        self.assertNotIn("defense_support", body)

    def test_normal_transport_patrol_is_a_separate_truck_bypass_and_is_gated_off(self) -> None:
        # Regression for native 2026-08-16 result: the RUSA Ural package deployed
        # while motor_left=0 because this separate patrol ignores command 19.
        self.assertIn('"attack_support/normal_transport_rusa"', self.waves)
        self.assertIn('{var "transport_as_done$"} {op "=="} {value 0}', self.waves)
        self.assertIn('{tag ally_sup_rusa_motor_hull}', self.waves)

        start = self.lua.find("local function enforceIfvOnlyTransport()")
        end = self.lua.find("\nend\n", start)
        body = self.lua[start:end]
        self.assertIn('setVar("transport_as_done", 1)', body)

    def test_transport_policy_is_reasserted_before_waves_can_reenable_trucks(self) -> None:
        game_start = self.lua[self.lua.find("local function onGameStart()") : self.lua.find("local function onQuant()")]
        quant = self.lua[self.lua.find("local function onQuant()") : self.lua.find("local function onGameEnd()")]
        module_tail = self.lua[self.lua.find("local id0 = identity()") :]

        self.assertIn("enforceIfvOnlyTransport()", game_start)
        self.assertIn("enforceIfvOnlyTransport()", quant)
        self.assertIn("enforceIfvOnlyTransport()", module_tail)
        self.assertIn('"transport", "ifv_only"', game_start)
        self.assertIn("identity_orders_mi_waves_ifv_only", module_tail)

    def test_log_mirror_exposes_normal_transport_gate(self) -> None:
        mirror = self.lua[self.lua.find("local function mirrorMotor()") : self.lua.find("local state =")]
        self.assertIn('"normal_transport_done", readVar("transport_as_done")', mirror)


if __name__ == "__main__":
    unittest.main()
