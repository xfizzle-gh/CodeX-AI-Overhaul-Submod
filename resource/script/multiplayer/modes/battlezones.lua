require([[/script/multiplayer/modes/utility]])

-- Time from start of match AI will wait before attempting to buy a unit.
local StartSpawnTime = {
	Min = 0.5 * 1000, -- 0.5 sec (ms)
	Max = 1 * 1000 -- 1 sec (ms)
}
 -- Time from last purchase AI will wait before attempting to buy a new unit.
local SpawnCooldownTime = {
	Min = 10 * 1000, -- 10 sec (ms)
	Max = 11 * 1000, -- 11 sec (ms)
}

-- Sets time limit AI will wait for a unit it has chosen to buy if the unit is not yet available
local UnitSpawnWaitTime = 1.5 * 60000 -- 1:30min (ms) 

-- Time delay for units to get a new move order after spawn move order. Loops.
local OrderRotationPeriod = 2.5 * 60000 -- 2:30 min (ms)

local firstPurchase = 2

function GameModeSpawnCooldown()
    if firstPurchase > 0 then
        firstPurchase = firstPurchase - 1
		return math.random(StartSpawnTime.Min, StartSpawnTime.Max)
	end

	return math.random(SpawnCooldownTime.Min, SpawnCooldownTime.Max)
end

flagLocationMap = {
	["battle_zones"] = {
		f1 = FlagLocation.Center, --A--
		f2 = function(side) return side == 'a' and FlagLocation.Friendly or FlagLocation.Enemy end, --B--
		f3 = function(side) return side == 'a' and FlagLocation.Enemy or FlagLocation.Friendly end, --C--
		f4 = FlagLocation.Center, --D--
		f5 = FlagLocation.Center, --E--
		f6 = FlagLocation.Center, --F--
		f7 = FlagLocation.Center, --G--
		f8 = function(side) return side == 'a' and FlagLocation.Enemy or FlagLocation.Friendly end, --H--
		f9 = function(side) return side == 'a' and FlagLocation.Friendly or FlagLocation.Enemy end, --I--

		center1 = FlagLocation.Center,
		center2 = FlagLocation.Center,
		center3 = FlagLocation.Center,
		center4 = FlagLocation.Center,
		center5 = FlagLocation.Center,
		team_a_back1 = function(side) return side == 'a' and FlagLocation.Friendly or FlagLocation.Enemy end,
		team_a_back2 = function(side) return side == 'a' and FlagLocation.Friendly or FlagLocation.Enemy end,
		team_a_back3 = function(side) return side == 'a' and FlagLocation.Friendly or FlagLocation.Enemy end,
		team_b_back1 = function(side) return side == 'a' and FlagLocation.Enemy or FlagLocation.Friendly end,
		team_b_back2 = function(side) return side == 'a' and FlagLocation.Enemy or FlagLocation.Friendly end,
		team_b_back3 = function(side) return side == 'a' and FlagLocation.Enemy or FlagLocation.Friendly end,
		["default"] = 1,
	},
	["ammunition"] = {
		base_flag_a = function(side) return side == 'a' and FlagLocation.FriendlyBase or FlagLocation.EnemyBase end,
		base_flag_b = function(side) return side == 'a' and FlagLocation.EnemyBase or FlagLocation.FriendlyBase end,
		["default"] = FlagLocation.Center,
	},
}

-- Count number of flags in the team's backfield
local function countBackFlags()
	local backFlag = 0
	for i, flag in pairs(BotApi.Scene.Flags) do
		local flagLocation = LookupFlagLocation(flag, flagLocationMap, spawnSide)
        if flagLocation == FlagLocation.Friendly then
            backFlag = backFlag + 1
        end
	end
    --if printDebug then print("backFlag:", backFlag) end
	-- Some maps are only middle flags
	if backFlag > 0 then
		captureBackFlag = true
  	end
	-- In larger team games, its not necessary for every bot to prioritize sending a squad to the "back flag"
	local player = BotApi.Instance.playerId
	if teamSize >= 2 and backFlag < 3 then
		if player % 2 == 1 then
			captureBackFlag = false
		end
	end
end

-- Function to determine if the team is losing
local function isTeamLosing(alliedFlags, opponentFlags, neutralFlags, totalFlags)
    if opponentFlags > alliedFlags and neutralFlags ~= totalFlags then
        return true
    else
        return false
    end
end

