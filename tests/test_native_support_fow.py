from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BOT_MAIN = ROOT / "resource/script/multiplayer/bot.main.lua"
ATTACK_SUPPORT = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
NATIVE_TEST = ROOT / "resource/script/multiplayer/modes/native_support_fow_test.lua"
TEST_UNIT = ROOT / "resource/set/multiplayer/units/conquest/native_support_test_rusa.set"
GAME_SET = ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set"
PURCHASE_ROOT = ROOT / "resource/script/multiplayer/units"

UNIT_ID = "codex_native_support_test(rusa)"


class NativeSupportFowDiagnosticTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.bot_main = BOT_MAIN.read_text(encoding="utf-8")
        cls.attack_support = ATTACK_SUPPORT.read_text(encoding="utf-8")
        cls.native = NATIVE_TEST.read_text(encoding="utf-8")
        cls.unit = TEST_UNIT.read_text(encoding="utf-8")
        cls.game_set = GAME_SET.read_text(encoding="utf-8")

    def test_pr101_attack_handoff_does_not_reuse_defenderbot_or_native_probe(self) -> None:
        self.assertIn('setVar("id_attack_support_human", humanId)', self.attack_support)
        self.assertIn('setVar("id_attack_support_mate", mateId)', self.attack_support)
        self.assertIn('setVar("id_attack_support", humanId)', self.attack_support)
        self.assertNotIn('setVar("id_attack_support", id.defenderBotId)', self.attack_support)
        self.assertNotIn("CODEX_NATIVE_SUPPORT_TEST", self.attack_support)

    def test_custom_attack_support_bot_never_loads_native_diagnostic(self) -> None:
        attack_route = self.bot_main.index("if isAttackSupportCandidate(identity) then")
        attack_return = self.bot_main.index("return", attack_route)
        conquest_require = self.bot_main.index("native_support_fow_test")
        self.assertLess(attack_return, conquest_require)
        self.assertIn('safeRequire("resource/script/multiplayer/modes/attack_support")', self.bot_main)
        self.assertIn('safeRequire("resource/script/multiplayer/modes/native_support_fow_test")', self.bot_main)

    def test_diagnostic_is_hard_gated_to_rusa_human_defense_defenderbot(self) -> None:
        self.assertIn("local NATIVE_SUPPORT_FOW_TEST_ENABLED = true", self.native)
        self.assertIn('local TEST_ARMY = "rusa"', self.native)
        self.assertIn('id.gameMode ~= "campaign_capture_the_flag"', self.native)
        self.assertIn("id.army ~= TEST_ARMY", self.native)
        self.assertIn("id.playerId ~= id.defenderBotId", self.native)
        self.assertIn('readSceneVar("user_is_defender")', self.native)
        self.assertIn('return false, "human_attack"', self.native)
        self.assertIn('return true, "human_defense_defenderbot"', self.native)
        self.assertNotIn("id_attack_support", self.native)

    def test_diagnostic_has_explicit_one_shot_state_before_spawn_api(self) -> None:
        self.assertIn("attempted = false", self.native)
        start = self.native.index("local function attemptNativeSpawn(trigger)")
        end = self.native.index("local function onGameStart()", start)
        body = self.native[start:end]
        guard = body.index("if state.attempted or state.spawned then return false end")
        latch = body.index("state.attempted = true")
        spawn_at = body.index("c:SpawnAt(")
        spawn = body.index("c:Spawn(")
        self.assertLess(guard, latch)
        self.assertLess(latch, spawn_at)
        self.assertLess(latch, spawn)

        quant_start = self.native.index("local function onQuant()")
        quant_end = self.native.index("local function onGameSpawn", quant_start)
        quant = self.native[quant_start:quant_end]
        self.assertIn("if not state.attempted and state.applicable ~= false then", quant)
        self.assertEqual(quant.count("attemptNativeSpawn"), 1)

    def test_unresolved_gate_logs_state_changes_instead_of_quant_spam(self) -> None:
        self.assertIn("lastGateWaitReason = nil", self.native)
        self.assertIn("if state.lastGateWaitReason ~= reason then", self.native)
        self.assertIn("state.lastGateWaitReason = reason", self.native)
        self.assertNotIn("% 50", self.native)

    def test_diagnostic_uses_real_botapi_spawn_lifecycle_and_logs_game_spawn(self) -> None:
        required = (
            "CODEX_NATIVE_SUPPORT_TEST",
            "IsUnitAvailable",
            "CanSpawn",
            "c:SpawnAt(TEST_UNIT, MAX_SQUAD_SIZE, SPAWN_INDEX)",
            "c:Spawn(TEST_UNIT, MAX_SQUAD_SIZE)",
            "ev:Subscribe(ev.GameSpawn",
            'emit("GameSpawn"',
            'emit("order", "SeekAndDestroy"',
            '"FirstPlayerId"',
            '"FirstEnemyId"',
            '"DefenderBotId"',
            '"spawnPointName"',
        )
        for marker in required:
            with self.subTest(marker=marker):
                self.assertIn(marker, self.native)

    def test_unproven_canspawn_signature_is_observed_but_never_called(self) -> None:
        self.assertIn('"not_called_unproven_signature"', self.native)
        self.assertNotIn("c:CanSpawn(TEST_UNIT)", self.native)

    def test_diagnostic_does_not_require_utility_on_special_support_slot(self) -> None:
        self.assertNotIn("require([[/script/multiplayer/modes/utility]])", self.native)
        self.assertNotIn('require("resource/script/multiplayer/modes/utility")', self.native)
        self.assertIn("Do NOT require utility.lua", self.attack_support)

    def test_hidden_test_unit_is_four_canonical_infantry_and_zero_cost(self) -> None:
        self.assertIn('{"' + UNIT_ID + '"', self.unit)
        self.assertIn('{not_for_player_sale 1}', self.unit)
        self.assertIn('{cost 0}', self.unit)
        self.assertIn('vehicle()', self.unit)
        self.assertEqual(self.unit.count("crew1("), 1)
        self.assertEqual(self.unit.count("crew2("), 1)
        self.assertEqual(self.unit.count("crew3("), 1)
        self.assertEqual(self.unit.count("crew4("), 1)
        self.assertEqual(self.unit.count("{"), self.unit.count("}"))
        self.assertEqual(self.unit.count("("), self.unit.count(")"))
        for breed in (
            "rus90_squadlead",
            "rus90_seniorrifleman",
            "rus90_mg",
            "rus90_rifleman",
        ):
            self.assertIn(breed, self.unit)

    def test_test_unit_is_not_added_to_any_repo_purchase_table(self) -> None:
        offenders = []
        for path in PURCHASE_ROOT.rglob("*.lua"):
            if UNIT_ID in path.read_text(encoding="utf-8"):
                offenders.append(str(path.relative_to(ROOT)))
        self.assertEqual(offenders, [])

    def test_campaign_extra_team_a_slot_and_normal_enemy_ai_remain_present(self) -> None:
        self.assertIn('{aiTeamPlayers 1}', self.game_set)
        self.assertIn('campaign_capture_the_flag = "conquest"', self.bot_main)
        self.assertIn('evacuation = "laststand"', self.bot_main)
        self.assertNotIn("native_support_fow_test", self.game_set)


if __name__ == "__main__":
    unittest.main()
