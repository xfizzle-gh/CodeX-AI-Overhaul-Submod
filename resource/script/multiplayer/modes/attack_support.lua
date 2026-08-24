-- Attack support controller (human ATTACK missions).
-- Identity + orders only. Do NOT require utility.lua / logic/main.lua here:
-- that path AVs on the attack support slot (no spawn deck) even with spawnPoint nil-guard
-- (proven 2026-07-29 log: crash in lua.event.notify2 right after Loading utility.lua).
--
-- Unit delivery is MI: attack_support_waves.inc (real-breed pool, MOVE in).
-- Lua Spawn is not viable on this slot (IsUnitAvailable always false; utility load crashes).
-- Mission participation is gated in MI by support_mission_enabled$. This controller publishes
-- its own routed Team A playerId as the attack-support owner and issues squad orders.
--
-- This slot also carries the ENGINE-STATE MIRROR. Every MIRROR_QUANTS quants it writes
-- one game.log line per wave engine - attack_support, enemy_defense (plus its garrison
-- anchors), defense_support, enemy_attack - and the resolved faction_support_army$.
-- Always on and log-only, because the on-screen diagnostics in those engines are gated
-- behind support_debug$ and default to off, so the log is all a shipped run leaves
-- behind. Reads go through readVar, which pcall-guards GetVar.

local PREFIX = "CODEX_ATTACK_SUPPORT"

local DEBUG_LOG = true

