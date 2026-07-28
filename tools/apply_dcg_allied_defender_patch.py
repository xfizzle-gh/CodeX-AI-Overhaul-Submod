from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONQUEST = ROOT / "resource/script/multiplayer/modes/conquest.lua"
WORKFLOW = ROOT / ".github/workflows/apply-dcg-allied-defender-patch.yml"
SELF = Path(__file__).resolve()

source = CONQUEST.read_text(encoding="utf-8")


def replace_once(old: str, new: str) -> None:
    global source
    count = source.count(old)
    if count != 1:
        raise RuntimeError(f"expected one literal match, found {count}: {old[:80]!r}")
    source = source.replace(old, new, 1)


def sub_once(pattern: str, replacement: str) -> None:
    global source
    source, count = re.subn(pattern, replacement, source, count=1, flags=re.DOTALL)
    if count != 1:
        raise RuntimeError(f"expected one regex match, found {count}: {pattern[:100]!r}")


replace_once(
    "    DCGWaveOffMax_Attacker = 3.5 * 60 * 1000,\n   -- Time between each spawn",
    "    DCGWaveOffMax_Attacker = 3.5 * 60 * 1000,\n"
    "    -- Time between allied DefenderBot support waves after preparation\n"
    "    DCGWaveOffMin_AlliedSupport = 4 * 60 * 1000,\n"
    "    DCGWaveOffMax_AlliedSupport = 8 * 60 * 1000,\n"
    "   -- Time between each spawn",
)

replace_once(
    "    Max_Attacker = 5,\n}",
    "    Max_Attacker = 5,\n"
    "    -- Allied DefenderBot follow-up support only; opening wave uses defender values\n"
    "    Min_AlliedSupport = 1,\n"
    "    Max_AlliedSupport = 3,\n"
    "}",
)

replace_once(
    "enableWaveCounter = true\n",
    "enableWaveCounter = true\n\n"
    "-- One conquest.lua runs per bot. Resolve engine-owned identities once per instance.\n"
    "local myId = BotApi.Instance.playerId or 0\n"
    "local firstEnemyId = 0\n"
    "local defenderBotId = 0\n"
    "local firstPlayerId = 0\n"
    "local isAlliedDefenderBot = false\n"
    "local prepTimeOver = not (BotApi.Events and BotApi.Events.PrepTimeOver)\n"
    "local missionIdentityRetryPending = false\n"
    "local alliedPrepHoldLogged = false\n\n"
    "local function resolvePositiveId(primary, fallback)\n"
    "\tif primary and primary > 0 then return primary end\n"
    "\tif fallback and fallback > 0 then return fallback end\n"
    "\treturn 0\n"
    "end\n\n"
    "local function refreshConquestIdentity()\n"
    "\tlocal conquest = BotApi.Conquest or {}\n"
    "\tmyId = BotApi.Instance.playerId or 0\n"
    "\tfirstEnemyId = resolvePositiveId(conquest.FirstEnemyId, BotApi.Instance.CampaignFirstEnemyId)\n"
    "\tdefenderBotId = resolvePositiveId(conquest.DefenderBotId, BotApi.Instance.CampaignDefenderBotId)\n"
    "\tfirstPlayerId = resolvePositiveId(conquest.FirstPlayerId, BotApi.Instance.CampaignFirstPlayerId)\n"
    "\tisAlliedDefenderBot = defenderBotId > 0 and myId == defenderBotId\n"
    "end\n\n"
    "local function isMissionAuthority()\n"
    "\treturn firstEnemyId > 0 and myId == firstEnemyId\n"
    "end\n\n"
    "local function publishConquestIds()\n"
    "\tif firstEnemyId > 0 then BotApi.Scene:SetVar(\"id_1st_enemy\", firstEnemyId) end\n"
    "\tif defenderBotId > 0 then BotApi.Scene:SetVar(\"id_defenderbot\", defenderBotId) end\n"
    "\tif firstPlayerId > 0 then BotApi.Scene:SetVar(\"id_1st_player\", firstPlayerId) end\n"
    "end\n",
)

