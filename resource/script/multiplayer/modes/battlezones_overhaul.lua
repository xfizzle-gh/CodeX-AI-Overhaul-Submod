-- Battle Zones-only AI overrides.
-- Keep these out of Conquest so campaign wave and preparation behavior is unchanged.

local BattleZonesSpawnRetryTime = 15 * 1000
local BaseGetUnitToSpawn = GetUnitToSpawn
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

-- Backfield logic originally allowed only purchases tagged Aux. The curated
-- roster currently has no Aux entries, so a bot assigned the friendly back flag
-- could produce a zero-weight purchase pool forever. Let it buy a normal unit,
-- then preserve captureBackFlag so that unit is ordered to secure the backfield.
function GetUnitToSpawn(units)
	if not units then
		return nil
	end

	if captureBackFlag then
		local hasAuxPurchase = false
		for _, unit in pairs(units) do
			if HasUnitType(unit, "Aux") then
				hasAuxPurchase = true
				break
			end
		end

		if not hasAuxPurchase then
			if not reportedAuxFallback then
				print("Print: player#" .. BotApi.Instance.playerId .. " has no Aux purchase; using a normal unit for the backfield objective")
				reportedAuxFallback = true
			end

			captureBackFlag = false
			local selectedUnit = BaseGetUnitToSpawn(units)
			captureBackFlag = true
			return selectedUnit
		end
	end

	return BaseGetUnitToSpawn(units)
end
