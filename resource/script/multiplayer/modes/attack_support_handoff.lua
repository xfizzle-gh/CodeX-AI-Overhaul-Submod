-- Attack-support ownership bridge for human ATTACK missions.
--
-- Unlike the old attack_support.lua controller, this module does not require a custom
-- Team-A bot. It runs alongside the normal conquest bot and publishes the human
-- commander's engine-owned FirstPlayerId as the support owner. MI then keeps those
-- units AI-controlled and unselectable after ownership is applied.

local PREFIX = "CODEX_ATTACK_SUPPORT_HANDOFF"

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

local function identity()
	local i = instance()
	local c = conquestApi()
	return {
		playerId = tonumber(i.playerId or 0) or 0,
		team = tostring(i.team or ""),
		attacking = c.Attacking,
		firstPlayerId = positiveId(c.FirstPlayerId, i.CampaignFirstPlayerId),
		firstEnemyId = positiveId(c.FirstEnemyId, i.CampaignFirstEnemyId),
	}
end

local state = {
	published = false,
	quant = 0,
	applicable = nil,
}

local function publish(isRetry)
	local id = identity()

	-- This bridge is intentionally single-writer: only the normal enemy mission
	-- authority (FirstEnemyId) may publish the player handoff.
	if id.firstEnemyId > 0 and id.playerId ~= id.firstEnemyId then
		state.applicable = false
		return false
	end

	-- From the enemy bot's perspective, Attacking=false means the human is attacking
	-- and this bot is defending. Human-defense missions must not arm attack support.
	if id.attacking == true then
		state.applicable = false
		return false
	end
	if id.attacking ~= false then return false end
	state.applicable = true

	local sc = scene()
	if not sc or not sc.SetVar then
		if not isRetry then emit("publish_skipped", "Scene.SetVar_missing") end
		return false
	end
	if id.firstEnemyId <= 0 or id.firstPlayerId <= 0 then
		if not isRetry or state.quant % 50 == 0 then
			emit("publish_skipped", "identity_unresolved",
				"source_playerId", id.playerId,
				"firstEnemyId", id.firstEnemyId,
				"firstPlayerId", id.firstPlayerId,
				"team", id.team,
				"retry", tostring(isRetry == true))
		end
		return false
	end

	sc:SetVar("id_attack_support", id.firstPlayerId)
	sc:SetVar("attack_support_ready", 1)
	sc:SetVar("attack_support_use_mi", 1)
	state.published = true
	emit("published",
		"id_attack_support", id.firstPlayerId,
		"source_playerId", id.playerId,
		"firstEnemyId", id.firstEnemyId,
		"team", id.team,
		"attacking", tostring(id.attacking),
		"retry", tostring(isRetry == true))
	return true
end

local function onGameStart()
	state.published = false
	state.quant = 0
	state.applicable = nil
	publish(false)
end

local function onQuant()
	state.quant = state.quant + 1
	if not state.published and state.applicable ~= false then
		publish(true)
	end
end

local function safeEvent(name, fn)
	return function(...)
		local ok, err = pcall(fn, ...)
		if not ok then emit("event_error", name, tostring(err)) end
	end
end

local ev = events()
if ev and ev.Subscribe then
	ev:Subscribe(ev.GameStart, safeEvent("GameStart", onGameStart))
	ev:Subscribe(ev.Quant, safeEvent("Quant", onQuant))
	emit("armed")
	-- bot.main.lua can require this module while the engine is already dispatching
	-- GameStart. Subscribers added mid-dispatch are not guaranteed to receive that
	-- same event, so bootstrap immediately from the already-populated Conquest IDs.
	-- If identity is not settled yet, onQuant keeps retrying as before.
	publish(false)
else
	emit("not_armed", "BotApi.Events_missing")
end