sub_once(
    r"local function isAttackerOrDefender\(\).*?\nend\n\nlocal function setVarsInMissionScript",
    "local function isAttackerOrDefender()\n"
    "\t-- v1.064+: explicit Conquest API (replaces fragile teamSize > 1 heuristic)\n"
    "\tif BotApi.Conquest and BotApi.Conquest.Attacking ~= nil then\n"
    "\t\tbotDefender = not BotApi.Conquest.Attacking\n"
    "\telse\n"
    "\t\tbotDefender = teamSize > 1\n"
    "\tend\n"
    "\trefreshConquestIdentity()\n"
    "\tif printDebug then\n"
    "\t\tprint(\"DCG role\", \"playerId\", myId, \"botDefender\", botDefender, \"firstEnemyId\", firstEnemyId, \"defenderBotId\", defenderBotId, \"firstPlayerId\", firstPlayerId, \"isAlliedDefenderBot\", isAlliedDefenderBot)\n"
    "\tend\n"
    "end\n\n"
    "local function setVarsInMissionScript",
)

sub_once(
    r"local function setVarsInMissionScript\(\).*?\nend\n\n-- Each order tick",
    "local function setVarsInMissionScript()\n"
    "\t-- Stable Conquest IDs are perspective-neutral and may be published by every bot.\n"
    "\tpublishConquestIds()\n"
    "\tif not isMissionAuthority() then return false end\n\n"
    "\t-- Everything below is enemy-bot perspective and must have one writer.\n"
    "\tBotApi.Scene:SetVar(\"user_is_defender\", botDefender and 0 or 1)\n\n"
    "\tlocal botNation = BotApi.Instance.army\n"
    "\tlocal botDifficulty = BotApi.Instance.difficulty\n"
    "\tlocal nationMap = { rusa = 1, ukr = 2, nato = 3, csa = 4, sov = 5, prc = 6, frg = 7}\n"
    "\tlocal difficultyMap = { easy = 1, normal = 2, hard = 3, heroic = 4 }\n"
    "\tlocal spawnMap = { a = 1, b = 2}\n"
    "\tlocal playerSpawnNameMap = {\n"
    "\t\ta1 = 1, a2 = 2, a3 = 3, a4 = 4,\n"
    "\t\tb1 = 5, b2 = 6, b3 = 7, b4 = 8,\n"
    "\t}\n\n"
    "\tBotApi.Scene:SetVar(\"bot_army\", nationMap[botNation] or 0)\n"
    "\tBotApi.Scene:SetVar(\"bot_difficulty\", difficultyMap[botDifficulty] or 0)\n"
    "\tBotApi.Scene:SetVar(\"bots_spawnside\", spawnMap[spawnSide] or 0)\n\n"
    "\tlocal playerSpawn = BotApi.Conquest and BotApi.Conquest.PlayerSpawnPoint\n"
    "\tif not playerSpawn or playerSpawn == \"\" then playerSpawn = BotApi.Instance.spawnPointName end\n"
    "\tBotApi.Scene:SetVar(\"player_spawn_name\", playerSpawnNameMap[playerSpawn] or 0)\n"
    "\tBotApi.Scene:SetVar(\"enemyid\", myId)\n\n"
    "\tif botDefender then\n"
    "\t\tif difficultyMap[botDifficulty] == 4 then\n"
    "\t\t\tbotDifficultyModifier = AiDefenderCount.Attacking.difficultyModifier.heroic\n"
    "\t\telseif difficultyMap[botDifficulty] == 3 then\n"
    "\t\t\tbotDifficultyModifier = AiDefenderCount.Attacking.difficultyModifier.hard\n"
    "\t\telseif difficultyMap[botDifficulty] == 2 then\n"
    "\t\t\tbotDifficultyModifier = AiDefenderCount.Attacking.difficultyModifier.normal\n"
    "\t\telse\n"
    "\t\t\tbotDifficultyModifier = AiDefenderCount.Attacking.difficultyModifier.easy\n"
    "\t\tend\n"
    "\telse\n"
    "\t\tif difficultyMap[botDifficulty] == 4 then\n"
    "\t\t\tbotDifficultyModifier = AiDefenderCount.Defending.difficultyModifier.heroic\n"
    "\t\telseif difficultyMap[botDifficulty] == 3 then\n"
    "\t\t\tbotDifficultyModifier = AiDefenderCount.Defending.difficultyModifier.hard\n"
    "\t\telseif difficultyMap[botDifficulty] == 2 then\n"
    "\t\t\tbotDifficultyModifier = AiDefenderCount.Defending.difficultyModifier.normal\n"
    "\t\telse\n"
    "\t\t\tbotDifficultyModifier = AiDefenderCount.Defending.difficultyModifier.easy\n"
    "\t\tend\n"
    "\tend\n\n"
    "\tif printDebug then print(\"botDifficultyModifier = \", botDifficultyModifier) end\n"
    "\tSetCEMissionVariables(botDefender)\n"
    "\treturn true\n"
    "end\n\n"
    "-- Each order tick",
)

