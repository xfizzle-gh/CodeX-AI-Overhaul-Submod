require([[/script/multiplayer/modes/utility]])

-- Time from start of match AI will wait before attempting to buy a unit.
local StartSpawnTime = {
	Min = 5 * 1000, -- sec (ms)
	Max = 6 * 1000, -- sec (ms)
}
 -- Time from last purchase AI will wait before attempting to buy a new unit.
local SpawnCooldownTime = {
	AttackerMin = 10 * 1000, -- sec (ms)
	AttackerMax = 25 * 1000, -- sec (ms)
	DefenderMin = 5 * 1000, -- sec (ms)
	DefenderMax = 10 * 1000, -- sec (ms)
}
local UnitSpawnWaitTime = 40 * 1000 -- sec (ms) Sets time limit AI will wait for a unit it has chosen to buy if the unit is not yet available

-- Time delay for units to get a new move order after spawn move order. Loops.
local OrderRotationPeriod
local OrderRotationTime = {
	Attacker = 2 * 60000, -- min (ms)
	Defender = 3 * 60000, -- min (ms)
}

local firstPurchase = 6

local function orderRotation()
	if team == "a" then
	 	OrderRotationPeriod = OrderRotationTime.Attacker
	else
		OrderRotationPeriod = OrderRotationTime.Defender
	end
end

function GameModeSpawnCooldown()
	if firstPurchase > 0 then
        firstPurchase = firstPurchase - 1
		return math.random(StartSpawnTime.Min, StartSpawnTime.Max)
	elseif team == "a" then
		return math.random(SpawnCooldownTime.AttackerMin, SpawnCooldownTime.AttackerMax)
	else 
		return math.random(SpawnCooldownTime.DefenderMin, SpawnCooldownTime.DefenderMax)
	end
end

local function setVarsInMissionScript()
	local botNation = BotApi.Instance.army
	local botDifficulty = BotApi.Instance.difficulty
	local botId = BotApi.Instance.playerId
	local spawnLane = tonumber(string.sub(BotApi.Instance.spawnPointName, 3, 3))

	local botLaneNation = string.format("%s_l%d_bot_nation", team, spawnLane)
	local botLaneDifficulty = string.format("%s_l%d_bot_diff", team, spawnLane)
	local botLaneId = string.format("%s_l%d_bot_id", team, spawnLane)

	local nationMap = { rus = 1, ger = 2, fin = 3, usa = 4, eng = 5 }
	local difficultyMap = { easy = 1, normal = 2, hard = 3, heroic = 4 }

	BotApi.Scene:SetVar(botLaneNation, nationMap[botNation] or 0)
	BotApi.Scene:SetVar(botLaneDifficulty, difficultyMap[botDifficulty] or 0)
	BotApi.Scene:SetVar(botLaneId, botId or 0)
end

function buildFlagLocationMap(flagPoints)
    local flags = {}
    for i, flag in pairs(flagPoints) do
        local phase = flag.name:sub(2, 2)
        local lane = flag.name:sub(3, 3)
        local order = flag.name:len() >= 5 and flag.name:sub(5, 5) or '4'
        local priority = 5 - order
        table.insert(flags, {name = flag.name, priority = priority, owner = flag.occupant, phase = phase, lane = lane, order = order})
    end
    return flags
end

local activePhase = "1"  -- Phase 1 is active by default

-- Function to determine if all flags in a given phase and lane are captured by the attacker
local function isLaneCapturedByAttacker(flags, phase, lane)
    for _, flag in pairs(flags) do
        if flag.phase == phase and flag.lane == lane and flag.owner ~= "a" then
            return false
        end
    end
    return true
end

-- Function to determine if all flags in a given phase are captured by the attacker
local function areAllFlagsCapturedByAttacker(flags, phase)
    for _, flag in pairs(flags) do
        if flag.phase == phase and flag.owner ~= "a" then
            return false
        end
    end
    return true
end

-- Function to get the lowest order flag not owned by the attacker in a lane and phase
local function getLowestOrderFlagNotOwned(flags, phase, lane)
    local lowestFlag = nil
    for _, flag in pairs(flags) do
        if flag.phase == phase and flag.lane == lane and flag.owner ~= "a" then
            if not lowestFlag or tonumber(flag.order) < tonumber(lowestFlag.order) then
                lowestFlag = flag
            end
        end
    end
    return lowestFlag
