-- Attack support controller (human ATTACK missions).
-- Identity + orders only. Do NOT require utility.lua / logic/main.lua here:
-- that path AVs on the attack support slot (no spawn deck) even with spawnPoint nil-guard
-- (proven 2026-07-29 log: crash in lua.event.notify2 right after Loading utility.lua).
--
-- Unit delivery is MI: attack_support_waves.inc (real-breed pool, MOVE in).
-- Lua Spawn is not viable on this slot (IsUnitAvailable always false; utility load crashes).
-- No enable var gates this: attack support is on by default on every human attack
-- mission, and publishing the identity below is what arms the MI wave engine.

local PREFIX = "CODEX_ATTACK_SUPPORT"

local DEBUG_LOG = true
local function log(...)
	if not DEBUG_LOG then return end
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

local function events()
	return (BotApi and BotApi.Events) or nil
end

local function cmds()
	return (BotApi and BotApi.Commands) or nil
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

local state = {
	quant = 0,
	ordered = {},
}

local function publishIdentity(id)
	if id.attacking ~= true then return end
	local sc = scene()
	if not sc or not sc.SetVar then
		log("identity_publish_skipped", "Scene.SetVar_missing")
		return
	end
	sc:SetVar("id_attack_support", id.playerId)
	sc:SetVar("attack_support_ready", 1)
	-- MI waves are the working delivery path for attack support units.
	sc:SetVar("attack_support_use_mi", 1)
	log("identity_published", "id_attack_support", id.playerId, "mi_waves", 1)
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

local function onGameStart()
	local id = identity()
	log("game_start", "playerId", id.playerId, "attacking", tostring(id.attacking), "army", id.army)
	publishIdentity(id)
	state.ordered = {}
	if id.attacking == true then
		log("mode", "mi_wave_delivery", "lua_spawn", "disabled_av_safe")
	else
		log("mode", "idle_not_attacking")
	end
end

local function onQuant()
	state.quant = state.quant + 1
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