replace_once("local waveSpawnPossible\n", "local waveSpawnPossible = true\n")
replace_once("\nsetDocVarsInNattorSpeak(currentDivision)\n\n", "\n")

sub_once(
    r"function calculateWaveUnitTotal\(\).*?\nend\n\nfunction WaveAttack",
    "function calculateWaveUnitTotal()-- (currentDivision, waveNumber, botDefender)\n"
    "\t-- Allied DefenderBot keeps the normal opening force, then switches to small support waves.\n"
    "\tif isAlliedDefenderBot and waveNumber > 0 then\n"
    "\t\twaveUnitTotal = math.random(WaveUnit.Min_AlliedSupport, WaveUnit.Max_AlliedSupport)\n"
    "\t\tif printDebug then print(\"DCG allied support wave size\", \"playerId\", myId, \"waveNumber\", waveNumber, \"waveUnitTotal\", waveUnitTotal) end\n"
    "\t\treturn\n"
    "\tend\n\n"
    "\tlocal ExtraUnitsValue = math.round((waveNumberExtraUnits[waveNumber] or 0) * ActiveDifficultySettings.waveGrowthScale)\n"
    "\tlocal divisionParams = divisions[currentDivision]\n"
    "\tlocal rawWaveTotal\n\n"
    "\tif botDefender then\n"
    "\t\trawWaveTotal = math.random(WaveUnit.Min_Defender, WaveUnit.Max_Defender) + divisionParams.defenderMultiplier + ExtraUnitsValue\n"
    "\telse\n"
    "\t\trawWaveTotal = math.random(WaveUnit.Min_Attacker, WaveUnit.Max_Attacker) + divisionParams.attackerMultiplier + math.round(ExtraUnitsValue/2)\n"
    "\tend\n\n"
    "\twaveUnitTotal = math.max(3, math.round(rawWaveTotal * ActiveDifficultySettings.waveScale))\n"
    "\tif printDebug then print(\"Print: waveUnitTotal\", waveUnitTotal, \"waveNumber\", waveNumber, \"isAlliedDefenderBot\", isAlliedDefenderBot) end\n"
    "end\n\n"
    "function WaveAttack",
)

sub_once(
    r"function WaveAttack\(\).*?\nend\n\nfunction WaveUnitCounter",
    "function WaveAttack()\n"
    "\tif not waveUnitTotal then calculateWaveUnitTotal() end\n"
    "\twaveSpawnPossible = true\n\n"
    "\tif waveUnitCount >= waveUnitTotal then\n"
    "\t\twaveSpawnActive = false\n"
    "\t\twaveUnitCount = 0\n"
    "\t\twaveNumber = waveNumber + 1\n"
    "\t\tcalculateWaveUnitTotal()\n"
    "\t\tif printDebug then print(\"Print: waveNumber\", waveNumber, \"SelectedDivision\", currentDivision, \"isAlliedDefenderBot\", isAlliedDefenderBot) end\n"
    "\telse\n"
    "\t\twaveSpawnActive = true\n"
    "\tend\n"
    "end\n\n"
    "function WaveUnitCounter",
)

