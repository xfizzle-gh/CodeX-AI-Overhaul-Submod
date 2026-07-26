-- Battle Zones-only AI overrides.
-- Keep these out of Conquest so campaign wave and preparation behavior is unchanged.

local BattleZonesSpawnRetryTime = 15 * 1000
local BaseGetUnitToSpawn = GetUnitToSpawn
local reportedAuxFallback = false

-- The shared utility uses this both to filter unit timers and to decide how long
-- to hold a failed purchase. Ninety seconds could leave a bot effectively idle.
function GetCurrentSpawnWaitTime()
	return BattleZonesSpawnRetryTime
end

-- Backfield logic normally allows only purchases tagged Aux. Prefer that path
-- when it produces a valid selection. If no eligible Aux purchase exists, retry
-- once with normal purchase weights while preserving the backfield assignment.
function GetUnitToSpawn(units)
	if not units then
		return nil
	end

	local selectedUnit = BaseGetUnitToSpawn(units)
	if selectedUnit or not captureBackFlag then
		return selectedUnit
	end

	if not reportedAuxFallback then
		print("Print: player#" .. BotApi.Instance.playerId .. " has no eligible Aux purchase; using a normal unit for the backfield objective")
		reportedAuxFallback = true
	end

	captureBackFlag = false
	selectedUnit = BaseGetUnitToSpawn(units)
	captureBackFlag = true
	return selectedUnit
end
