require([[/script/multiplayer/modes/utility]])
require([[/script/multiplayer/modes/utility_ce]])

-- [1.5.6] Code:X Reversion
printDebug = true

Context.SpawnSeekTimer = Context.SpawnSeekTimer or {}

-- Time from start of match AI will wait before attempting to buy a unit.
-- Attacker first buy is near-instant: official preparationTime in campaign_capture_the_flag.set
-- now gates the defense prep phase (v1.064+). Do NOT stack another 7-8 min here.
StartSpawnTime = {
    -- Bot is defender
    DefenseMin = 0 * 60 * 1000, 
    DefenseMax = 0 * 60 * 1000,
    -- Bot is attacker (prep phase controls real delay when player defends)
    AttackMin = 1 * 1000, 
    AttackMax = 1 * 1000,
}

-- Time from last purchase AI will wait before attempting to buy a new unit.
SpawnCooldownTime = {
    DCGWaveOffMin = 2.25 * 60000,
    DCGWaveOffMax = 3.75 * 60000,
    DCGMin = 2 * 1000,
    DCGMax = 5 * 1000,
}

WaveUnit = {
    Min = 8,
    Max = 14,
}

-- Sets time limit AI will wait for a unit it has chosen to buy if the unit is not yet available
local UnitSpawnWaitTime = 1.0 * 60000 -- 1:30min (ms) 

-- Time delay for units to get a new move order after spawn move order. Loops.
local OrderRotationPeriod = 1.75 * 60000 -- 1:45 min (ms)
-- Re-issue a move shortly after spawn in case the first order is skipped/eaten.
local SpawnOrderNudgeDelay = 5 * 1000 -- 5s

botDefender = false
botDifficultyModifier = 0
enableWaveCounter = true



-- One conquest.lua runs per bot. Resolve engine-owned identities once per instance.
local myId = BotApi.Instance.playerId or 0
local firstEnemyId = 0
local defenderBotId = 0
local firstPlayerId = 0
local missionIdentityRetryPending = false

local function resolvePositiveId(primary, fallback)
	if primary and primary > 0 then return primary end
	if fallback and fallback > 0 then return fallback end
	return 0
end

local function refreshConquestIdentity()
	local conquest = BotApi.Conquest or {}
	myId = BotApi.Instance.playerId or 0
	firstEnemyId = resolvePositiveId(conquest.FirstEnemyId, BotApi.Instance.CampaignFirstEnemyId)
	defenderBotId = resolvePositiveId(conquest.DefenderBotId, BotApi.Instance.CampaignDefenderBotId)
	firstPlayerId = resolvePositiveId(conquest.FirstPlayerId, BotApi.Instance.CampaignFirstPlayerId)
end

local function isMissionAuthority()
	return firstEnemyId > 0 and myId == firstEnemyId
end

local function publishConquestIds()
	if firstEnemyId > 0 then BotApi.Scene:SetVar("id_1st_enemy", firstEnemyId) end
	if defenderBotId > 0 then BotApi.Scene:SetVar("id_defenderbot", defenderBotId) end
	if firstPlayerId > 0 then BotApi.Scene:SetVar("id_1st_player", firstPlayerId) end
end

-- Attack-side scripts need the physical side the enemy bot spawned on: the
-- dynamic campaign swaps attacker/defender spawns per mission instance, so a
-- static entry waypoint is never correct. utility.lua derives spawnSide from
-- BotApi.Instance.spawnPointName ("a1" -> "a"). One writer only: this is
-- published from the mission-authority branch alongside the perspective vars.
-- Must be a sibling of publishConquestIds (NOT nested). Nested scope made the
-- setVarsInMissionScript call resolve to nil and hard-crash enemy bot init.
local function publishEnemySpawnSide()
	local side = spawnSide
	if type(side) ~= "string" or side == "" then
		local sp = BotApi.Instance and BotApi.Instance.spawnPointName
		if type(sp) == "string" and #sp > 0 then
			side = string.sub(sp, 1, 1)
		end
	end
	local sideNum = 0
	if side == "a" or side == "A" then
		sideNum = 1
	elseif side == "b" or side == "B" then
		sideNum = 2
	end
	-- Always publish a number (never nil) so Scene:SetVar cannot native-fault.
	BotApi.Scene:SetVar("enemy_spawnside", sideNum)
	if printDebug then
		print("Print: enemy_spawnside published", sideNum, "rawSide", tostring(side), "spawnPoint", tostring(BotApi.Instance and BotApi.Instance.spawnPointName))
	end
end

local DifficultySettings = {
    easy = {
        waveScale = 0.85,
        waveGrowthScale = 1.00,
    },
    normal = {
        waveScale = 0.92,
        waveGrowthScale = 1.00,
    },
    hard = {
        waveScale = 1.06,
        waveGrowthScale = 1.00,
    },
    heroic = {
        waveScale = 1.12,
        waveGrowthScale = 1.00,
    },
}

local ActiveDifficultySettings = DifficultySettings.heroic