end

local function calculateFlagPriority(f, flags)
    local spawn = BotApi.Instance.spawnPointName
    local spawnPhase = string.sub(spawn, 2, 2)
    local spawnLane = string.sub(spawn, 3, 3)
    --print("Frontlines: Spawn: " .. spawn .. " SpawnLane: " .. spawnLane)
    --print("Frontlines: Flag: " .. f.name .. " FlagLane: " .. f.lane)

    -- Switch to Phase 2 if all flags in Phase 1 are captured by the attacker
    if activePhase == "1" and areAllFlagsCapturedByAttacker(flags, "1") then
        activePhase = "2"
    end

    -- Ignore flags that are not in the active phase, unless the defender is prioritizing Phase 2 after losing all Phase 1 flags
    if f.phase ~= activePhase then
        if spawn:sub(1, 1) == "b" then
            local currentLaneCaptured = isLaneCapturedByAttacker(flags, "1", spawnLane)
            -- Defender should prioritize Phase 2 flags in their lane even if activePhase is 1, if they lost all Phase 1 flags
            if currentLaneCaptured and f.phase == "2" and f.lane == spawnLane then
                return f.priority + 5 -- Defender should prioritize Phase 2 flags in their own lane after losing Phase 1
            end
        end
        return 0
    end

    -- Ignore flags in lanes that are completely captured by the attacker
    if isLaneCapturedByAttacker(flags, f.phase, f.lane) then
        return 0
    end

    -- Attacker behavior in Phase 1
    if spawn:sub(1, 1) == "a" then
        local currentLaneCaptured = isLaneCapturedByAttacker(flags, "1", spawnLane)

        -- Attackers should prioritize other lanes in Phase 1 if their own lane is fully captured
        if currentLaneCaptured and activePhase == "1" then
            if f.phase == "1" and f.lane ~= spawnLane and f.owner ~= "a" then
                -- Find the lowest order flag in the current lane that is not captured
                local lowestOrderFlag = getLowestOrderFlagNotOwned(flags, f.phase, f.lane)

                -- Prioritize the lowest order flag highly in other lanes
                if lowestOrderFlag and f.name == lowestOrderFlag.name then
                    return f.priority + 5 -- Give higher priority to the next flag in order
                end

                -- If the flag is not the next one in order, but is still not captured, use its default priority
                return f.priority
            end
        end

        -- If the attacker's lane is not fully captured, prioritize only flags in their own lane in order
        if f.lane == spawnLane and f.phase == activePhase then
            local lowestOrderFlag = getLowestOrderFlagNotOwned(flags, f.phase, spawnLane)
            if lowestOrderFlag and f.name == lowestOrderFlag.name then
                return f.priority + 5 -- Give higher priority to the next flag in order
            end

            -- If the flag is not the next one in order, but is still not captured, use its default priority
            if f.owner ~= "a" then
                return f.priority
            end

            -- Ignore flag if captured
            return 0
        end

        -- Ignore flags in other lanes if current lane is not fully captured
        return 0
    end

    -- Defender behavior in Phase 1
    if spawn:sub(1, 1) == "b" and activePhase == "1" then
        -- Find the lowest order flag in the defender's lane that is not captured
        local lowestOrderFlag = getLowestOrderFlagNotOwned(flags, f.phase, spawnLane)

        if f.lane == spawnLane then
            -- If the lowest order flag is still owned by the defender, prioritize defending it
            if lowestOrderFlag and f.name == lowestOrderFlag.name and f.owner == "b" then
                return f.priority + 15 -- Prioritize defending the lowest order flag
            end

            -- If the flag is captured by the attacker, assign a lower priority for recapture
            if f.owner == "a" then
                return 1 -- Lower priority for recapturing lost flags
            end

            -- Default priority for other unowned flags
            return f.priority + 3
        end

        -- Ignore flags in other lanes if current lane is not fully captured
        return 0
    end

    -- Defender behavior in Phase 2
	if activePhase == "2" and spawn:sub(1, 1) == "b" then
		local currentLaneCaptured = isLaneCapturedByAttacker(flags, "2", spawnLane)

		-- Defender should prioritize only their own lane until it is fully captured
		if not currentLaneCaptured then
			if f.lane == spawnLane then
				local lowestOrderFlag = getLowestOrderFlagNotOwned(flags, f.phase, spawnLane)
				if lowestOrderFlag and f.name == lowestOrderFlag.name and f.owner == "b" then
					return f.priority + 4 -- Prioritize defending the lowest order flag
				elseif f.owner == "a" then
					return 1 -- Lower priority for recapturing lost flags
				end
				return f.priority -- Default priority for other unowned flags
			else
				-- Ignore flags in other lanes if the current lane is not fully captured
				return 0
			end
		end

		-- If the current lane is fully captured, allow defenders to prioritize other lanes in Phase 2
		if currentLaneCaptured then
			if f.lane ~= spawnLane then
				local lowestOrderFlag = getLowestOrderFlagNotOwned(flags, f.phase, f.lane)
				if lowestOrderFlag and f.name == lowestOrderFlag.name then
					return f.priority + 4 -- Defend the lowest order flag in other lanes
				elseif f.owner == "a" then
					return 1 -- Lower priority for recapturing lost flags in other lanes
				end
				return f.priority -- Default priority for other unowned flags in other lanes
			end
		end
	end

    return f.priority
