-- Diagnostic only: prove whether BotApi-native actor creation is the missing
-- terrain-FoW registration step for allied support.
--
-- This module is loaded only alongside the normal Conquest controller. It never
-- runs on the custom Team-A attack-support bot, does not require utility.lua, and
-- does not replace the legacy parked-template support path.
--
-- The experiment is deliberately narrow: one RUSA infantry squad, one attempt,
-- human-defense missions only, actual engine DefenderBot only.

local PREFIX = "CODEX_NATIVE_SUPPORT_TEST"
local NATIVE_SUPPORT_FOW_TEST_ENABLED = true
local TEST_ARMY = "rusa"
local TEST_UNIT = "codex_native_support_test(rusa)"
local MAX_SQUAD_SIZE = 6
local SPAWN_INDEX = 0

local function emit(...)
	local out = { PREFIX .. ":" }
	for n = 1, select("#", ...) do
		out[#out + 1] = tostring(select(n, ...))
	end
	print(table.concat(out, " "))
end

local function instance()
	return (BotApi and BotApi.Instance) or {}
end

local function conquestApi()
	return (BotApi and BotApi.Conquest) or {}
end

local function scene()
	return (BotApi and BotApi.Scene) or nil
end

local function commands()
	return (BotApi and BotApi.Commands) or nil
end

local function events()
	return (BotApi and BotApi.Events) or nil
end

local function positiveId(primary, fallback)
	primary = tonumber(primary or 0) or 0
	fallback = tonumber(fallback or 0) or 0
	if primary > 0 then return primary end
	if fallback > 0 then return fallback end
	return 0
end

local function safeSpawnPointName()
	local ok, value = pcall(function()
		return instance().spawnPointName
	end)
	if not ok then return "err" end
	if value == nil or tostring(value) == "" then return "nil" end
	return tostring(value)
end

local function readSceneVar(name)
	local sc = scene()
	if not sc or not sc.GetVar then return nil, "missing" end
	local ok, value = pcall(function() return sc:GetVar(name) end)
	if not ok then return nil, "error" end
	if value == nil then return nil, "nil" end
	return value, "ok"
end

local function identity()
	local i = instance()
	local c = conquestApi()
	return {
		playerId = tonumber(i.playerId or 0) or 0,
		team = tostring(i.team or ""),
		army = tostring(i.army or ""),
		gameMode = tostring(i.gameMode or ""),
		attacking = c.Attacking,
		firstPlayerId = positiveId(c.FirstPlayerId, i.CampaignFirstPlayerId),
		firstEnemyId = positiveId(c.FirstEnemyId, i.CampaignFirstEnemyId),
		defenderBotId = positiveId(c.DefenderBotId, i.CampaignDefenderBotId),
		spawnPointName = safeSpawnPointName(),
	}
end

local state = {
	quant = 0,
	attempted = false,
	awaitingSpawnEvent = false,
	spawned = false,
	applicable = nil,
	unit = nil,
	squadId = nil,
}

local function logIdentity(tag, id)
	local c = commands()
	emit(tag,
		"controller_playerId", id.playerId,
		"team", id.team,
		"army", id.army,
		"attacking", tostring(id.attacking),
		"FirstPlayerId", id.firstPlayerId,
		"FirstEnemyId", id.firstEnemyId,
		"DefenderBotId", id.defenderBotId,
		"spawnPointName", id.spawnPointName,
		"Spawn", tostring(c ~= nil and type(c.Spawn) == "function"),
		"SpawnAt", tostring(c ~= nil and type(c.SpawnAt) == "function"),
		"IsUnitAvailable", tostring(c ~= nil and type(c.IsUnitAvailable) == "function"),
		"CanSpawn", tostring(c ~= nil and type(c.CanSpawn) == "function"))
end

local function missionGate(id)
	if not NATIVE_SUPPORT_FOW_TEST_ENABLED then
		state.applicable = false
		return false, "feature_disabled"
	end
	if id.gameMode ~= "campaign_capture_the_flag" then
		state.applicable = false
		return false, "wrong_game_mode"
	end
	if id.army ~= TEST_ARMY then
		state.applicable = false
		return false, "unsupported_army"
	end
	if id.defenderBotId <= 0 then
		return nil, "defenderbot_unresolved"
	end
	if id.playerId ~= id.defenderBotId then
		state.applicable = false
		return false, "not_defenderbot"
	end

	local userIsDefender, status = readSceneVar("user_is_defender")
	if status ~= "ok" then
		return nil, "user_is_defender_" .. status
	end
	local role = tonumber(userIsDefender)
	if role == 0 then
		state.applicable = false
		return false, "human_attack"
	end
	if role ~= 1 then
		return nil, "user_is_defender_unresolved"
	end
	state.applicable = true
	return true, "human_defense_defenderbot"
end

local function unitAvailable()
	local c = commands()
	if not c or type(c.IsUnitAvailable) ~= "function" then
		emit("unit_check", "requested_unit", TEST_UNIT, "IsUnitAvailable", "missing")
		return false
	end
	local ok, value = pcall(function() return c:IsUnitAvailable(TEST_UNIT) end)
	if not ok then
		emit("unit_check", "requested_unit", TEST_UNIT, "IsUnitAvailable", "error", tostring(value))
		return false
	end
	emit("unit_check", "requested_unit", TEST_UNIT, "IsUnitAvailable", tostring(value == true))
	return value == true
end

local function logCanSpawn()
	local c = commands()
	if not c or type(c.CanSpawn) ~= "function" then
		emit("CanSpawn", "requested_unit", TEST_UNIT, "result", "api_missing")
		return
	end
	local ok, value = pcall(function() return c:CanSpawn(TEST_UNIT) end)
	if ok then
		emit("CanSpawn", "requested_unit", TEST_UNIT, "result", tostring(value))
	else
		emit("CanSpawn", "requested_unit", TEST_UNIT, "result", "error", tostring(value))
	end
end

local function attemptNativeSpawn(trigger)
	if state.attempted or state.spawned then return false end

	local id = identity()
	local allowed, reason = missionGate(id)
	if allowed == nil then
		if state.quant == 0 or state.quant % 50 == 0 then
			emit("gate_wait", "trigger", trigger, "reason", reason,
				"controller_playerId", id.playerId, "DefenderBotId", id.defenderBotId)
		end
		return false
	end
	if allowed == false then
		emit("gate_skip", "trigger", trigger, "reason", reason,
			"controller_playerId", id.playerId, "DefenderBotId", id.defenderBotId,
			"army", id.army)
		return false
	end

	-- One-shot latch is set before touching any spawn API. A failed call is still a
	-- completed diagnostic attempt and must never become a Quant spawn loop.
	state.attempted = true
	state.unit = TEST_UNIT
	logIdentity("spawn_context", id)
	emit("spawn_request", "trigger", trigger, "requested_unit", TEST_UNIT,
		"maxSquadSize", MAX_SQUAD_SIZE, "spawnIndex", SPAWN_INDEX)

	if not unitAvailable() then
		emit("spawn_failed", "reason", "unit_unavailable", "requested_unit", TEST_UNIT)
		return false
	end
	logCanSpawn()

	local c = commands()
	if not c then
		emit("spawn_failed", "reason", "Commands_missing")
		return false
	end

	if type(c.SpawnAt) == "function" then
		state.awaitingSpawnEvent = true
		local ok, result = pcall(function()
			return c:SpawnAt(TEST_UNIT, MAX_SQUAD_SIZE, SPAWN_INDEX)
		end)
		emit("SpawnAt", "attempt", "requested_unit", TEST_UNIT,
			"ok", tostring(ok), "result", tostring(result))
		if ok and result == true then
			return true
		end
		state.awaitingSpawnEvent = false
	else
		emit("SpawnAt", "attempt", "requested_unit", TEST_UNIT, "result", "api_missing")
	end

	if type(c.Spawn) == "function" then
		state.awaitingSpawnEvent = true
		local ok, result = pcall(function()
			return c:Spawn(TEST_UNIT, MAX_SQUAD_SIZE)
		end)
		emit("Spawn", "fallback_attempt", "requested_unit", TEST_UNIT,
			"ok", tostring(ok), "result", tostring(result))
		if ok and result == true then
			return true
		end
		state.awaitingSpawnEvent = false
		emit("spawn_failed", "reason", "Spawn_returned_false", "requested_unit", TEST_UNIT)
		return false
	end

	emit("spawn_failed", "reason", "Spawn_api_missing", "requested_unit", TEST_UNIT)
	return false
end

local function onGameStart()
	state.quant = 0
	state.attempted = false
	state.awaitingSpawnEvent = false
	state.spawned = false
	state.applicable = nil
	state.unit = nil
	state.squadId = nil
	local id = identity()
	logIdentity("GameStart", id)
	attemptNativeSpawn("GameStart")
end

local function onQuant()
	state.quant = state.quant + 1
	if not state.attempted and state.applicable ~= false then
		attemptNativeSpawn("Quant")
	end
end

local function onGameSpawn(args)
	if not state.awaitingSpawnEvent or state.spawned then return end
	local squadId = args and args.squadId or nil
	if squadId == nil then
		emit("GameSpawn", "requested_unit", tostring(state.unit), "squadId", "nil")
		return
	end
	state.awaitingSpawnEvent = false
	state.spawned = true
	state.squadId = squadId
	emit("GameSpawn", "requested_unit", tostring(state.unit), "squadId", tostring(squadId),
		"controller_playerId", identity().playerId)

	local sc = scene()
	if sc and sc.SetVar then
		pcall(function() sc:SetVar("codex_native_support_test_squad", squadId) end)
	end

	local c = commands()
	if c and type(c.SeekAndDestroy) == "function" then
		local ok, result = pcall(function() return c:SeekAndDestroy(squadId) end)
		emit("order", "SeekAndDestroy", "squadId", tostring(squadId),
			"ok", tostring(ok), "result", tostring(result))
	else
		emit("order", "SeekAndDestroy", "squadId", tostring(squadId), "result", "api_missing")
	end
end

local function safeEvent(name, fn)
	return function(...)
		local ok, err = pcall(fn, ...)
		if not ok then emit("event_error", name, tostring(err)) end
	end
end

local id0 = identity()
logIdentity("module_loaded", id0)

local ev = events()
if ev and ev.Subscribe then
	ev:Subscribe(ev.GameStart, safeEvent("GameStart", onGameStart))
	ev:Subscribe(ev.Quant, safeEvent("Quant", onQuant))
	ev:Subscribe(ev.GameSpawn, safeEvent("GameSpawn", onGameSpawn))
	emit("armed", "one_shot", true, "test_unit", TEST_UNIT, "role", "human_defense_defenderbot_only")
else
	emit("not_armed", "BotApi.Events_missing")
end
