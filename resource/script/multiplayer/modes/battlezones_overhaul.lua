-- Battle Zones-only AI overrides.
-- Keep these out of Conquest so campaign wave and preparation behavior is unchanged.

local BattleZonesSpawnRetryTime = 15 * 1000
local BaseGetUnitToSpawn = GetUnitToSpawn
local reportedAuxFallback = false

-- Keep the base 90-second TimeToSpawnUnit selection horizon, but retry a failed
-- purchase after 15 seconds instead of holding the bot for the full horizon.
function TrySpawnUnit()
	if Context.SpawnWait.CooldownTimer then
		return
	end

	if Context.SpawnWait.WaitTimer then
		return
	end

	if not BotApi.Commands:CanSpawn() then
		return
	end

	if spawningUnit then
		if OnUnitPurchased then
			OnUnitPurchased()
		elseif enableWaveCounter then
			WaveUnitCounter()
		end

		KillSpawnWaitTimer()
		Context.SpawnInfo = nil
		SetSpawnCooldownTimer()
		spawningUnit = nil
		return
	end

	local retryPendingUnit = Context.SpawnWait.RetryPendingUnit
	Context.SpawnWait.RetryPendingUnit = false

	if not retryPendingUnit or not Context.SpawnInfo then
		UpdateUnitToSpawn(Context.Purchase)
	end

	if not Context.SpawnInfo then
		return
	end

	local unit = Context.SpawnInfo.unit

	if not BotApi.Commands:IsUnitAvailable(unit) then
		print("Print: !!WARNING!! player#" .. BotApi.Instance.playerId .. " tried to purchase: " .. unit .. " which is not available")
		KillSpawnWaitTimer()
		Context.SpawnInfo = nil
		return
	end

	if GameModeSpawnUnit(unit, MaxSquadSize) then
		spawningUnit = true
		return
	end

	if retryPendingUnit then
		Context.SpawnInfo = nil
	end

	if not Context.SpawnWait.WaitTimer then
		if printDebug then
			print("Print: player#" .. BotApi.Instance.playerId .. " tried to purchase: " .. unit .. " Not enough MP, DP, CP, or the unit timer is not unlocked")
			print("Print: player#" .. BotApi.Instance.playerId .. " will retry this purchase path in " .. (BattleZonesSpawnRetryTime / 1000 + 1) .. "s")
		end
		Context.SpawnWait.WaitTimer = BotApi.Events:SetQuantTimer(
			function()
				Context.SpawnWait.WaitTimer = nil
				Context.SpawnWait.RetryPendingUnit = Context.SpawnInfo ~= nil
			end,
			BattleZonesSpawnRetryTime + 1000)
	end
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