local function ApplyDifficultyScaling()
    local botDifficulty = BotApi.Instance.difficulty
    ActiveDifficultySettings = DifficultySettings[botDifficulty] or DifficultySettings.heroic

    if printDebug then
        print("difficulty waveScale =", ActiveDifficultySettings.waveScale, "waveGrowthScale =", ActiveDifficultySettings.waveGrowthScale)
    end
end

local conquestSpawnPointIndex = 0

-- Sequential bot spawns (v1.064+). Override default utility Spawn().
function GameModeSpawnUnit(unit, maxSquadSize)
	if BotApi.Commands.SpawnAt and BotApi.Commands:SpawnAt(unit, maxSquadSize, conquestSpawnPointIndex) then
		if setAiSpawnIndex then
			conquestSpawnPointIndex = setAiSpawnIndex(conquestSpawnPointIndex)
		else
			conquestSpawnPointIndex = conquestSpawnPointIndex + 1
		end
		return true
	end
	return BotApi.Commands:Spawn(unit, maxSquadSize)
end

local function isAttackerOrDefender()
	-- v1.064+: explicit Conquest API (replaces fragile teamSize > 1 heuristic)
	if BotApi.Conquest and BotApi.Conquest.Attacking ~= nil then
		botDefender = not BotApi.Conquest.Attacking
	else
		botDefender = teamSize > 1
	end
	refreshConquestIdentity()
	if printDebug then
		print("DCG role", "playerId", myId, "botDefender", botDefender, "firstEnemyId", firstEnemyId, "defenderBotId", defenderBotId, "firstPlayerId", firstPlayerId, "defenderBotPurchaseHost", false)
	end
end

local function setVarsInMissionScript()
	-- Stable Conquest IDs are perspective-neutral and may be published by every bot.
	publishConquestIds()
	if not isMissionAuthority() then return false end

	-- Everything below is enemy-bot perspective and must have one writer.
	BotApi.Scene:SetVar("user_is_defender", botDefender and 0 or 1)
	publishEnemySpawnSide()

	local botNation = BotApi.Instance.army
	local botDifficulty = BotApi.Instance.difficulty
	-- Keep in sync with dcg/player_nation side map (1 rusa .. 8 pol)
	local nationMap = { rusa = 1, ukr = 2, nato = 3, csa = 4, sov = 5, prc = 6, frg = 7, pol = 8,
		-- legacy / alias ids
		rus = 1, ger = 2, fin = 3, usa = 3, eng = 3, jap = 6 }
	local difficultyMap = { easy = 1, normal = 2, hard = 3, heroic = 4 }
	local spawnMap = { a = 1, b = 2}
	local playerSpawnNameMap = {
		a1 = 1, a2 = 2, a3 = 3, a4 = 4,
		b1 = 5, b2 = 6, b3 = 7, b4 = 8,
	}
	-- Opposite-alliance guess for MI when {type side} fails (West vs East).
	local eastNations = { rusa = true, sov = true, prc = true, pol = true, rus = true, jap = true }
	local westNations = { nato = true, ukr = true, csa = true, frg = true, usa = true, eng = true, ger = true, fin = true }

	BotApi.Scene:SetVar("bot_army", nationMap[botNation] or 0)
	-- Hint only: MI dcg/player_nation remains authority when side matches.
	-- If side detection fails, MI default uses bot_army to pick the opposite bloc.
	if eastNations[botNation] then
		BotApi.Scene:SetVar("user_nation_hint", 3) -- prefer NATO/West
	elseif westNations[botNation] then
		BotApi.Scene:SetVar("user_nation_hint", 1) -- prefer RUSA/East
	else
		BotApi.Scene:SetVar("user_nation_hint", 3)
	end
	BotApi.Scene:SetVar("bot_difficulty", difficultyMap[botDifficulty] or 0)
	BotApi.Scene:SetVar("bots_spawnside", spawnMap[spawnSide] or 0)

	local playerSpawn = BotApi.Conquest and BotApi.Conquest.PlayerSpawnPoint
	if not playerSpawn or playerSpawn == "" then playerSpawn = BotApi.Instance.spawnPointName end
	BotApi.Scene:SetVar("player_spawn_name", playerSpawnNameMap[playerSpawn] or 0)
	BotApi.Scene:SetVar("enemyid", myId)

	if botDefender then
		if difficultyMap[botDifficulty] == 4 then
			botDifficultyModifier = AiDefenderCount.Attacking.difficultyModifier.heroic
		elseif difficultyMap[botDifficulty] == 3 then
			botDifficultyModifier = AiDefenderCount.Attacking.difficultyModifier.hard
		elseif difficultyMap[botDifficulty] == 2 then
			botDifficultyModifier = AiDefenderCount.Attacking.difficultyModifier.normal
		else
			botDifficultyModifier = AiDefenderCount.Attacking.difficultyModifier.easy
		end
	else
		if difficultyMap[botDifficulty] == 4 then
			botDifficultyModifier = AiDefenderCount.Defending.difficultyModifier.heroic
		elseif difficultyMap[botDifficulty] == 3 then
			botDifficultyModifier = AiDefenderCount.Defending.difficultyModifier.hard
		elseif difficultyMap[botDifficulty] == 2 then
			botDifficultyModifier = AiDefenderCount.Defending.difficultyModifier.normal
		else
			botDifficultyModifier = AiDefenderCount.Defending.difficultyModifier.easy
		end
	end

	if printDebug then print("botDifficultyModifier = ", botDifficultyModifier) end
	SetCEMissionVariables(botDefender)
	return true
