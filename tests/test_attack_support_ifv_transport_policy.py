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

    def test_player_side_legacy_transport_paths_stay_hard_disabled(self) -> None:
        start = self.lua.find("local function disableLegacyAttackSupport()")
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

    def test_normal_transport_patrol_bypass_remains_gated_off(self) -> None:
        self.assertIn('"attack_support/normal_transport_rusa"', self.waves)
        self.assertIn('{var "transport_as_done$"} {op "=="} {value 0}', self.waves)
        self.assertIn('{tag ally_sup_rusa_motor_hull}', self.waves)
        self.assertIn('setVar("transport_as_done", 1)', self.lua)

    def test_isolation_gate_runs_at_load_start_spawn_and_quant(self) -> None:
        self.assertGreaterEqual(self.lua.count("disableLegacyAttackSupport()"), 4)
        game_start = self.lua[self.lua.find("local function onGameStart()") : self.lua.find("local function onQuant()")]
        quant = self.lua[self.lua.find("local function onQuant()") : self.lua.find("local function onGameEnd()")]
        attempt = self.lua[self.lua.find("local function attemptNativeBirth") : self.lua.find("local function pickFlagName")]
        self.assertIn("disableLegacyAttackSupport()", game_start)
        self.assertIn("disableLegacyAttackSupport()", quant)
        self.assertIn("disableLegacyAttackSupport()", attempt)

    def test_native_probe_does_not_reenable_legacy_mi_waves(self) -> None:
        self.assertIn('setVar("attack_support_ready", 0)', self.lua)
        self.assertIn('setVar("attack_support_use_mi", 0)', self.lua)
        self.assertNotIn('setVar("attack_support_ready", 1)', self.lua)
        self.assertNotIn('setVar("attack_support_use_mi", 1)', self.lua)


if __name__ == "__main__":
    unittest.main()
