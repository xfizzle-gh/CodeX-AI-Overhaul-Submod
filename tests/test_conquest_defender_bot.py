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

    def test_current_attacker_and_defender_cadence_remain_role_separated(self) -> None:
        # The old string-label cadence assertions were stale on main. Current runtime
        # expresses the separation through role-specific start times and ApplyWaveCadence.
        self.assertIn("DefenseMin = 0 * 60 * 1000", self.source)
        self.assertIn("DefenseMax = 0 * 60 * 1000", self.source)
        self.assertIn("AttackMin = 1 * 1000", self.source)
        self.assertIn("AttackMax = 1 * 1000", self.source)
        start = self.source.index("local function ApplyWaveCadence()")
        end = self.source.index("function OnGameStart()", start)
        cadence = self.source[start:end]
        self.assertGreaterEqual(cadence.count("if botDefender then"), 2)
        self.assertIn("WaveUnitOverride.DefendMin", cadence)
        self.assertIn("WaveUnitOverride.AttackMin", cadence)
        self.assertIn("DCGWaveOffOverwrite.DefenseMinWaveOff", cadence)
        self.assertIn("DCGWaveOffOverwrite.AttackMinWaveOff", cadence)

    def test_current_wave_size_uses_difficulty_scaled_roll(self) -> None:
        # main no longer carries the old NormalWaveSizeScale constant. The canonical
        # reduction/scale is now rollWaveSize() over the active WaveUnit range.
        start = self.source.index("local function rollWaveSize()")
        end = self.source.index("local function setDocVarsInNattorSpeak()", start)
        roll = self.source[start:end]
        self.assertIn("math.random(WaveUnit.Min, WaveUnit.Max)", roll)
        self.assertIn("ActiveDifficultySettings.waveScale", roll)
        self.assertIn("math.max(1, math.floor(raw * scale + 0.5))", roll)

    def test_wave_transition_advances_before_recalculation(self) -> None:
        transition = self.source.index("waveNumber = waveNumber + 1")
        recalculation = self.source.index("calculateWaveUnitTotal()", transition)
        self.assertLess(transition, recalculation)
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
        # setDocVarsInNattorSpeak no longer accepts currentDivision on main.
        self.assertIn(
            "if wroteMissionVars then\n\t\tsetDocVarsInNattorSpeak()",
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
        self.assertNotIn("KillSpawnCooldownTimer()", prep)
        self.assertNotIn("SetSpawnCooldownTimer()", prep)


if __name__ == "__main__":
    unittest.main()