sub_once(
    r"local firstPurchase = true\nfunction GameModeSpawnCooldown\(\).*?\nend\n\nfunction table.shuffle",
    "local firstPurchase = true\n"
    "function GameModeSpawnCooldown()\n"
    "\tWaveAttack()\n"
    "\tlocal spawnTime\n"
    "\tlocal cadence = \"within-wave\"\n\n"
    "\tif botDefender and firstPurchase then\n"
    "\t\tspawnTime = {Min = StartSpawnTime.DefenseMin, Max = StartSpawnTime.DefenseMax}\n"
    "\t\tcadence = isAlliedDefenderBot and \"allied-opening\" or \"enemy-defender-opening\"\n"
    "\telseif firstPurchase then\n"
    "\t\tspawnTime = {Min = StartSpawnTime.AttackMin, Max = StartSpawnTime.AttackMax}\n"
    "\t\tcadence = \"enemy-attacker-opening\"\n"
    "\telseif not waveSpawnActive then\n"
    "\t\tif isAlliedDefenderBot then\n"
    "\t\t\tif prepTimeOver then\n"
    "\t\t\t\tspawnTime = {Min = SpawnCooldownTime.DCGWaveOffMin_AlliedSupport, Max = SpawnCooldownTime.DCGWaveOffMax_AlliedSupport}\n"
    "\t\t\t\tcadence = \"allied-support\"\n"
    "\t\t\telse\n"
    "\t\t\t\t-- OnGameQuant blocks the purchase; PrepTimeOver starts the real 4-8 minute timer.\n"
    "\t\t\t\tspawnTime = {Min = 1 * 1000, Max = 1 * 1000}\n"
    "\t\t\t\tcadence = \"allied-prep-hold\"\n"
    "\t\t\tend\n"
    "\t\telseif botDefender then\n"
    "\t\t\tspawnTime = {Min = SpawnCooldownTime.DCGWaveOffMin_Defender, Max = SpawnCooldownTime.DCGWaveOffMax_Defender}\n"
    "\t\t\tcadence = \"enemy-defender\"\n"
    "\t\telse\n"
    "\t\t\tspawnTime = {Min = SpawnCooldownTime.DCGWaveOffMin_Attacker, Max = SpawnCooldownTime.DCGWaveOffMax_Attacker}\n"
    "\t\t\tcadence = \"enemy-attacker\"\n"
    "\t\tend\n"
    "\telse\n"
    "\t\tspawnTime = {Min = SpawnCooldownTime.DCGMin, Max = SpawnCooldownTime.DCGMax}\n"
    "\tend\n\n"
    "\tlocal cooldown = math.random(spawnTime.Min, spawnTime.Max)\n"
    "\tif printDebug then print(\"DCG cadence\", cadence, \"playerId\", myId, \"waveNumber\", waveNumber, \"cooldownSeconds\", cooldown / 1000) end\n"
    "\tfirstPurchase = false\n"
    "\treturn cooldown\n"
    "end\n\n"
    "function table.shuffle",
)

replace_once(
    "\t\treturn basePriority * priorityMultiplier\n",
    "\t\t-- Post-opening allied support should be infantry-led without changing enemy doctrine logic.\n"
    "\t\tif isAlliedDefenderBot and waveNumber > 0 then\n"
    "\t\t\tif UnitType(\"Infantry\") then\n"
    "\t\t\t\tpriorityMultiplier = priorityMultiplier * 2.0\n"
    "\t\t\telseif UnitType(\"Tank\") or UnitType(\"Cannon\") or UnitType(\"Artillery\") or UnitType(\"Sortie\") or UnitType(\"Air\") or UnitType(\"Aircraft\") then\n"
    "\t\t\t\tpriorityMultiplier = priorityMultiplier * 0.35\n"
    "\t\t\telse\n"
    "\t\t\t\tpriorityMultiplier = priorityMultiplier * 0.65\n"
    "\t\t\tend\n"
    "\t\tend\n\n"
    "\t\treturn basePriority * priorityMultiplier\n",
)

sub_once(
    r"function OnGameStart\(\).*?\nend\n\nfunction OnGameQuant\(\).*?\nend\n\nfunction GotoNextWaypoint",
    "function OnGameStart()\n"
    "\tisAttackerOrDefender()\n"
    "\tApplyDifficultyScaling()\n"
    "\tCheckIfChallengeMap()\n"
    "\tlocal wroteMissionVars = setVarsInMissionScript()\n"
    "\tif wroteMissionVars then\n"
    "\t\tsetDocVarsInNattorSpeak(currentDivision)\n"
    "\telseif firstEnemyId <= 0 then\n"
    "\t\tmissionIdentityRetryPending = true\n"
    "\tend\n"
    "\tOnGameStartUtility(\"conquest\")\n"
    "end\n\n"
    "local function retryMissionIdentityOnce()\n"
    "\tif not missionIdentityRetryPending then return end\n"
    "\tmissionIdentityRetryPending = false\n"
    "\trefreshConquestIdentity()\n"
    "\tlocal wroteMissionVars = setVarsInMissionScript()\n"
    "\tif wroteMissionVars then setDocVarsInNattorSpeak(currentDivision) end\n"
    "\tif printDebug then print(\"DCG identity retry\", \"playerId\", myId, \"firstEnemyId\", firstEnemyId, \"defenderBotId\", defenderBotId, \"isAlliedDefenderBot\", isAlliedDefenderBot) end\n"
    "end\n\n"
    "function OnGameQuant()\n"
    "\tretryMissionIdentityOnce()\n"
    "\tlocal alliedSupportBlocked = isAlliedDefenderBot and waveNumber > 0 and not prepTimeOver\n"
    "\tif alliedSupportBlocked then\n"
    "\t\tif printDebug and not alliedPrepHoldLogged then\n"
    "\t\t\talliedPrepHoldLogged = true\n"
    "\t\t\tprint(\"DCG allied support held until PrepTimeOver\", \"playerId\", myId, \"waveNumber\", waveNumber)\n"
    "\t\tend\n"
    "\telse\n"
    "\t\tTrySpawnUnit()\n"
    "\tend\n\n"
    "\tlocal waypoints = BotApi.Scene.Waypoints\n"
    "\tif #waypoints == 0 then\n"
    "\t\tfor i, squad in pairs(BotApi.Scene.Squads) do\n"
    "\t\t\tif not Context.SquadTimers[squad] then\n"
    "\t\t\t\tSetSquadOrder(CaptureFlag, squad, OrderRotationPeriod)\n"
    "\t\t\tend\n"
    "\t\tend\n"
    "\tend\n"
    "end\n\n"
    "function GotoNextWaypoint",
)

