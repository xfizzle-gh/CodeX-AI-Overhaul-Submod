-- Battle Zones-only AI overrides.
-- Keep these out of Conquest so campaign wave and preparation behavior is unchanged.

local BattleZonesSpawnRetryTime = 15 * 1000
local reportedCanSpawnBypass = false

-- The shared utility uses this both to filter unit timers and to decide how long
-- to hold a failed purchase. Ninety seconds could leave a bot effectively idle.
function GetCurrentSpawnWaitTime()
	return BattleZonesSpawnRetryTime
end

-- Battle Zones can report CanSpawn() false for an allied bot even though its
-- purchase module and budget are valid. Direct Spawn() remains the authoritative
-- validation step, so attempt it and use the bounded retry path on failure.
function TrySpawnUnit()
	if Context.SpawnWait.CooldownTimer then
		return
	end

	if Context.SpawnWait.WaitTimer then
		return
	end

	if not BotApi.Commands:CanSpawn() and not reportedCanSpawnBypass then
		print("Print: player#" .. BotApi.Instance.playerId .. " CanSpawn returned false; Battle Zones will validate through Spawn instead")
		reportedCanSpawnBypass = true
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
		print("Print: !!WARNING!! player#" .. BotApi.Instance.playerId .. " tried to purchase unavailable unit: " .. unit)
		KillSpawnWaitTimer()
		Context.SpawnInfo = nil
		return
	end

	if GameModeSpawnUnit(unit, MaxSquadSize) then
		spawningUnit = true
		return
	end

	local currentUnitSpawnWaitTime = GetCurrentSpawnWaitTime()

	if retryPendingUnit then
		Context.SpawnInfo = nil
	end

	if not Context.SpawnWait.WaitTimer then
		if printDebug then
			print("Print: player#" .. BotApi.Instance.playerId .. " tried to purchase: " .. unit .. " Not enough MP, DP, CP, spawn access, or the unit timer is not unlocked")
			print("Print: player#" .. BotApi.Instance.playerId .. " will retry or select another unit in " .. (currentUnitSpawnWaitTime / 1000 + 1) .. "s")
		end

		Context.SpawnWait.WaitTimer = BotApi.Events:SetQuantTimer(
			function()
				Context.SpawnWait.WaitTimer = nil
				Context.SpawnWait.RetryPendingUnit = Context.SpawnInfo ~= nil
			end,
			currentUnitSpawnWaitTime + 1000)
	end
end
