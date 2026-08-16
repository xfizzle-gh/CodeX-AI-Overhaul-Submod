-- Attack support controller (human ATTACK missions).
--
-- PR #101 changes the lifecycle to match the useful TMAI v0.17 pattern instead of
-- merely changing final ownership:
--
--   parked support pool -> REAL HUMAN owner before waves arm
--   -> visible deployed batch remains human-owned / user-control briefly
--   -> automatic transfer to the extra Team-A Mate
--   -> TMAI settle period
--   -> MI action move orders from attack_support_tmai_handoff.inc
--
-- The support units are still supplied by our separate support pool, not purchased
-- from or charged to the human Dynamic Conquest roster. The Mate still has no economy
-- or purchase stack. Do NOT require utility.lua / logic/main.lua on this slot: that
-- path native-crashed on the custom support process.
--
-- This module intentionally does NOT issue BotApi CaptureFlag/SeekAndDestroy orders.
-- TMAI's relevant architecture is Lua strategy/identity -> MI movement execution, so
-- PR #101 gives the mission-script bridge sole authority for the handoff and orders.

local PREFIX = "CODEX_ATTACK_SUPPORT"
local HANDOFF_PREFIX = "CODEX_TMAI_HANDOFF"
local DEBUG_LOG = true
local HUMAN_DISCOVERY_QUANTS = 10
local MIRROR_QUANTS = 200

