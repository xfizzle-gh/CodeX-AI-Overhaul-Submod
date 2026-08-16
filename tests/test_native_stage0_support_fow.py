from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
LUA = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
UNIT = ROOT / "resource/set/multiplayer/units/conquest/codex_native_support_rusa.set"
GAME = ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set"


class NativeStageZeroSupportTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.lua = LUA.read_text(encoding="utf-8")
        cls.unit = UNIT.read_text(encoding="utf-8")
        cls.game = GAME.read_text(encoding="utf-8")

    def test_hidden_stage_zero_unit_uses_existing_rusa_breeds(self) -> None:
        self.assertIn('"codex_native_support_stage0(rusa)"', self.unit)
        self.assertIn("min_stage(0)", self.unit)
        self.assertIn("max_stage(99)", self.unit)
        self.assertIn("{cost 0}", self.unit)
        self.assertIn("{not_for_player_sale 1}", self.unit)
        for breed in (
            "rus90_squadlead",
            "rus90_seniorrifleman",
            "rus90_mg",
            "rus90_rifleman",
        ):
            self.assertIn(breed, self.unit)

    def test_probe_stays_on_existing_conquest_unitset(self) -> None:
        self.assertIn('{unitset {value "conquest"}', self.game)
        self.assertNotIn('{unitset {value "2022s"}', self.game)
        self.assertIn("{aiTeamPlayers 1}", self.game)

    def test_native_call_is_hard_gated_by_availability_and_can_spawn(self) -> None:
        attempt = self.lua[
            self.lua.find("local function attemptNativeBirth") :
            self.lua.find("local function pickFlagName")
        ]
        self.assertIn("if not checkAvailability(cmd) then", attempt)
        self.assertIn('"native_call", "suppressed"', attempt)
        self.assertIn("if not checkCanSpawn(cmd) then", attempt)
        self.assertIn("cmd:SpawnAt(UNIT, MAX_SQUAD_SIZE, 0)", attempt)
        self.assertNotIn("cmd:Spawn(UNIT", attempt)

        availability_pos = attempt.index("if not checkAvailability(cmd) then")
        can_spawn_pos = attempt.index("if not checkCanSpawn(cmd) then")
        spawn_pos = attempt.index("cmd:SpawnAt(UNIT, MAX_SQUAD_SIZE, 0)")
        self.assertLess(availability_pos, can_spawn_pos)
        self.assertLess(can_spawn_pos, spawn_pos)

    def test_no_fragile_or_rejected_spawn_context_paths_return(self) -> None:
        self.assertNotIn(":QueryScene(", self.lua)
        self.assertNotIn("require([[/script/multiplayer/modes/utility]])", self.lua)
        self.assertNotIn("require(\"resource/script/multiplayer/modes/utility\")", self.lua)
        self.assertNotIn("PlayerSpawnPoint", self.lua)
        self.assertNotIn("spawnPointName", self.lua)

    def test_legacy_player_zero_support_is_disabled_for_the_probe(self) -> None:
        gate = self.lua[
            self.lua.find("local function disableLegacyAttackSupport") :
            self.lua.find("local state =")
        ]
        self.assertIn('setVar("attack_support_ready", 0)', gate)
        self.assertIn('setVar("attack_support_use_mi", 0)', gate)
        self.assertIn('setVar("attack_support_motor_left", 0)', gate)
        self.assertIn('setVar("attack_support_hmmwv_left", 0)', gate)
        self.assertIn('setVar("attack_support_motor_test", 0)', gate)
        self.assertIn('setVar("transport_as_done", 1)', gate)
        self.assertNotIn('setVar("attack_support_ifv_left", 0)', gate)

    def test_probe_is_one_shot_rusa_human_attack_only(self) -> None:
        self.assertIn("state.attempted = true", self.lua)
        self.assertIn('if tostring(i.gameMode or "") ~= "campaign_capture_the_flag" then', self.lua)
        self.assertIn("if c.Attacking ~= true then", self.lua)
        self.assertIn('if tostring(i.team or "") ~= "a" then', self.lua)
        self.assertIn('if tostring(i.army or "") ~= "rusa" then', self.lua)
        self.assertIn('"rusa_proof_only"', self.lua)

    def test_game_spawn_is_required_before_tmai_style_settle_and_order(self) -> None:
        self.assertIn('ev:Subscribe(ev.GameSpawn, safeEvent("GameSpawn", onGameSpawn))', self.lua)
        self.assertIn('"native_birth_confirmed", true', self.lua)
        self.assertIn("SETTLE_MS = 3000", self.lua)
        self.assertIn("cmd:CaptureFlag(squad, flagName)", self.lua)
        self.assertIn("cmd:SeekAndDestroy(squad)", self.lua)

    def test_no_manual_transfer_or_owner_permutation_in_probe(self) -> None:
        self.assertNotIn('SetVar("id_attack_support"', self.lua)
        self.assertNotIn('setVar("id_attack_support"', self.lua)
        self.assertNotIn("DefenderBotId", self.lua)
        self.assertNotIn("FirstPlayerId", self.lua)
        self.assertIn('"manual_transfer", "not_required"', self.lua)


if __name__ == "__main__":
    unittest.main()
