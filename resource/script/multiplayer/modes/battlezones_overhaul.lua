-- Battle Zones-only AI overrides.
-- Keep these out of Conquest so campaign wave and preparation behavior is unchanged.

local BattleZonesSpawnRetryTime = 15 * 1000
local BaseGetUnitToSpawn = GetUnitToSpawn
local BaseCaptureFlag = CaptureFlag
local BaseIsSquadInScript = IsSquadInScript
local reportedAuxFallback = false

local function HasUnitType(unit, wantedType)
	for _, unitType in ipairs(unit.type or {}) do
		if unitType == wantedType then
			return true
		end
	end
	return false
end

-- The shared utility uses this both to filter unit timers and to decide how long
-- to hold a failed purchase. Ninety seconds could leave a bot effectively idle.
function GetCurrentSpawnWaitTime()
	return BattleZonesSpawnRetryTime
end

-- Apply role weights before the base Battle Zones selector applies its existing
-- infantry-count, AT-count, and class multipliers. Prefer an eligible Aux unit
-- for a backfield assignment, then fall back to a normal purchase if none exists.
function GetUnitToSpawn(units)
	if not units then
		return nil
	end

	local weightedUnits = {}
	for _, unit in pairs(units) do
		local weightedUnit = {}
		for key, value in pairs(unit) do
			weightedUnit[key] = value
		end

		local roleMultiplier = 1.0
		if HasUnitType(unit, "Team") then
			roleMultiplier = roleMultiplier * 0.85
		end
		if HasUnitType(unit, "Medic") then
			roleMultiplier = roleMultiplier * 0.60
		end
		if HasUnitType(unit, "Recon") then
			roleMultiplier = roleMultiplier * 0.80
		end
		if HasUnitType(unit, "Sniper") then
			roleMultiplier = roleMultiplier * 0.75
		end
		if HasUnitType(unit, "MG") then
			roleMultiplier = roleMultiplier * 0.90
		end
		if HasUnitType(unit, "Engineer") then
			roleMultiplier = roleMultiplier * 0.90
		end

		weightedUnit.priority = (unit.priority or 0) * roleMultiplier
		table.insert(weightedUnits, weightedUnit)
	end

	local selectedUnit = BaseGetUnitToSpawn(weightedUnits)
	if selectedUnit or not captureBackFlag then
		return selectedUnit
	end

	if not reportedAuxFallback then
		print("Print: player#" .. BotApi.Instance.playerId .. " has no eligible Aux purchase; using a normal unit for the backfield objective")
		reportedAuxFallback = true
	end

	captureBackFlag = false
	selectedUnit = BaseGetUnitToSpawn(weightedUnits)
	captureBackFlag = true
	return selectedUnit
end

-- Code:X and imported mission scripts use both alert spellings. Preserve the
-- existing behavior and add the unprefixed tag without changing Conquest.
function IsSquadInScript(squad)
	if BotApi.Scene:IsSquadTagged(squad, "lua_alert") then
		BotApi.Commands:SeekAndDestroy(squad)
		return true
	end
	return BaseIsSquadInScript(squad)
end

-- Objective play remains primary. Normal squads receive a limited flanking
-- chance after the opening neutral phase, scaled by the current flag state.
function CaptureFlag(squad)
	if IsSquadInScript(squad) then
		return
	end

	if captureBackFlag or IsSquadToIgnore(squad) then
		return BaseCaptureFlag(squad)
	end

	local alliedFlags, opponentFlags, neutralFlags, totalFlags = CalculateFlagStatistics(BotApi.Scene.Flags)
	local flankChance = 0

	if neutralFlags ~= totalFlags then
		if opponentFlags > alliedFlags then
			flankChance = 0.15
		elseif alliedFlags > opponentFlags then
			flankChance = 0.35
		else
			flankChance = 0.25
		end
	end

	if math.random() < flankChance then
		BotApi.Commands:SeekAndDestroy(squad)
		return
	end

	return BaseCaptureFlag(squad)
end
