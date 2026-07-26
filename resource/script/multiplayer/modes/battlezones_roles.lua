-- Battle Zones-only role weighting and tactical behavior.
-- Loaded by the curated 2022s faction purchase modules after the base mode and
-- spawn-reliability layer have initialized.

if BattleZonesRolesLoaded then
	return
end
BattleZonesRolesLoaded = true

local BaseGetUnitToSpawn = GetUnitToSpawn
local BaseCaptureFlag = CaptureFlag
local BaseIsSquadInScript = IsSquadInScript

local function HasUnitType(unit, wantedType)
	for _, unitType in ipairs(unit.type or {}) do
		if unitType == wantedType then
			return true
		end
	end
	return false
end

-- Apply role weights before the base selector applies infantry-count, AT-count,
-- class, availability, and unit-timer logic.
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

	return BaseGetUnitToSpawn(weightedUnits)
end

-- Code:X and imported mission scripts use both alert spellings.
function IsSquadInScript(squad)
	if BotApi.Scene:IsSquadTagged(squad, "lua_alert") then
		BotApi.Commands:SeekAndDestroy(squad)
		return true
	end
	return BaseIsSquadInScript(squad)
end

-- Objective capture remains the default. Flanking is disabled during the
-- all-neutral opening and while the bot is securing a friendly backfield flag.
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
