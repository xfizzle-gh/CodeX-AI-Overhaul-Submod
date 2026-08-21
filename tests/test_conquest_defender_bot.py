from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
CONQUEST_LUA = ROOT / "resource/script/multiplayer/modes/conquest.lua"


class ConquestRuntimeSourceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.source = CONQUEST_LUA.read_text(encoding="utf-8")

    def test_engine_ids_are_resolved_and_published(self) -> None:
        self.assertIn("resolvePositiveId(conquest.FirstEnemyId", self.source)
        self.assertIn("resolvePositiveId(conquest.DefenderBotId", self.source)
        self.assertIn("resolvePositiveId(conquest.FirstPlayerId", self.source)
        self.assertIn('BotApi.Scene:SetVar("id_defenderbot", defenderBotId)', self.source)
        self.assertIn('"defenderBotPurchaseHost", false', self.source)

    def test_hidden_defender_owner_has_no_purchase_branch(self) -> None:
        forbidden = (
            "isAlliedDefenderBot",
            "DCGWaveOffMin_AlliedSupport",
            "DCGWaveOffMax_AlliedSupport",
            "Min_AlliedSupport",
            "Max_AlliedSupport",
            "DCG allied support",
            "allied-prep-hold",
            "allied-opening",
        )
        for marker in forbidden:
            self.assertNotIn(marker, self.source)

    def test_runtime_bot_cadences_remain_separate(self) -> None:
        self.assertIn('print("DCG cadence"', self.source)
        self.assertIn("StartSpawnTime", self.source)
        self.assertIn("SpawnCooldownTime", self.source)
        self.assertIn("botDefender", self.source)

    def test_normal_calculated_waves_keep_global_reduction(self) -> None:
        self.assertIn("function rollWaveSize()", self.source)
        self.assertIn("ActiveDifficultySettings.waveScale", self.source)
        self.assertIn("WaveUnit.Min", self.source)

    def test_wave_transition_advances_before_recalculation(self) -> None:
        self.assertIn("waveNumber = waveNumber + 1", self.source)
        self.assertIn("rollWaveSize()", self.source)
        self.assertNotIn("if not botDefender or botDefender then", self.source)
        self.assertIn("waveSpawnPossible = true", self.source)

    def test_only_mission_authority_writes_perspective_vars(self) -> None:
        authority_guard = self.source.index(
            "if not isMissionAuthority() then return false end"
        )
        perspective_var = self.source.index(
            'BotApi.Scene:SetVar("user_is_defender"', authority_guard
        )
        ce_vars = self.source.index("SetCEMissionVariables(botDefender)", authority_guard)
        self.assertLess(authority_guard, perspective_var)
        self.assertLess(authority_guard, ce_vars)
        self.assertIn(
            "if wroteMissionVars then setDocVarsInNattorSpeak() end",
            self.source,
        )

    def test_ai_purchase_ownership_and_orders_are_preserved(self) -> None:
        self.assertIn("BotApi.Commands:SpawnAt", self.source)
        self.assertIn("BotApi.Commands:Spawn(unit, maxSquadSize)", self.source)
        self.assertIn("TrySpawnUnit()", self.source)
        self.assertIn("BotApi.Commands:SeekAndDestroy", self.source)
        self.assertIn("BotApi.Commands:CaptureFlag", self.source)
        self.assertNotIn("control user", self.source)

    def test_first_quant_retries_late_conquest_ids_once(self) -> None:
        self.assertIn(
            "firstEnemyId <= 0 or defenderBotId <= 0 or firstPlayerId <= 0",
            self.source,
        )
        self.assertIn("local function retryMissionIdentityOnce()", self.source)
        self.assertIn("missionIdentityRetryPending = false", self.source)

    def test_prep_event_only_updates_mission_and_enemy_attack_release(self) -> None:
        start = self.source.index("function OnPrepTimeOver()")
        end = self.source.index("BotApi.Events:Subscribe", start)
        prep = self.source[start:end]
        self.assertIn('BotApi.Scene:SetVar("prep_inform", 1)', prep)
        self.assertIn("if not botDefender and not ai_attack_started then", prep)
        self.assertIn("scheduleEmptyFieldSpawnKick()", prep)
        self.assertNotIn("KillSpawnCooldownTimer()", prep)
        self.assertNotIn("SetSpawnCooldownTimer()", prep)

    def test_arty_flag_slots_only_match_capture_points(self) -> None:
        self.assertIn('tostring(name or ""):match("^f([1-5])$")', self.source)
        self.assertNotIn('match("(%d+)$")', self.source)

    def test_empty_field_spawn_kick_clears_wave_off(self) -> None:
        self.assertIn("function scheduleEmptyFieldSpawnKick()", self.source)
        self.assertIn("DCG empty-field spawn kick", self.source)
        self.assertIn("KillSpawnCooldownTimer()", self.source)


if __name__ == "__main__":
    unittest.main()