-- Function to calculate flag priority for selection
local function calculateFlagPriority(f, team, enemyTeam, teamIsLosing, totalFlags)
    if captureBackFlag then
        if f.location == "friendly" and f.owner ~= team then
            return f.priority
        else
            return 0
        end
    end

    if gameMode == "battle_zones" then
        if f.location == "friendly" and f.owner == enemyTeam then
            return f.priority * 6
        end
        if f.location == "friendly" and f.owner ~= enemyTeam then
            return f.priority * 0.25
        end
        if f.location == "center" and f.owner == team then
            return f.priority * 0.75
        end
        if f.location == "enemy" and f.owner == team then
            return f.priority * 4
        end

        if f.location == "enemy" and teamIsLosing and totalFlags > 3 then
            return 0
        elseif f.location == "enemy" and teamIsLosing and totalFlags == 3 then
            return f.priority * 3
        end
        if f.location == "enemy" and not teamIsLosing and f.owner == "enemy" then
            return f.priority * 2
        end
    end

    if gameMode == "ammunition" then
        if f.owner == team and f.location == "center" then
            return f.priority * 0.75
        end
        if f.owner == team and f.location == "enemy" then
            return f.priority * 3
        end
        if f.owner == enemyTeam and f.location == "friendly" then
            return f.priority * 99
        end
    end

    return f.priority
end

-- Main function to get the flag to capture
function GetFlagToCapture(flagPoints, getPriority, getPosition)
    local alliedFlags, opponentFlags, neutralFlags, totalFlags = CalculateFlagStatistics(BotApi.Scene.Flags)
    local teamIsLosing = isTeamLosing(alliedFlags, opponentFlags, neutralFlags, totalFlags)
    local capturableFlags = CalculateCapturableFlags(totalFlags, alliedFlags)

    PrintFlagDebugInfo(alliedFlags, opponentFlags, neutralFlags, totalFlags, capturableFlags, teamIsLosing)
    
    searchDestroy = CalculateSearchDestroyValue(capturableFlags, alliedFlags, opponentFlags)
    local flags = PrepareFlags(flagPoints, getPriority, getPosition)

    return GetRandomItem(flags, function(f)
        return calculateFlagPriority(f, team, enemyTeam, teamIsLosing, totalFlags)
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

 -- Choose appropriate UnitSpawnWaitTime based on isEvacAttacker
 	local currentUnitSpawnWaitTime = GetCurrentSpawnWaitTime()

	--if printDebug then print("player#".. BotApi.Instance.playerId.. " Unit, TTS, Min TTS") end
	for i, unit in pairs(units) do
		local min_team = unit.min_team  -- not used
		local min_income = unit.min_income -- not used
		local tts = BotApi.Commands:TimeToSpawnUnit(unit.unit)
		local min_tts = GetUnitSelectionTTSLimit()
		local available = BotApi.Commands:IsUnitAvailable(unit.unit)
		
		if not min_income then min_income = -1 end
		if not min_team then min_team = 0 end
		
		--if printDebug then print("------ Unit", unit.unit, (tts / 1000), (min_tts / 1000)) end

		if teamSize >= min_team and income >= min_income and tts <= min_tts and available then
			table.insert(unitsToSpawn, unit)
		end
	end

	-- TODO: instead of return nil, find the shortest tts and delay calling function again by that time 
	if #unitsToSpawn == 0 then
		return nil
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
		
		-- search "type" array for specific element
		local function UnitType (val)
			for index, value in ipairs(t.type) do
				if value == val then
					return true
				end
			end
			return false
		end

		local basePriority = t.priority
		local priorityMultiplier = 1

		if captureBackFlag then
			if UnitType("Aux") then
				return t.priority
			else
				return 0
			end
		end

		-- Bot buys infantry first
		if unitCounts.BotInfantry < 15 then -- minimum amount of infantry
			if UnitType("Infantry") then
				priorityMultiplier = priorityMultiplier
			elseif not UnitType("Doctrine") then
				priorityMultiplier = priorityMultiplier * 0.1
			end
		elseif unitCounts.BotInfantry >= 25 then -- maximum amount of infantry
			if UnitType("Infantry") and not UnitType("AT") then
				priorityMultiplier = priorityMultiplier * 0.1
			end
		end

		if unitCounts.BotInfantry > 10 and unitCounts.BotATInfantry <= 2 then
			if UnitType("Infantry") and UnitType("AT") then
				priorityMultiplier = priorityMultiplier * 4
			end
		elseif unitCounts.BotATInfantry >= 5 then
			if UnitType("Infantry") and UnitType("AT") then
				priorityMultiplier = priorityMultiplier * 0.1
			end
		end

		if unitCounts.BotInfantry + unitCounts.BotATInfantry >= 25 and unitCounts.BotATInfantry >= 2 then
			if UnitType("Infantry") then
				priorityMultiplier = priorityMultiplier * 0.1
			end
		end
	
		-- Global priorities for different class of squads
		if UnitType("Squad") then
			if UnitType("Class1") then
				priorityMultiplier = priorityMultiplier * 3
			elseif UnitType("Class2") then
				priorityMultiplier = priorityMultiplier * 3 * 0.67
			else
				priorityMultiplier = priorityMultiplier * 1.5 * 0.25
			end
		end
	
		-- Global priorities for different class of cannons
		if UnitType("Cannon") then
			if UnitType("Class1") then
				priorityMultiplier = priorityMultiplier * 0.80
			elseif UnitType("Class2") then
				priorityMultiplier = priorityMultiplier * 0.80 * 0.67
			else
				priorityMultiplier = priorityMultiplier * 0.80 * 0.25
			end
		end
	
		-- Global priorities for different class of all other vehicles and infantry teams
		if not UnitType("Cannon") or not UnitType("Squad") then
			if UnitType("Class1") then
				priorityMultiplier = priorityMultiplier * 1
			elseif UnitType("Class2") then
				priorityMultiplier = priorityMultiplier * 0.67
			else
				priorityMultiplier = priorityMultiplier * 0.25
			end
		end

		return basePriority * priorityMultiplier
	end)