local function emitWithPrefix(prefix, ...)
	local out = { prefix .. ":" }
	for n = 1, select("#", ...) do
		out[#out + 1] = tostring(select(n, ...))
	end
	print(table.concat(out, " "))
end

local function log(...)
	if DEBUG_LOG then emitWithPrefix(PREFIX, ...) end
end

local function handoffLog(...)
	if DEBUG_LOG then emitWithPrefix(HANDOFF_PREFIX, ...) end
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

local function readVarNumber(name)
	local sc = scene()
	if not sc or not sc.GetVar then return 0 end
	local ok, value = pcall(function() return sc:GetVar(name) end)
	if not ok then return 0 end
	return tonumber(value or 0) or 0
end

local function setVar(name, value)
	local sc = scene()
	if not sc or not sc.SetVar then return false end
	local ok, err = pcall(function() sc:SetVar(name, value) end)
	if not ok then
		handoffLog("setvar_failed", name, tostring(err))
		return false
	end
	return true
end

local function identity()
	local i = instance()
	local c = conquestApi()
	return {
		playerId = tonumber(i.playerId or 0) or 0,
		team = tostring(i.team or ""),
		enemyTeam = tostring(i.enemyTeam or ""),
		army = tostring(i.army or ""),
		difficulty = tostring(i.difficulty or ""),
		gameMode = tostring(i.gameMode or ""),
		attacking = c.Attacking,
		firstPlayerId = positiveId(c.FirstPlayerId, i.CampaignFirstPlayerId),
		firstEnemyId = positiveId(c.FirstEnemyId, i.CampaignFirstEnemyId),
		defenderBotId = positiveId(c.DefenderBotId, i.CampaignDefenderBotId),
		hostId = tonumber(i.hostId or 0) or 0,
	}
end

local state = {
	quant = 0,
	attackMission = nil,
	armed = false,
	humanId = 0,
	mateId = 0,
	lastWait = nil,
	lastSeq = -1,
}

local function excludedHumanCandidate(playerId, id)
	if playerId <= 0 then return true end
	if playerId == id.playerId then return true end
	if id.firstEnemyId > 0 and playerId == id.firstEnemyId then return true end
	if id.defenderBotId > 0 and playerId == id.defenderBotId then return true end
	return false
end

-- The extra Team-A Mate can occupy Conquest.FirstPlayerId, so FirstPlayerId by itself
-- is not a safe human identity while {aiTeamPlayers 1} exists. We use the same
-- BotApi.Scene:QueryScene primitive already used by shipped utility.lua, but directly
-- and narrowly: locate live player IDs with soldiers, then exclude every known bot ID.
-- In normal single-player Dynamic Conquest this leaves exactly the human commander.
local function queryHumanCandidates(id)
	local sc = scene()
	if not sc or not sc.QueryScene then
		return {}, "QueryScene_missing"
	end
	local ok, result = pcall(function()
		return sc:QueryScene({"soldier"}, 5)
	end)
	if not ok or type(result) ~= "table" then
		return {}, ok and "QueryScene_bad_result" or ("QueryScene_error:" .. tostring(result))
	end

	local candidates = {}
	for rawPlayerId, bucket in pairs(result) do
		local playerId = tonumber(rawPlayerId or 0) or 0
		local counts = type(bucket) == "table" and bucket[2] or nil
		local soldiers = type(counts) == "table" and (tonumber(counts[1] or 0) or 0) or 0
		if soldiers > 0 and not excludedHumanCandidate(playerId, id) then
			candidates[#candidates + 1] = { id = playerId, soldiers = soldiers }
		end
	end
	table.sort(candidates, function(a, b)
		if a.soldiers ~= b.soldiers then return a.soldiers > b.soldiers end
		return a.id < b.id
	end)
	return candidates, "ok"
end

local function resolveHumanId(id)
	local candidates, reason = queryHumanCandidates(id)
	if #candidates == 0 then return 0, reason end

	-- Prefer a candidate corroborated by the engine's FirstPlayerId when that field
	-- is actually human in this topology.
	for _, candidate in ipairs(candidates) do
		if id.firstPlayerId > 0 and candidate.id == id.firstPlayerId then
			return candidate.id, "query+FirstPlayerId"
		end
	end

	-- hostId is only used as corroboration, never as a blind ownership fallback.
	for _, candidate in ipairs(candidates) do
		if id.hostId > 0 and candidate.id == id.hostId then
			return candidate.id, "query+hostId"
		end
	end

	if #candidates == 1 then
		return candidates[1].id, "single_nonbot_soldier_owner"
	end

	local parts = {}
	for _, candidate in ipairs(candidates) do
		parts[#parts + 1] = tostring(candidate.id) .. ":" .. tostring(candidate.soldiers)
	end
	return 0, "ambiguous_candidates=" .. table.concat(parts, ",")
end

local function logWait(reason, id)
	if reason == state.lastWait then return end
	state.lastWait = reason
	handoffLog("wait", reason,
		"mate", id.playerId,
		"firstPlayerId", id.firstPlayerId,
		"firstEnemyId", id.firstEnemyId,
		"defenderBotId", id.defenderBotId,
		"hostId", id.hostId)
end

local function resetMissionVars()
	setVar("attack_support_ready", 0)
	setVar("attack_support_use_mi", 1)
	setVar("id_attack_support", 0)
	setVar("id_attack_support_human", 0)
	setVar("id_attack_support_mate", 0)
	setVar("tmai_handoff_prepare", 0)
	setVar("tmai_handoff_prepared", 0)
	setVar("tmai_handoff_enabled", 0)
	setVar("tmai_handoff_busy", 0)
	setVar("tmai_handoff_seq", 0)
end

local function armHumanOriginHandoff(id)
	if id.attacking == false then
		state.attackMission = false
		return false
	end
	if id.attacking ~= true then
		logWait("role_unresolved", id)
		return false
	end
	state.attackMission = true

	local mateId = positiveId(id.playerId, 0)
	if mateId <= 0 then
		logWait("mate_id_unresolved", id)
		return false
	end

	local humanId, source = resolveHumanId(id)
	if humanId <= 0 then
		logWait("human_id_unresolved:" .. tostring(source), id)
		return false
	end
	if humanId == mateId then
		logWait("human_equals_mate_rejected", id)
		return false
	end

	state.humanId = humanId
	state.mateId = mateId
	setVar("id_attack_support_human", humanId)
	setVar("id_attack_support_mate", mateId)

	-- Existing attack_support_waves.inc calls am_own_to_support during activation.
	-- Deliberately point that FIRST pass at the human. The MI handoff bridge later
	-- transfers only the deployed attack_support_src batch to the Mate.
	setVar("id_attack_support", humanId)
	setVar("tmai_handoff_prepare", 1)

	-- Fail closed until MI confirms that the hidden/inactive pool was assigned to
	-- the human before any support wave can arm.
	if readVarNumber("tmai_handoff_prepared") ~= 1 then
		logWait("waiting_for_human_pool_seed", id)
		return false
	end

	setVar("tmai_handoff_enabled", 1)
	setVar("attack_support_use_mi", 1)
	setVar("attack_support_ready", 1)
	state.armed = true
	state.lastWait = nil
	handoffLog("armed",
		"human", humanId,
		"mate", mateId,
		"human_source", source,
		"first_owner_var", humanId,
		"flow", "human_origin_to_mate_to_MI_action_move")
	return true
end

local function mirrorState()
	log("mirror",
		"human", state.humanId,
		"mate", state.mateId,
		"prepared", readVarNumber("tmai_handoff_prepared"),
		"enabled", readVarNumber("tmai_handoff_enabled"),
		"busy", readVarNumber("tmai_handoff_busy"),
		"seq", readVarNumber("tmai_handoff_seq"),
		"wave_num", readVarNumber("attack_support_wave_num"),
		"waves_left", readVarNumber("attack_support_waves_left"))
end

local function observeHandoffs()
	local seq = readVarNumber("tmai_handoff_seq")
	if seq ~= state.lastSeq then
		if state.lastSeq >= 0 then
			handoffLog("completed", "seq", seq, "human", state.humanId, "mate", state.mateId,
				"order_transport", "MI_action_move")
		end
		state.lastSeq = seq
	end
end

local function onGameStart()
	state.quant = 0
	state.attackMission = nil
	state.armed = false
	state.humanId = 0
	state.mateId = 0
	state.lastWait = nil
	state.lastSeq = -1
	resetMissionVars()
	local id = identity()
	log("game_start", "playerId", id.playerId, "attacking", tostring(id.attacking), "army", id.army)
	armHumanOriginHandoff(id)
end

local function onQuant()
	state.quant = state.quant + 1
	if state.attackMission ~= false and not state.armed and state.quant % HUMAN_DISCOVERY_QUANTS == 0 then
		armHumanOriginHandoff(identity())
	end
	if state.armed then observeHandoffs() end
	if DEBUG_LOG and state.quant % MIRROR_QUANTS == 0 then mirrorState() end
end

local function onGameEnd()
	setVar("tmai_handoff_enabled", 0)
	setVar("attack_support_ready", 0)
	log("game_end", "q", state.quant, "human", state.humanId, "mate", state.mateId,
		"handoffs", readVarNumber("tmai_handoff_seq"))
end

local function safeEvent(name, fn)
	return function(...)
		local ok, err = pcall(fn, ...)
		if not ok then
			log("event_error", name, tostring(err))
			handoffLog("event_error", name, tostring(err))
		end
	end
end

local id0 = identity()
log("module_loaded", "playerId", id0.playerId, "team", id0.team,
	"attacking", tostring(id0.attacking), "firstPlayerId", id0.firstPlayerId,
	"firstEnemyId", id0.firstEnemyId, "defenderBotId", id0.defenderBotId, "hostId", id0.hostId)

local ev = events()
if ev and ev.Subscribe then
	ev:Subscribe(ev.GameStart, safeEvent("GameStart", onGameStart))
	ev:Subscribe(ev.Quant, safeEvent("Quant", onQuant))
	ev:Subscribe(ev.GameEnd, safeEvent("GameEnd", onGameEnd))
	log("armed", "tmai_human_origin_handoff_controller")
else
	log("not_armed", "BotApi.Events_missing")
end