end

-- Each order tick: 50% scatter flank / 50% weighted CaptureFlag.
-- Flank uses CWA attack_support_flank_* / entry_* (never rear/air).
local FlankOrderChance = 0.50
local FlankWaypointChance = 0.55
local FlankFirstOrderChance = 0.45
local loggedWaypointNames = false
local waveSpawnPossible
local waveSpawnActive = true
local waveUnitCount = 0
local waveNumber = 0
local waveUnitTotal

local function rollWaveSize()
	local raw = math.random(WaveUnit.Min, WaveUnit.Max)
	local scale = (ActiveDifficultySettings and ActiveDifficultySettings.waveScale) or 1
	return math.max(1, math.floor(raw * scale + 0.5))
end

local function setDocVarsInNattorSpeak()
	BotApi.Scene:SetVar("ai_divisions", 0)
	BotApi.Scene:SetVar("bots_divisions", 0)
end

local usedOpeningArty = false
local lastWaveArty = -1

local function flagSlot(name)
	local n = tostring(name or ""):match("^f([1-5])$")
	return n and tonumber(n) or nil
end

local function publishLiveArtyFlags()
	local live = {0, 0, 0, 0, 0}
	local n = 0
	for _, flag in pairs(BotApi.Scene.Flags or {}) do
		local i = flagSlot(flag.name)
		if i then
			live[i] = 1
			n = n + 1
			if printDebug then print("DCG live flag", flag.name, "slot", i) end
		end
	end
	for i = 1, 5 do
		BotApi.Scene:SetVar("arty_f" .. i, live[i])
	end
	return n, live
end

local function requestOpeningArty()
	if usedOpeningArty then return end
	usedOpeningArty = true
	BotApi.Scene:SetVar("arty_intensity", 1)
	local n = publishLiveArtyFlags()
	if n < 1 then n = 1 end
	BotApi.Scene:SetVar("arty_prep_open", n)
	if printDebug then print("DCG arty_prep_open requested", n) end
end

