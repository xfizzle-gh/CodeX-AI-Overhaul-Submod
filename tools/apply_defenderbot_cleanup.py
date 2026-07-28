from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONQUEST = ROOT / "resource/script/multiplayer/modes/conquest.lua"
TESTS = ROOT / "tests/test_conquest_defender_bot.py"
SELF = ROOT / "tools/apply_defenderbot_cleanup.py"
WORKFLOW = ROOT / ".github/workflows/apply-defenderbot-cleanup.yml"


def replace_once(source: str, old: str, new: str, label: str) -> str:
    count = source.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected exactly one match, found {count}")
    return source.replace(old, new, 1)


source = CONQUEST.read_text(encoding="utf-8")

source = replace_once(
    source,
    '''    -- Time between allied DefenderBot support waves after preparation
    DCGWaveOffMin_AlliedSupport = 4 * 60 * 1000,
    DCGWaveOffMax_AlliedSupport = 8 * 60 * 1000,
''',
    '',
    'allied cooldown constants',
)

source = replace_once(
    source,
    '''    -- Allied DefenderBot follow-up support only; opening wave uses defender values
    Min_AlliedSupport = 1,
    Max_AlliedSupport = 3,
''',
    '',
    'allied wave-size constants',
)

source = replace_once(
    source,
    '-- Global reduction for normal calculated waves. Allied 1-3 support waves return early and are unchanged.\n',
    '-- Global reduction for all runtime AI purchase waves.\n',
    'normal wave scale comment',
)

source = replace_once(
    source,
    '''local isAlliedDefenderBot = false
local prepTimeOver = not (BotApi.Events and BotApi.Events.PrepTimeOver)
local missionIdentityRetryPending = false
local alliedPrepHoldLogged = false
''',
    '''local missionIdentityRetryPending = false
''',
    'dead allied state',
)

source = replace_once(
    source,
    '\tisAlliedDefenderBot = defenderBotId > 0 and myId == defenderBotId\n',
    '',
    'allied identity assignment',
)

source = replace_once(
    source,
    '\t\tprint("DCG role", "playerId", myId, "botDefender", botDefender, "firstEnemyId", firstEnemyId, "defenderBotId", defenderBotId, "firstPlayerId", firstPlayerId, "isAlliedDefenderBot", isAlliedDefenderBot)\n',
    '\t\tprint("DCG role", "playerId", myId, "botDefender", botDefender, "firstEnemyId", firstEnemyId, "defenderBotId", defenderBotId, "firstPlayerId", firstPlayerId, "defenderBotPurchaseHost", false)\n',
    'role diagnostic',
)

source = replace_once(
    source,
    '''\t-- Allied DefenderBot keeps the normal opening force, then switches to small support waves.
\tif isAlliedDefenderBot and waveNumber > 0 then
\t\twaveUnitTotal = math.random(WaveUnit.Min_AlliedSupport, WaveUnit.Max_AlliedSupport)
\t\tif printDebug then print("DCG allied support wave size", "playerId", myId, "waveNumber", waveNumber, "waveUnitTotal", waveUnitTotal) end
\t\treturn
\tend

''',
    '',
    'allied wave calculator branch',
)

source = replace_once(
    source,
    '\tif printDebug then print("Print: waveUnitTotal", waveUnitTotal, "waveNumber", waveNumber, "normalWaveSizeScale", NormalWaveSizeScale, "isAlliedDefenderBot", isAlliedDefenderBot) end\n',
    '\tif printDebug then print("Print: waveUnitTotal", waveUnitTotal, "waveNumber", waveNumber, "normalWaveSizeScale", NormalWaveSizeScale) end\n',
    'wave-size diagnostic',
)

source = replace_once(
    source,
    '\t\tif printDebug then print("Print: waveNumber", waveNumber, "SelectedDivision", currentDivision, "isAlliedDefenderBot", isAlliedDefenderBot) end\n',
    '\t\tif printDebug then print("Print: waveNumber", waveNumber, "SelectedDivision", currentDivision) end\n',
    'wave-number diagnostic',
)