end

function OnGameStart()
    countBackFlags()
	-- unitMode is 2022s for lobby; bot buy lists are conquest.<army> (Code:X).
	OnGameStartUtility(BotApi.Instance.unitMode)

end

function OnGameQuant()
	TrySpawnUnit()

	local waypoints = BotApi.Scene.Waypoints
	if #waypoints == 0 then
		for i, squad in pairs(BotApi.Scene.Squads) do
			if not Context.SquadTimers[squad] then
				SetSquadOrder(CaptureFlag, squad, OrderRotationPeriod)
			end
		end
	end
end

function GotoNextWaypoint(squad)
	local waypoints = BotApi.Scene.Waypoints
	BotApi.Commands:CaptureFlag(squad, waypoints[math.random(#waypoints)]) --captureflag is basically gothereandattack
	--if printDebug then print("Print: #captureFlag call inside GoToNextWaypoint") end
end

function OnWaypoint(args)
	--if printDebug then print("Print: #GotoNextWaypoint call inside OnWaypoint") end
	GotoNextWaypoint(args.squadId)
end

	-- NOTE: Returns true if squad tagged "_lua_mi" or "_lua_alert".
	-- NOTE: "_lua_mi" = reserved for mission script use.
	-- NOTE: "_lua_alert" = squad abruptly runs into enemy force seek&destroy.

function IsSquadInScript(squad)
	if BotApi.Scene:IsSquadTagged(squad, "_lua_mi") or BotApi.Scene:IsSquadTagged(squad, "repairing") or BotApi.Scene:IsSquadTagged(squad, "aio_morale_owned") or BotApi.Scene:IsSquadTagged(squad, "aio_morale_surrendering") then
		--if printDebug then print("Print: SQUADinSCRIPT thus no action squad", squad, "Player#",BotApi.Instance.playerId, "Team", team) end
		return true
	end
	if BotApi.Scene:IsSquadTagged(squad, "_lua_alert") then
		--if printDebug then print("Print: SQUADinALERT thus seek by squad", squad, "Player#",BotApi.Instance.playerId, "Team", team) end
		BotApi.Commands:SeekAndDestroy(squad)
		return true
	end
end

	-- NOTE: Returns true if squad tagged "_lua_ignore" for general ignore.
function IsSquadToIgnore(squad)
	if BotApi.Scene:IsSquadTagged(squad, "_lua_ignore") then
		return true
	end
end

function CaptureFlag(squad)
	local flag = GetFlagToCapture(BotApi.Scene.Flags, getDefaultFlagPriority, GetFlagPosition)

	if not flag then
		--if printDebug then print("Print: No Flags so SeekAndDestroy by squad", squad, "Player#",BotApi.Instance.playerIdon, "Team", team) end
		BotApi.Commands:SeekAndDestroy(squad)
		return
	end

	if IsSquadInScript(squad) then
		return
	end

	if IsSquadToIgnore(squad) then
		local rndAI = math.random()

		if searchDestroy > rndAI then
			--if printDebug then print("Print: [see_enemy] seek by squad", squad, "Player#",BotApi.Instance.playerId, "Team", team) end
			BotApi.Commands:SeekAndDestroy(squad)
			return
		else
			--if printDebug then print("Print: [see_enemy] donothing by squad", squad, "Player#",BotApi.Instance.playerId, "Team", team) end
			return
		end
	end

	--if printDebug then print("Print: [notags] ctf by squad", squad, "Player#",BotApi.Instance.playerId, "Team", team, "Flag name: ", flag.name) end
	return BotApi.Commands:CaptureFlag(squad, flag.name)
end

function OnGameSpawn(args)
	local waypoints = BotApi.Scene.Waypoints
	if #waypoints == 0 then
		SetSquadOrder(CaptureFlag, args.squadId, OrderRotationPeriod)
	else
		GotoNextWaypoint(args.squadId)
		--if printDebug then print("Print: #waypoints != 0") end
	end
end

BotApi.Events:Subscribe(BotApi.Events.GameStart, OnGameStart)
BotApi.Events:Subscribe(BotApi.Events.GameEnd, OnGameStop)
BotApi.Events:Subscribe(BotApi.Events.Quant, OnGameQuant)
BotApi.Events:Subscribe(BotApi.Events.GameSpawn, OnGameSpawn)
BotApi.Events:Subscribe(BotApi.Events.Waypoint, OnWaypoint)