end

function GetFlagToCapture(flagPoints)
	local flags = buildFlagLocationMap(flagPoints)

	return GetRandomItem(flags, function(f) 
		return calculateFlagPriority(f, flags)
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
	
		
		local cannon -- temp until move order is fixed
		if team == "a" then
			cannon = 0
		else
			cannon = 1
		end

		-- Global priorities for different class of cannons
		if UnitType("Cannon") then
			if UnitType("Class1") then
				priorityMultiplier = priorityMultiplier * 0.80 * cannon
			elseif UnitType("Class2") then
				priorityMultiplier = priorityMultiplier * 0.80 * 0.67 * cannon
			else
				priorityMultiplier = priorityMultiplier * 0.80 * 0.25 * cannon
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
    setVarsInMissionScript()
	orderRotation()
    -- unitMode is 2022s for lobby; bot buy lists are conquest.<army> (Code:X).
    OnGameStartUtility("conquest")

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
	if BotApi.Scene:IsSquadTagged(squad, "_lua_mi") or BotApi.Scene:IsSquadTagged(squad, "repairing") or BotApi.Scene:IsSquadTagged(squad, "aio_morale_owned") or BotApi.Scene:IsSquadTagged(squad, "aio_morale_surrendering") or BotApi.Scene:IsSquadTagged(squad, "aio_morale_surrender_evacuating") then
		--if printDebug then print("Print: SQUADinSCRIPT thus no action squad", squad, "Player#",BotApi.Instance.playerId, "Team", team) end
		return true
	end
	if BotApi.Scene:IsSquadTagged(squad, "_lua_alert") then
		--if printDebug then print("Print: SQUADinALERT thus seek by squad", squad, "Player#",BotApi.Instance.playerId, "Team", team) end
		if not isEvacDefender then
			BotApi.Commands:SeekAndDestroy(squad)
		end
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
	local flag = GetFlagToCapture(BotApi.Scene.Flags)

	if not flag then
		--if printDebug then print("Print: No Flags so SeekAndDestroy by squad", squad, "Player#",BotApi.Instance.playerIdon, "Team", team) end
		BotApi.Commands:SeekAndDestroy(squad)
		return
	end

	if IsSquadInScript(squad) then
		return
	end

	if IsSquadToIgnore(squad) then
		return
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

-- Called once when preptime ends. Forces Attacker bots to move to flags instead of waiting for OrderRotationPeriod
function OnPrepTimeOver()
    for squad, timer in pairs(Context.SquadTimers) do
        if BotApi.Scene:IsSquadExists(squad) then
            Context.SquadTimers[squad] = nil
            CaptureFlag(squad)
            SetSquadOrder(CaptureFlag, squad, OrderRotationPeriod)
        end
    end
end

BotApi.Events:Subscribe(BotApi.Events.GameStart, OnGameStart)
BotApi.Events:Subscribe(BotApi.Events.GameEnd, OnGameStop)
BotApi.Events:Subscribe(BotApi.Events.Quant, OnGameQuant)
BotApi.Events:Subscribe(BotApi.Events.GameSpawn, OnGameSpawn)
BotApi.Events:Subscribe(BotApi.Events.Waypoint, OnWaypoint)
BotApi.Events:Subscribe(BotApi.Events.PrepTimeOver, OnPrepTimeOver)