old_cooldown = '''function GameModeSpawnCooldown()
\tWaveAttack()
\tlocal spawnTime
\tlocal cadence = "within-wave"

\tif botDefender and firstPurchase then
\t\tspawnTime = {Min = StartSpawnTime.DefenseMin, Max = StartSpawnTime.DefenseMax}
\t\tcadence = isAlliedDefenderBot and "allied-opening" or "enemy-defender-opening"
\telseif firstPurchase then
\t\tspawnTime = {Min = StartSpawnTime.AttackMin, Max = StartSpawnTime.AttackMax}
\t\tcadence = "enemy-attacker-opening"
\telseif not waveSpawnActive then
\t\tif isAlliedDefenderBot then
\t\t\tif prepTimeOver then
\t\t\t\tspawnTime = {Min = SpawnCooldownTime.DCGWaveOffMin_AlliedSupport, Max = SpawnCooldownTime.DCGWaveOffMax_AlliedSupport}
\t\t\t\tcadence = "allied-support"
\t\t\telse
\t\t\t\t-- OnGameQuant blocks the purchase; PrepTimeOver starts the real 4-8 minute timer.
\t\t\t\tspawnTime = {Min = 1 * 1000, Max = 1 * 1000}
\t\t\t\tcadence = "allied-prep-hold"
\t\t\tend
\t\telseif botDefender then
\t\t\tspawnTime = {Min = SpawnCooldownTime.DCGWaveOffMin_Defender, Max = SpawnCooldownTime.DCGWaveOffMax_Defender}
\t\t\tcadence = "enemy-defender"
\t\telse
\t\t\tspawnTime = {Min = SpawnCooldownTime.DCGWaveOffMin_Attacker, Max = SpawnCooldownTime.DCGWaveOffMax_Attacker}
\t\t\tcadence = "enemy-attacker"
\t\tend
\telse
\t\tspawnTime = {Min = SpawnCooldownTime.DCGMin, Max = SpawnCooldownTime.DCGMax}
\tend

\tlocal cooldown = math.random(spawnTime.Min, spawnTime.Max)
\tif printDebug then print("DCG cadence", cadence, "playerId", myId, "waveNumber", waveNumber, "cooldownSeconds", cooldown / 1000) end
\tfirstPurchase = false
\treturn cooldown
end
'''
new_cooldown = '''function GameModeSpawnCooldown()
\tWaveAttack()
\tlocal spawnTime
\tlocal cadence = "within-wave"

\tif botDefender and firstPurchase then
\t\tspawnTime = {Min = StartSpawnTime.DefenseMin, Max = StartSpawnTime.DefenseMax}
\t\tcadence = "enemy-defender-opening"
\telseif firstPurchase then
\t\tspawnTime = {Min = StartSpawnTime.AttackMin, Max = StartSpawnTime.AttackMax}
\t\tcadence = "enemy-attacker-opening"
\telseif not waveSpawnActive then
\t\tif botDefender then
\t\t\tspawnTime = {Min = SpawnCooldownTime.DCGWaveOffMin_Defender, Max = SpawnCooldownTime.DCGWaveOffMax_Defender}
\t\t\tcadence = "enemy-defender"
\t\telse
\t\t\tspawnTime = {Min = SpawnCooldownTime.DCGWaveOffMin_Attacker, Max = SpawnCooldownTime.DCGWaveOffMax_Attacker}
\t\t\tcadence = "enemy-attacker"
\t\tend
\telse
\t\tspawnTime = {Min = SpawnCooldownTime.DCGMin, Max = SpawnCooldownTime.DCGMax}
\tend

\tlocal cooldown = math.random(spawnTime.Min, spawnTime.Max)
\tif printDebug then print("DCG cadence", cadence, "playerId", myId, "waveNumber", waveNumber, "cooldownSeconds", cooldown / 1000) end
\tfirstPurchase = false
\treturn cooldown
end
'''
source = replace_once(source, old_cooldown, new_cooldown, 'spawn cooldown function')

source = replace_once(
    source,
    '''\t\t-- Post-opening allied support should be infantry-led without changing enemy doctrine logic.
\t\tif isAlliedDefenderBot and waveNumber > 0 then
\t\t\tif UnitType("Infantry") then
\t\t\t\tpriorityMultiplier = priorityMultiplier * 2.0
\t\t\telseif UnitType("Tank") or UnitType("Cannon") or UnitType("Artillery") or UnitType("Sortie") or UnitType("Air") or UnitType("Aircraft") then
\t\t\t\tpriorityMultiplier = priorityMultiplier * 0.35
\t\t\telse
\t\t\t\tpriorityMultiplier = priorityMultiplier * 0.65
\t\t\tend
\t\tend

''',
    '',
    'allied infantry bias',
)