replace_once(
    "    if not IsSquadActive(squad) then return end\n\n\t-- Only mark attack-started",
    "    if not IsSquadActive(squad) then return end\n"
    "\tif printDebug then print(\"DCG spawned squad\", squad, \"botPlayerId\", myId, \"defenderBotId\", defenderBotId, \"isAlliedDefenderBot\", isAlliedDefenderBot, \"waveNumber\", waveNumber) end\n\n"
    "\t-- Only mark attack-started",
)

sub_once(
    r"function OnPrepTimeOver\(\).*?\nend\n\nBotApi.Events:Subscribe",
    "function OnPrepTimeOver()\n"
    "\tprepTimeOver = true\n"
    "\talliedPrepHoldLogged = false\n"
    "\tBotApi.Scene:SetVar(\"prep_inform\", 1)\n"
    "\tif printDebug then print(\"Print: prep_inform set to 1, Player defense prep is over.\") end\n\n"
    "\t-- Start the allied support clock now, not during the unpaused preparation phase.\n"
    "\tif isAlliedDefenderBot and waveNumber > 0 and not waveSpawnActive then\n"
    "\t\tKillSpawnCooldownTimer()\n"
    "\t\tSetSpawnCooldownTimer()\n"
    "\t\tif printDebug then print(\"DCG allied support released after preparation\", \"playerId\", myId, \"waveNumber\", waveNumber) end\n"
    "\tend\n\n"
    "\t-- When player was defending, bot is attacker — release attack start for CE scripts.\n"
    "\tif not botDefender and not ai_attack_started then\n"
    "\t\tai_attack_started = true\n"
    "\t\tBotApi.Scene:SetVar(\"ai_attack_started\", 1)\n"
    "\t\tif printDebug then print(\"AI attack released after prep time.\") end\n"
    "\t\tif SelectAiSpawnStrategy then SelectAiSpawnStrategy() end\n"
    "\tend\n"
    "end\n\n"
    "BotApi.Events:Subscribe",
)

required = [
    "isAlliedDefenderBot = defenderBotId > 0 and myId == defenderBotId",
    "waveSpawnPossible = true",
    "Min_AlliedSupport = 1",
    "Max_AlliedSupport = 3",
    "DCGWaveOffMin_AlliedSupport = 4 * 60 * 1000",
    "DCGWaveOffMax_AlliedSupport = 8 * 60 * 1000",
    "if isAlliedDefenderBot and waveNumber > 0 then",
    "if not isMissionAuthority() then return false end",
    "DCG allied support held until PrepTimeOver",
    'UnitType("Infantry")',
    "BotApi.Commands:SpawnAt",
]
for needle in required:
    if needle not in source:
        raise RuntimeError(f"missing required patched source: {needle}")

for forbidden in [
    "if not botDefender or botDefender then",
    "setDocVarsInNattorSpeak(currentDivision)\n\nlocal waveNumberExtraUnits",
    "Conquest.DefenderBotId) or BotApi.Instance.CampaignDefenderBotId",
    "control user",
]:
    if forbidden in source:
        raise RuntimeError(f"forbidden source remains: {forbidden}")

CONQUEST.write_text(source, encoding="utf-8", newline="\n")

# Remove the one-shot patch machinery from the final branch diff.
if WORKFLOW.exists():
    WORKFLOW.unlink()
if SELF.exists():
    SELF.unlink()