local function requestWaveArty(wave)
	local w = wave
	if w < 1 then w = 1 end
	if lastWaveArty == w then return end
	lastWaveArty = w
	local _, live = publishLiveArtyFlags()
	local slots = {}
	for i = 1, 5 do
		if live[i] == 1 then slots[#slots + 1] = i end
	end
	local slot = (#slots > 0) and slots[math.random(#slots)] or 1
	BotApi.Scene:SetVar("arty_wave_slot", slot)
	BotApi.Scene:SetVar("arty_prep_wave", w)
	BotApi.Scene:SetVar("arty_smoke", 1)
	if printDebug then print("DCG arty_prep_wave requested", w, "slot", slot) end
end

local function startAmbientArty()
end

local function spawnParaDrop(kind, tries)
	if not isMissionAuthority or not isMissionAuthority() then return end
	tries = tries or 0
	local army = BotApi.Instance.army
	local west = (army == "nato" or army == "ukr" or army == "csa" or army == "frg")
	local paraKind = 1
	local unit = west and "c130_para_ai" or "il-76td_para_ai"
	if kind == "veh" then
		paraKind = 2
		unit = west and "c130_lav25_ai" or "il-76td_bmd2_ai"
	end
	local ok = GameModeSpawnUnit(unit, MaxSquadSize or 7)
	if not ok and kind == "veh" then
		paraKind = 1
		unit = west and "c130_para_ai" or "il-76td_para_ai"
		ok = GameModeSpawnUnit(unit, MaxSquadSize or 7)
	end
	if not ok then
		if tries < 8 then
			if printDebug then print("DCG codex_para retry", tries + 1) end
			BotApi.Events:SetQuantTimer(function() spawnParaDrop(kind, tries + 1) end, 4000)
		elseif printDebug then
			print("DCG codex_para spawn FAIL")
		end
		return
	end
	local _, live = publishLiveArtyFlags()
	local quiet = {}
	for i = 1, 5 do
		if live[i] ~= 1 then quiet[#quiet + 1] = i end
	end
	local lz
	local liveSlots = {}
	for i = 1, 5 do
		if live[i] == 1 then liveSlots[#liveSlots + 1] = i end
	end
	if #liveSlots > 0 then
		lz = liveSlots[math.random(#liveSlots)]
	else
		lz = 6 + math.random(1, 3)
	end
	BotApi.Scene:SetVar("codex_para_lz", lz)
	BotApi.Scene:SetVar("codex_para_kind", paraKind)
	BotApi.Scene:SetVar("codex_para", 1)
	if printDebug then print("DCG codex_para spawn", unit, "kind", paraKind, "lz", lz) end
end

local paraScheduled = false
local function scheduleParaDrops()
end

function WaveAttack()
	if not waveUnitTotal then
		waveUnitTotal = rollWaveSize()
	end
	if not botDefender then
		waveSpawnPossible = true
	end
	if waveSpawnPossible then
		if waveUnitCount >= waveUnitTotal then
			waveUnitTotal = rollWaveSize()
			waveSpawnActive = false
			waveUnitCount = 0
			waveNumber = waveNumber + 1
			if isMissionAuthority and isMissionAuthority() then
				BotApi.Scene:SetVar("wave_number", waveNumber)
				requestWaveArty(waveNumber)
			end
			if printDebug then print("Print: waveUnitTotal", waveUnitTotal, "waveNumber", waveNumber) end
		else
			if waveUnitCount == 0 and ActivateAiStrategy then
				waveUnitTotal = ActivateAiStrategy(waveUnitTotal)
			end
			waveSpawnActive = true
		end
	end
end

function WaveUnitCounter()
	if waveSpawnPossible then
		waveUnitCount = waveUnitCount + 1
		if printDebug then print("Print: waveUnitCount =", waveUnitCount) end
	end
end

function EndWaveOnPurchaseFail()
	if not waveSpawnPossible or not waveSpawnActive then return end
	if waveUnitCount <= 0 then return end
	waveSpawnActive = false
	waveUnitCount = 0
	waveNumber = waveNumber + 1
	if isMissionAuthority and isMissionAuthority() then
		BotApi.Scene:SetVar("wave_number", waveNumber)
		requestWaveArty(waveNumber)
	end
	if printDebug then print("DCG wave ended on purchase fail", "waveNumber", waveNumber) end
end

local firstPurchase = true
function GameModeSpawnCooldown()
	WaveAttack()
	local spawnTime
	if botDefender and firstPurchase then
		spawnTime = {Min = StartSpawnTime.DefenseMin, Max = StartSpawnTime.DefenseMax}
	elseif firstPurchase then
		spawnTime = {Min = StartSpawnTime.AttackMin, Max = StartSpawnTime.AttackMax}
	elseif not waveSpawnActive then
		spawnTime = {Min = SpawnCooldownTime.DCGWaveOffMin, Max = SpawnCooldownTime.DCGWaveOffMax}
	else
		spawnTime = {Min = SpawnCooldownTime.DCGMin, Max = SpawnCooldownTime.DCGMax}
	end
	local cooldown = math.random(spawnTime.Min, spawnTime.Max)
	if printDebug then print("DCG cadence", "playerId", myId, "waveNumber", waveNumber, "cooldownSeconds", cooldown / 1000) end
	firstPurchase = false
	return cooldown
end


function table.shuffle(tbl)
	local rand = math.random
	for i = #tbl, 2, -1 do
	  local j = rand(i)
	  tbl[i], tbl[j] = tbl[j], tbl[i]
	end
	return tbl
end
  
-- Function to shuffle the flags table
local function shuffleFlags(flags)
	if waveNumber <= 1 then
		table.sort(flags, function(a, b) return a.name < b.name end)
	else
		table.shuffle(flags)
	end
end

-- Function to calculate flag priority for attacker
-- NOTE: own flags must stay > 0 or GetRandomItem total becomes 0 and orders fail
-- (bot defenders often run with botDefender=false when teamSize==1).
local function calculateAttackerPriority(f, enemyTeam, team, firstEnemyFlagEncountered)
    if f.owner == enemyTeam and not firstEnemyFlagEncountered then
        firstEnemyFlagEncountered = true
        return f.priority, firstEnemyFlagEncountered
    elseif f.owner == enemyTeam then
        return f.priority, firstEnemyFlagEncountered
    elseif f.owner == team then
        return f.priority * 0.1, firstEnemyFlagEncountered
    end
    return f.priority, firstEnemyFlagEncountered
end

-- Function to calculate flag priority for defender
local function calculateDefenderPriority(f, enemyTeam, team)
    if f.owner == enemyTeam then
        return f.priority * 2
    elseif f.owner == team then
        return f.priority * 0.5
    end
    return f.priority
end

function GetFlagToCapture(flagPoints, getPriority, flags)
	local alliedFlags, opponentFlags, neutralFlags, totalFlags = CalculateFlagStatistics(BotApi.Scene.Flags)
	local capturableFlags = CalculateCapturableFlags(totalFlags, alliedFlags)

	PrintFlagDebugInfo(alliedFlags, opponentFlags, neutralFlags, totalFlags, capturableFlags, teamIsLosing)
	searchDestroy = CalculateSearchDestroyValue(capturableFlags, alliedFlags, opponentFlags)

	if waveNumber <= 1 then
        shuffleFlags(flags)
    end

	local firstEnemyFlagEncountered = false

	return GetRandomItem(flags, function(f)
		if not botDefender then
			local priority
			priority, firstEnemyFlagEncountered = calculateAttackerPriority(f, enemyTeam, team, firstEnemyFlagEncountered)
			return priority
		end
		return calculateDefenderPriority(f, enemyTeam, team)
	end)
end

function GetCurrentSpawnWaitTime()
    return UnitSpawnWaitTime
end

function GetUnitToSpawn(units)
	if not units then
		return nil
	end
	
	local unitsToSpawn = {}
	
	local income = BotApi.Commands:Income(BotApi.Instance.playerId)

	if printDebug then print("Player#".. BotApi.Instance.playerId.. " Units") end
	for i, unit in pairs(units) do
		local min_team = unit.min_team  -- not used
		local min_income = unit.min_income -- not used
		local available = BotApi.Commands:IsUnitAvailable(unit.unit)
		
		if not min_income then min_income = -1 end
		if not min_team then min_team = 0 end
		
		--if printDebug then print("------ Unit", unit.unit) end

		if teamSize >= min_team and income >= min_income and available then
			table.insert(unitsToSpawn, unit)
		end
	end

	-- TODO: instead of return nil, find the shortest tts and delay calling function again by that time 
	if #unitsToSpawn == 0 then
		return nil
	end

	local earlyLeftover = {
		["t_55m_fin"] = true,
		["leopard_1a5_n"] = true,
		["leopard_c2_mexas"] = true,
		["t-55a_mangal1"] = true,
		["t55_exp"] = true,
		["t-62_rus"] = true,
		["t62m1_rus1"] = true,
		["t72a"] = true,
		["t62m1_ukr"] = true,
		["t72a_ukr"] = true,
		["leopard_1a5_ukr"] = true,
		["ztz59"] = true,
		["ztz79"] = true,
		["ztz88a"] = true,
	}
	local modernMbt = {
		["leopord_2a4_ger"] = true,
		["leopord_2a6_n"] = true,
		["leopard_2a7+"] = true,
		["m1a1_n"] = true,
		["m1a2_sep_armor"] = true,
		["t90a_rus"] = true,
		["t90m"] = true,
		["t90m_2024"] = true,
		["t72b3"] = true,
		["leopord_2a4"] = true,
		["leopord_2a6"] = true,
		["m1a1"] = true,
		["t64bv2024"] = true,
		["ztz99a"] = true,
		["wz1001"] = true,
	}
	local modernUnlocked = false
	for _, entry in ipairs(unitsToSpawn) do
		if modernMbt[entry.unit] then
			modernUnlocked = true
			break
		end
	end

	searchProps = {
-- Human tags
		"soldier", 
		"crew", 
		"soldier_pzscheck",
		"soldier_pzfaust",
		"soldier_atr",
		"soldier_atr_grenade",
		"soldier_bazooka",
	}
	local sceneUnits = BotApi.Scene:QueryScene(searchProps, 5)

	local unitCounts = {
		BotInfantry = 0,
		BotATInfantry = 0,
		BotTanks = 0,
	}
	
	local propertyToVariable = {
	-- Humans
		["soldier"] = {"BotInfantry"},
		["soldier_pzscheck"] = {"BotInfantry", "BotATInfantry"},
		["soldier_pzfaust"] = {"BotInfantry", "BotATInfantry"},
		["soldier_atr"] = {"BotInfantry", "BotATInfantry"},
		["soldier_atr_grenade"] = {"BotInfantry", "BotATInfantry"},
		["soldier_bazooka"] = {"BotInfantry", "BotATInfantry"},
	}
	
	local botUnits = sceneUnits[BotApi.Instance.playerId][2]
	
	for i, prop in ipairs(searchProps) do
		local count = botUnits[i]
		local variables = propertyToVariable[prop]
		if variables then
			for _, variable in ipairs(variables) do
				unitCounts[variable] = unitCounts[variable] + count
			end
		end
	end

	return GetRandomItem(unitsToSpawn, function(t)
		local base = t.priority
		if GetUnitPriority then
			base = GetUnitPriority(t)
		end
		if modernUnlocked and earlyLeftover[t.unit] then
			base = base * 0.2
		end
		return base
	end)
end

local function ApplyWaveCadence()
	if WaveUnitOverride then
		if botDefender then
			WaveUnit.Min = WaveUnitOverride.DefendMin
			WaveUnit.Max = WaveUnitOverride.DefendMax
		else
			WaveUnit.Min = WaveUnitOverride.AttackMin
			WaveUnit.Max = WaveUnitOverride.AttackMax
		end
	end
	if DCGWaveOffOverwrite then
		if botDefender then
			SpawnCooldownTime.DCGWaveOffMin = DCGWaveOffOverwrite.DefenseMinWaveOff
			SpawnCooldownTime.DCGWaveOffMax = DCGWaveOffOverwrite.DefenseMaxWaveOff
		else
			SpawnCooldownTime.DCGWaveOffMin = DCGWaveOffOverwrite.AttackMinWaveOff
			SpawnCooldownTime.DCGWaveOffMax = DCGWaveOffOverwrite.AttackMaxWaveOff
		end
	end
	if printDebug then print("DCG WaveUnit", WaveUnit.Min, WaveUnit.Max, "waveOffMin", SpawnCooldownTime.DCGWaveOffMin / 1000) end
end

function OnGameStart()
	isAttackerOrDefender()
	ApplyDifficultyScaling()
	ApplyWaveCadence()
	CheckIfChallengeMap()
	local wroteMissionVars = setVarsInMissionScript()
	if wroteMissionVars then
		setDocVarsInNattorSpeak()
	elseif firstEnemyId <= 0 or defenderBotId <= 0 or firstPlayerId <= 0 then
		-- Retry once on the first quant: new Conquest IDs may settle after GameStart.
		missionIdentityRetryPending = true
	end
	OnGameStartUtility("conquest")
end

local function retryMissionIdentityOnce()
	if not missionIdentityRetryPending then return end
	missionIdentityRetryPending = false
	refreshConquestIdentity()
	local wroteMissionVars = setVarsInMissionScript()
	if wroteMissionVars then setDocVarsInNattorSpeak() end
	if printDebug then print("DCG identity retry", "playerId", myId, "firstEnemyId", firstEnemyId, "defenderBotId", defenderBotId, "firstPlayerId", firstPlayerId) end
end

-- Attack missions often never raise PrepTimeOver. Publish prep_inform once the
-- human is confirmed attacker so MI attack probes are not gated forever.
-- NOTE: must stay ABOVE OnGameQuant — a local defined after its caller resolves
-- to a nil global at call time and hard-crashes the bot on its first quant.
-- botDefender is THIS BOT's role: true means the bot defends, so the human is the
-- ATTACKER (SetVar("user_is_defender", botDefender and 0 or 1) right above, and
-- OnPrepTimeOver's "when player was defending, bot is attacker" branch uses
-- `not botDefender`). The early return therefore has to fire on `not botDefender`:
-- that is the human-DEFENCE mission, which runs a real 480s preparation phase and
-- must wait for OnPrepTimeOver. Publishing prep_inform there at the first quant
-- made every prep_inform consumer treat prep as already over at t=0 - it fired
-- dcg_script's dcg2/userdefend/prep_end during the player's own placement, and it
-- would let the defence-mission wave engines deploy into the prep phase.
local attackPrepInformPublished = false
local function ensureAttackPrepInform()
	if attackPrepInformPublished then return end
	if not botDefender then return end -- bot is attacker => human is defender; wait for real prep
	if not isMissionAuthority or not isMissionAuthority() then return end
	BotApi.Scene:SetVar("prep_inform", 1)
	attackPrepInformPublished = true
	startAmbientArty()
	scheduleParaDrops()
	if printDebug then print("Print: prep_inform set to 1 (human attack / no defense prep).") end
end

function OnGameQuant()
	retryMissionIdentityOnce()
	ensureAttackPrepInform()
	TrySpawnUnit()

	-- Always keep order timers (waypoint maps used to skip this and only got a one-shot move).
	for i, squad in pairs(BotApi.Scene.Squads) do
		if not Context.SquadTimers[squad] then
			SetSquadOrder(CaptureFlag, squad, OrderRotationPeriod)
		end
	end
end

function OnWaypoint(args)
	if not args or not args.squadId then return end
	if not BotApi.Scene:IsSquadExists(args.squadId) then return end
	-- Hand off to CaptureFlag loop so flanks/scatter apply after first waypoint.
	if not Context.SquadTimers[args.squadId] then
		SetSquadOrder(CaptureFlag, args.squadId, OrderRotationPeriod)
	else
		CaptureFlag(args.squadId)
	end
end

-- NOTE: Returns true if squad tagged "_lua_mi" / "repairing" / alert tags.
-- "_lua_alert" or "lua_alert" = squad abruptly runs into enemy force.
function IsSquadInScript(squad)
	if BotApi.Scene:IsSquadTagged(squad, "_lua_mi") or BotApi.Scene:IsSquadTagged(squad, "repairing") or BotApi.Scene:IsSquadTagged(squad, "aio_morale_owned") or BotApi.Scene:IsSquadTagged(squad, "aio_morale_surrendering") or BotApi.Scene:IsSquadTagged(squad, "aio_morale_surrender_evacuating") then
		if printDebug then print("Print: SQUADinSCRIPT thus no action squad", squad, "Player#",BotApi.Instance.playerId, "Team", team) end
		return true

	elseif BotApi.Scene:IsSquadTagged(squad, "_lua_alert") or BotApi.Scene:IsSquadTagged(squad, "lua_alert") then
		-- 60/40 SPLIT ON ENEMY CONTACT:
		-- 40%: SeekAndDestroy, 60%: hold/suppress (do nothing)
		if math.random() < 0.4 then
			BotApi.Commands:SeekAndDestroy(squad)
		else
			-- do nothing on purpose
		end
		return true
	end

	return false
end

-- MI/repair only — alert must not block a forced spawn kick.
local function IsSquadReserved(squad)
	return BotApi.Scene:IsSquadTagged(squad, "_lua_mi") or BotApi.Scene:IsSquadTagged(squad, "repairing") or BotApi.Scene:IsSquadTagged(squad, "aio_morale_owned") or BotApi.Scene:IsSquadTagged(squad, "aio_morale_surrendering") or BotApi.Scene:IsSquadTagged(squad, "aio_morale_surrender_evacuating")
end

	-- NOTE: Returns true if squad tagged "_lua_ignore" for general ignore.
function IsSquadToIgnore(squad)
	if BotApi.Scene:IsSquadTagged(squad, "_lua_ignore") then
		return true
	end
end

local function waypointName(wp)
	if type(wp) == "string" then return wp end
	if type(wp) == "table" then return wp.name or wp.id end
	return nil
end

local function collectNamedWaypoints(needles)
	local out = {}
	local wps = BotApi.Scene.Waypoints
	if not wps then return out end
	if printDebug and not loggedWaypointNames then
		loggedWaypointNames = true
		local names = {}
		for i = 1, #wps do names[i] = tostring(waypointName(wps[i]) or wps[i]) end
		print("DCG waypoints", table.concat(names, ","))
	end
	for i = 1, #wps do
		local name = waypointName(wps[i])
		if name then
			for n = 1, #needles do
				if string.find(name, needles[n], 1, true) then
					table.insert(out, name)
					break
				end
			end
		end
	end
	return out
end

local function pickFlankWaypoint()
	local side = spawnSide
	if type(side) ~= "string" or side == "" then side = "a" end
	side = string.lower(side)
	local flanks = collectNamedWaypoints({"attack_support_flank_" .. side})
	if #flanks == 0 then flanks = collectNamedWaypoints({"attack_support_flank_"}) end
	if #flanks > 0 then return flanks[math.random(#flanks)] end
	local entries = collectNamedWaypoints({"attack_support_entry_" .. side})
	if #entries > 0 then return entries[math.random(#entries)] end
	return nil
end

local function IssueScatterOrder(squad, flags, logTag)
	local candidates = {}
	for _, f in pairs(flags) do
		if f.owner ~= team then
			table.insert(candidates, f)
		end
	end

	local flank = pickFlankWaypoint()
	local preferWaypoint = flank and (#candidates == 0 or math.random() <= FlankWaypointChance)
	if preferWaypoint then
		if printDebug then print("Print:", logTag, "waypoint", flank, "squad", squad, "Player#", BotApi.Instance.playerId) end
		return BotApi.Commands:CaptureFlag(squad, flank)
	end

	if #candidates > 0 then
		local pick = candidates[math.random(#candidates)]
		if printDebug then print("Print:", logTag, "flag", pick.name, "squad", squad, "Player#", BotApi.Instance.playerId) end
		return BotApi.Commands:CaptureFlag(squad, pick.name)
	end

	if printDebug then print("Print:", logTag, "S&D fallback squad", squad, "Player#", BotApi.Instance.playerId) end
	BotApi.Commands:SeekAndDestroy(squad)
end

function CaptureFlag(squad)
    local flags = {}
    for i, flag in pairs(BotApi.Scene.Flags) do
        table.insert(flags, {id = i, name = flag.name, priority = getDefaultFlagPriority(flag), owner = flag.occupant})
    end

    local flag = GetFlagToCapture(BotApi.Scene.Flags, getDefaultFlagPriority, flags)

    if IsSquadInScript(squad) then return end

    if IsSquadToIgnore(squad) then
        if searchDestroy > math.random() then
            if printDebug then print("Print: [see_enemy] seek by squad ", squad, "Player#", BotApi.Instance.playerId) end
            BotApi.Commands:SeekAndDestroy(squad)
        else
            -- Was idle for full OrderRotationPeriod; give a real path instead.
            IssueScatterOrder(squad, flags, "[see_enemy] scatter")
        end
        return
    end

    -- 50/50 scatter flank vs weighted CaptureFlag every order tick.
    if math.random() <= FlankOrderChance then
        IssueScatterOrder(squad, flags, "[flank order]")
        return
    end

    if not flag then
        if printDebug then print("Print: No Flags so SeekAndDestroy by squad ", squad, "Player#", BotApi.Instance.playerId) end
        BotApi.Commands:SeekAndDestroy(squad)
        return
    end

    if printDebug then print("Print: [notags] ctf by squad", squad, "Player#", BotApi.Instance.playerId, "Flag name: ", flag.name) end
    return BotApi.Commands:CaptureFlag(squad, flag.name)
end


local function IsSquadActive(squad)
	return squad ~= nil and BotApi.Scene:IsSquadExists(squad)
end

local function ScheduleSpawnOrderNudge(squad)
	if Context.SpawnSeekTimer[squad] then
		BotApi.Events:KillQuantTimer(Context.SpawnSeekTimer[squad])
		Context.SpawnSeekTimer[squad] = nil
	end
	Context.SpawnSeekTimer[squad] = BotApi.Events:SetQuantTimer(function()
		Context.SpawnSeekTimer[squad] = nil
		if not IsSquadActive(squad) then return end
		if IsSquadReserved(squad) then return end
		if printDebug then print("Print: [spawn nudge] squad", squad, "Player#", BotApi.Instance.playerId) end
		-- Force a real path (ignore alert/ignore tags for this kick only).
		local flags = {}
		for i, flag in pairs(BotApi.Scene.Flags) do
			table.insert(flags, {id = i, name = flag.name, priority = getDefaultFlagPriority(flag), owner = flag.occupant})
		end
		IssueScatterOrder(squad, flags, "[spawn nudge]")
	end, SpawnOrderNudgeDelay)
end

function OnGameSpawn(args)
    if not args or not args.squadId then return end
    local squad = args.squadId
    if not IsSquadActive(squad) then return end
	if sceneVarRequested and not sceneVariableSquad then
		sceneVariableSquad = squad
		sceneVarRequested = false
		if printDebug then print("Spawned Scene variable successfully!", squad) end
		if StartSceneCheckTimer then StartSceneCheckTimer() end
		if SetGeneralSquadTagCheckTimer then SetGeneralSquadTagCheckTimer() end
		return
	end
	if printDebug then print("DCG spawned squad", squad, "botPlayerId", myId, "defenderBotId", defenderBotId, "waveNumber", waveNumber) end

	-- Only mark attack-started / rearrange spawns when the bot is the attacker.
	if not botDefender and not ai_attack_started then
        ai_attack_started = true
        BotApi.Scene:SetVar("ai_attack_started", 1)
        requestOpeningArty()
        if printDebug then print("AI has started their attack!") end
        SelectAiSpawnStrategy()
    end

	-- Always register the CaptureFlag order loop (scatter uses waypoints when present).
	-- Waypoint maps used to get a single move order at spawn and never re-order,
	-- which left squads standing at the spawn line for the rest of the match.
	if not botDefender and math.random() <= FlankFirstOrderChance then
		local flank = pickFlankWaypoint()
		if flank then
			BotApi.Commands:CaptureFlag(squad, flank)
			if printDebug then print("Print: [spawn flank]", flank, "squad", squad, "Player#", BotApi.Instance.playerId) end
		end
	end
	SetSquadOrder(CaptureFlag, squad, OrderRotationPeriod)
	ScheduleSpawnOrderNudge(squad)
end

local emptyFieldKickTimer = nil
local function scheduleEmptyFieldSpawnKick()
	if emptyFieldKickTimer then return end
	emptyFieldKickTimer = BotApi.Events:SetQuantTimer(function()
		emptyFieldKickTimer = nil
		if botDefender then return end
		if isMissionAuthority and not isMissionAuthority() then return end
		local n = 0
		for _, squad in pairs(BotApi.Scene.Squads or {}) do
			if BotApi.Scene:IsSquadExists(squad) then
				n = n + 1
			end
		end
		if n == 0 then
			waveSpawnPossible = true
			waveSpawnActive = true
			if KillSpawnCooldownTimer then KillSpawnCooldownTimer() end
			if KillSpawnWaitTimer then KillSpawnWaitTimer() end
			if printDebug then print("DCG empty-field spawn kick") end
		end
		scheduleEmptyFieldSpawnKick()
	end, 45 * 1000)
end

-- v1.064+: prep phase ended (timer or Skip Preparation). Mission scripts key off prep_inform.
function OnPrepTimeOver()
	BotApi.Scene:SetVar("prep_inform", 1)
	startAmbientArty()
	if printDebug then print("Print: prep_inform set to 1, Player defense prep is over.") end

	-- When player was defending, bot is attacker — release attack start for CE scripts.
	if not botDefender and not ai_attack_started then
		ai_attack_started = true
		BotApi.Scene:SetVar("ai_attack_started", 1)
		requestOpeningArty()
		if printDebug then print("AI attack released after prep time.") end
		if SelectAiSpawnStrategy then SelectAiSpawnStrategy() end
		scheduleParaDrops()
		scheduleEmptyFieldSpawnKick()
	end
end

BotApi.Events:Subscribe(BotApi.Events.GameStart, OnGameStart)
BotApi.Events:Subscribe(BotApi.Events.GameEnd, OnGameStop)
BotApi.Events:Subscribe(BotApi.Events.Quant, OnGameQuant)
BotApi.Events:Subscribe(BotApi.Events.GameSpawn, OnGameSpawn)
BotApi.Events:Subscribe(BotApi.Events.Waypoint, OnWaypoint)
if BotApi.Events.PrepTimeOver then
	BotApi.Events:Subscribe(BotApi.Events.PrepTimeOver, OnPrepTimeOver)
end