source = replace_once(
    source,
    '\tif printDebug then print("DCG identity retry", "playerId", myId, "firstEnemyId", firstEnemyId, "defenderBotId", defenderBotId, "isAlliedDefenderBot", isAlliedDefenderBot) end\n',
    '\tif printDebug then print("DCG identity retry", "playerId", myId, "firstEnemyId", firstEnemyId, "defenderBotId", defenderBotId, "firstPlayerId", firstPlayerId) end\n',
    'identity retry diagnostic',
)

old_quant = '''function OnGameQuant()
\tretryMissionIdentityOnce()
\tlocal alliedSupportBlocked = isAlliedDefenderBot and waveNumber > 0 and not prepTimeOver
\tif alliedSupportBlocked then
\t\tif printDebug and not alliedPrepHoldLogged then
\t\t\talliedPrepHoldLogged = true
\t\t\tprint("DCG allied support held until PrepTimeOver", "playerId", myId, "waveNumber", waveNumber)
\t\tend
\telse
\t\tTrySpawnUnit()
\tend

\tlocal waypoints = BotApi.Scene.Waypoints
'''
new_quant = '''function OnGameQuant()
\tretryMissionIdentityOnce()
\tTrySpawnUnit()

\tlocal waypoints = BotApi.Scene.Waypoints
'''
source = replace_once(source, old_quant, new_quant, 'quant purchase gate')

source = replace_once(
    source,
    '\tif printDebug then print("DCG spawned squad", squad, "botPlayerId", myId, "defenderBotId", defenderBotId, "isAlliedDefenderBot", isAlliedDefenderBot, "waveNumber", waveNumber) end\n',
    '\tif printDebug then print("DCG spawned squad", squad, "botPlayerId", myId, "defenderBotId", defenderBotId, "waveNumber", waveNumber) end\n',
    'spawn diagnostic',
)

old_prep = '''function OnPrepTimeOver()
\tprepTimeOver = true
\talliedPrepHoldLogged = false
\tBotApi.Scene:SetVar("prep_inform", 1)
\tif printDebug then print("Print: prep_inform set to 1, Player defense prep is over.") end

\t-- Start the allied support clock now, not during the unpaused preparation phase.
\tif isAlliedDefenderBot and waveNumber > 0 and not waveSpawnActive then
\t\tKillSpawnCooldownTimer()
\t\tSetSpawnCooldownTimer()
\t\tif printDebug then print("DCG allied support released after preparation", "playerId", myId, "waveNumber", waveNumber) end
\tend

\t-- When player was defending, bot is attacker — release attack start for CE scripts.
'''
new_prep = '''function OnPrepTimeOver()
\tBotApi.Scene:SetVar("prep_inform", 1)
\tif printDebug then print("Print: prep_inform set to 1, Player defense prep is over.") end

\t-- When player was defending, bot is attacker — release attack start for CE scripts.
'''
source = replace_once(source, old_prep, new_prep, 'prep allied release')

for forbidden in (
    'isAlliedDefenderBot',
    'DCGWaveOffMin_AlliedSupport',
    'DCGWaveOffMax_AlliedSupport',
    'Min_AlliedSupport',
    'Max_AlliedSupport',
    'DCG allied support',
    'allied-prep-hold',
    'allied-opening',
):
    if forbidden in source:
        raise RuntimeError(f"dead DefenderBot purchase marker remains: {forbidden}")

CONQUEST.write_text(source, encoding="utf-8")

TESTS.write_text(
    '''from __future__ import annotations

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
        self.assertIn('cadence = "enemy-defender-opening"', self.source)
        self.assertIn('cadence = "enemy-attacker-opening"', self.source)
        self.assertIn('cadence = "enemy-defender"', self.source)
        self.assertIn('cadence = "enemy-attacker"', self.source)

    def test_normal_calculated_waves_keep_global_reduction(self) -> None:
        self.assertIn("local NormalWaveSizeScale = 0.85", self.source)
        self.assertIn(
            "rawWaveTotal * ActiveDifficultySettings.waveScale * NormalWaveSizeScale",
            self.source,
        )
        self.assertIn("waveUnitTotal = math.max(3", self.source)

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
        self.assertIn(
            "if wroteMissionVars then setDocVarsInNattorSpeak(currentDivision) end",
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
''',
    encoding="utf-8",
)

SELF.unlink()
WORKFLOW.unlink()