local function emit(...)
	local out = { PREFIX .. ":" }
	for n = 1, select("#", ...) do
		out[#out + 1] = tostring(select(n, ...))
	end
	print(table.concat(out, " "))
end

local function log(...)
	if not DEBUG_LOG then return end
	emit(...)
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

local function events()
	return (BotApi and BotApi.Events) or nil
end

local function cmds()
	return (BotApi and BotApi.Commands) or nil
end

local function readVar(name)
	local sc = scene()
	if not sc then return "na" end
	local ok, v = pcall(function() return sc:GetVar(name) end)
	if not ok then return "err" end
	if v == nil then return "nil" end
	return tostring(v)
end

local function positiveId(primary, fallback)
	primary = tonumber(primary or 0) or 0
	fallback = tonumber(fallback or 0) or 0
	if primary > 0 then return primary end
	if fallback > 0 then return fallback end
	return 0
end

-- NEVER touch spawnPointName / PlayerSpawnPoint / require(utility) on this slot.
local function identity()
	local i = instance()
	local c = conquestApi()
	return {
		playerId = tonumber(i.playerId or 0) or 0,
		team = tostring(i.team or ""),
		army = tostring(i.army or ""),
		difficulty = tostring(i.difficulty or ""),
		gameMode = tostring(i.gameMode or ""),
		attacking = c.Attacking,
		firstPlayerId = positiveId(c.FirstPlayerId, i.CampaignFirstPlayerId),
		firstEnemyId = positiveId(c.FirstEnemyId, i.CampaignFirstEnemyId),
		defenderBotId = positiveId(c.DefenderBotId, i.CampaignDefenderBotId),
	}
end

local function mirrorMotor()
	emit("motor_left", readVar("attack_support_motor_left"),
		"wave_cmd", readVar("attack_support_wave_cmd"),
		"test", readVar("attack_support_motor_test"),
		"test_done", readVar("attack_support_motor_test_done"))
	emit("place_defense", readVar("defense_support_place"),
		"pad", readVar("defense_support_entry_rr"),
		"stage", readVar("defense_support_stage"))
	emit("place_enemy_defense", readVar("enemy_defense_place"),
		"pad", readVar("enemy_defense_entry_rr"),
		"stage", readVar("enemy_defense_stage"))
	emit("place_attack", "pad", readVar("attack_support_entry_rr"),
		"stage", readVar("attack_support_stage"))
	emit("place_enemy_attack", "pad", readVar("enemy_attack_entry_rr"),
		"stage", readVar("enemy_attack_stage"))
	emit("motor_enemy_left", readVar("enemy_attack_motor_left"),
		"wave_cmd", readVar("enemy_attack_wave_cmd"),
		"test", readVar("enemy_attack_motor_test"),
		"test_done", readVar("enemy_attack_motor_test_done"))
	emit("e2", "e2_test", readVar("support_e2_test"),
		"e2_stage", readVar("support_e2_stage"),
		"e2_fail", readVar("support_e2_fail"),
		"e2_lz", readVar("support_e2_lz"),
		"e2_flag", readVar("support_e2_flag"))
end

local state = {
	quant = 0,
	ordered = {},
	identityPublished = false,
	attackMission = nil,
}

local function publishIdentity(id, isRetry)
	if id.attacking == false then
		state.attackMission = false
		return false
	end
	if id.attacking ~= true then return false end
	state.attackMission = true
	local sc = scene()
	if not sc or not sc.SetVar then
		if not isRetry then log("identity_publish_skipped", "Scene.SetVar_missing") end
		return false
	end
	-- bot.main.lua routes this module only onto the non-human Team A attack-support
	-- controller and explicitly excludes DefenderBotId. Therefore this controller's
	-- own playerId is the authoritative same-side owner. DefenderBotId is a defender-
	-- side identity in human-attack missions and must never own friendly support.
	local ownerId = positiveId(id.playerId, 0)
	if ownerId <= 0 then
		if not isRetry or state.quant % 50 == 0 then
			log("identity_publish_skipped", "support_controller_owner_unresolved",
				"controller_playerId", id.playerId,
				"defenderBotId", id.defenderBotId,
				"team", id.team,
				"retry", tostring(isRetry == true))
		end
		return false
	end
	sc:SetVar("id_attack_support", ownerId)
	sc:SetVar("attack_support_ready", 1)
	sc:SetVar("attack_support_use_mi", 1)
	state.identityPublished = true
	log("identity_published", "id_attack_support", ownerId,
		"controller_playerId", id.playerId,
		"defenderBotId", id.defenderBotId,
		"team", id.team,
		"retry", tostring(isRetry == true),
		"mi_waves", 1)
	return true
end

local function pickFlagName()
	local sc = scene()
	if not sc or type(sc.Flags) ~= "table" then return nil end
	local names = {}
	for _, flag in pairs(sc.Flags) do
		if flag and flag.name then
			names[#names + 1] = tostring(flag.name)
		end
	end
	if #names == 0 then return nil end
	return names[math.random(#names)]
end

local function orderSquad(squad)
	local sc = scene()
	if sc and sc.IsSquadTagged then
		local ok, owned = pcall(function()
			return sc:IsSquadTagged(squad, "aio_morale_owned") or sc:IsSquadTagged(squad, "_lua_mi") or sc:IsSquadTagged(squad, "repairing")
		end)
		if ok and owned then return end
	end
	local c = cmds()
	if not c then return end
	local flagName = pickFlagName()
	if flagName and c.CaptureFlag then
		local ok = pcall(function() c:CaptureFlag(squad, flagName) end)
		if ok then
			log("order_capture", tostring(squad), flagName)
			return
		end
	end
	if c.SeekAndDestroy then
		pcall(function() c:SeekAndDestroy(squad) end)
		log("order_seek", tostring(squad))
	end
end

local function orderNewSquads()
	local sc = scene()
	if not sc or type(sc.Squads) ~= "table" then return end
	for _, squad in pairs(sc.Squads) do
		local key = tostring(squad)
		if not state.ordered[key] then
			state.ordered[key] = true
			orderSquad(squad)
		end
	end
end

local MIRROR_QUANTS = 200

local function mirrorEngineState()
	emit("mirror", "q", state.quant,
		"faction_support_army", readVar("faction_support_army"))
	emit("mirror", "attack_support",
		"armed", readVar("attack_support_armed"),
		"wave_num", readVar("attack_support_wave_num"),
		"waves_left", readVar("attack_support_waves_left"))
	emit("mirror", "enemy_defense",
		"armed", readVar("enemy_defense_armed"),
		"wave_num", readVar("enemy_defense_wave_num"),
		"waves_left", readVar("enemy_defense_waves_left"),
		"garrison_place", readVar("enemy_defense_place"),
		"garrison_group", readVar("enemy_defense_group"))
	emit("mirror", "defense_support",
		"armed", readVar("defense_support_armed"),
		"wave_num", readVar("defense_support_wave_num"),
		"waves_left", readVar("defense_support_waves_left"))
	emit("mirror", "enemy_attack",
		"armed", readVar("enemy_attack_armed"),
		"wave_num", readVar("enemy_attack_wave_num"),
		"waves_left", readVar("enemy_attack_waves_left"))
end

local function onGameStart()
	state.quant = 0
	state.ordered = {}
	state.identityPublished = false
	state.attackMission = nil
	local id = identity()
	log("game_start", "playerId", id.playerId, "attacking", tostring(id.attacking), "army", id.army)
	publishIdentity(id, false)
	if id.attacking == true then
		log("mode", "mi_wave_delivery", "lua_spawn", "disabled_av_safe")
	elseif id.attacking == false then
		log("mode", "idle_not_attacking")
	else
		log("mode", "role_unresolved_retry_pending")
	end
end

local function onQuant()
	state.quant = state.quant + 1
	if not state.identityPublished and state.attackMission ~= false then
		publishIdentity(identity(), true)
	end
	orderNewSquads()
	if state.quant > 0 and state.quant % 400 == 0 then
		local sc = scene()
		if sc and type(sc.Squads) == "table" then
			for _, squad in pairs(sc.Squads) do
				orderSquad(squad)
			end
		end
	end
	if DEBUG_LOG and state.quant % 200 == 0 then
		log("heartbeat", "q", state.quant)
	end
	if state.quant % MIRROR_QUANTS == 0 then
		mirrorMotor()
		mirrorEngineState()
	end
end

local function onGameEnd()
	log("game_end", "q", state.quant)
end

local function safeEvent(name, fn)
	return function(...)
		local ok, err = pcall(fn, ...)
		if not ok then
			log("event_error", name, tostring(err))
		end
	end
end

local id0 = identity()
log("module_loaded", "playerId", id0.playerId, "team", id0.team, "attacking", tostring(id0.attacking))

local ev = events()
if ev and ev.Subscribe then
	ev:Subscribe(ev.GameStart, safeEvent("GameStart", onGameStart))
	ev:Subscribe(ev.Quant, safeEvent("Quant", onQuant))
	ev:Subscribe(ev.GameEnd, safeEvent("GameEnd", onGameEnd))
	log("armed", "identity_orders_mi_waves")
else
	log("not_armed", "BotApi.Events_missing")
end
