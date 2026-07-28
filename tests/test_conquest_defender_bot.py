from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
CONQUEST_LUA = ROOT / "resource/script/multiplayer/modes/conquest.lua"


class ConquestDefenderBotSourceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.source = CONQUEST_LUA.read_text(encoding="utf-8")

    def test_engine_owned_defender_bot_identity(self) -> None:
        self.assertIn(
            "isAlliedDefenderBot = defenderBotId > 0 and myId == defenderBotId",
            self.source,
        )
        self.assertIn("botDefender = not BotApi.Conquest.Attacking", self.source)
        self.assertIn("resolvePositiveId(conquest.DefenderBotId", self.source)
        self.assertNotIn("botDefender = teamSize > 1\n\tif printDebug", self.source)

    def test_one_script_keeps_three_cadences_separate(self) -> None:
        self.assertIn('cadence = "allied-support"', self.source)
        self.assertIn('cadence = "enemy-defender"', self.source)
        self.assertIn('cadence = "enemy-attacker"', self.source)
        self.assertIn("DCGWaveOffMin_AlliedSupport = 4 * 60 * 1000", self.source)
        self.assertIn("DCGWaveOffMax_AlliedSupport = 8 * 60 * 1000", self.source)

    def test_allied_opening_wave_is_preserved(self) -> None:
        self.assertIn(
            'cadence = isAlliedDefenderBot and "allied-opening" or "enemy-defender-opening"',
            self.source,
        )
        self.assertIn("if isAlliedDefenderBot and waveNumber > 0 then", self.source)
        self.assertIn("Min_AlliedSupport = 1", self.source)
        self.assertIn("Max_AlliedSupport = 3", self.source)
        allied_branch = self.source.index(
            "if isAlliedDefenderBot and waveNumber > 0 then"
        )
        global_clamp = self.source.index("waveUnitTotal = math.max(3", allied_branch)
        self.assertLess(allied_branch, global_clamp)

    def test_wave_transition_advances_before_recalculation(self) -> None:
        transition = self.source.index("waveNumber = waveNumber + 1")
        recalculation = self.source.index("calculateWaveUnitTotal()", transition)
        self.assertLess(transition, recalculation)
        self.assertNotIn("if not botDefender or botDefender then", self.source)
        self.assertIn("waveSpawnPossible = true", self.source)

    def test_allied_support_is_held_until_prep_ends(self) -> None:
        self.assertIn(
            "isAlliedDefenderBot and waveNumber > 0 and not prepTimeOver",
            self.source,
        )
        self.assertIn("DCG allied support held until PrepTimeOver", self.source)
        self.assertIn("KillSpawnCooldownTimer()", self.source)
        self.assertIn("SetSpawnCooldownTimer()", self.source)
        self.assertIn("DCG allied support released after preparation", self.source)

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

        standalone_calls = [
            line.strip()
            for line in self.source.splitlines()
            if line.strip() == "setDocVarsInNattorSpeak(currentDivision)"
        ]
        self.assertEqual(len(standalone_calls), 1)
        self.assertIn(
            "if wroteMissionVars then setDocVarsInNattorSpeak(currentDivision) end",
            self.source,
        )

    def test_infantry_bias_and_ai_ownership_are_preserved(self) -> None:
        self.assertIn('if UnitType("Infantry") then', self.source)
        self.assertIn("BotApi.Commands:SpawnAt", self.source)
        self.assertIn("BotApi.Commands:Spawn(unit, maxSquadSize)", self.source)
        self.assertNotIn("control user", self.source)

    def test_first_quant_retries_late_conquest_ids_once(self) -> None:
        self.assertIn(
            "firstEnemyId <= 0 or defenderBotId <= 0 or firstPlayerId <= 0",
            self.source,
        )
        self.assertIn("local function retryMissionIdentityOnce()", self.source)
        self.assertIn("missionIdentityRetryPending = false", self.source)


if __name__ == "__main__":
    unittest.main()
