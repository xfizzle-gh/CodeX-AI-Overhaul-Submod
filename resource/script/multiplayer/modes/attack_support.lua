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
local ARM_RETRY_QUANTS = 10
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

-- Native crash report 2026-08-16 disproved the QueryScene polling approach:
-- the Mate was player 1 / FirstPlayerId 1, QueryScene returned no human candidate,
-- and repeated native QueryScene calls every ~0.2s were the only #101-specific native
-- operation still executing before the process terminated without a Lua exception.
--
-- We already have direct live topology proof in bot.main.lua and game.log that this
-- single-player Conquest layout uses four combat player slots:
--   Mate = 1, enemy = 2, human = 3, DefenderBot = 4.
-- Rather than hard-code player 3, resolve the human as the ONE missing ID from 1..4
-- after accounting for the Mate, enemy, and DefenderBot. This tolerates permutations
-- of those three engine-owned IDs, avoids native scene probing entirely, and fails
-- closed if the runtime topology is not exactly the proven four-slot shape.
local function resolveHumanId(id)
	local known = {
		{ name = "mate", value = tonumber(id.playerId or 0) or 0 },
		{ name = "enemy", value = tonumber(id.firstEnemyId or 0) or 0 },
		{ name = "defender", value = tonumber(id.defenderBotId or 0) or 0 },
	}
	local occupied = {}
	for _, item in ipairs(known) do
		local playerId = item.value
		if playerId < 1 or playerId > 4 then
			return 0, "four_slot_" .. item.name .. "_out_of_range=" .. tostring(playerId)
		end
		if occupied[playerId] then
			return 0, "four_slot_duplicate_id=" .. tostring(playerId)
		end
		occupied[playerId] = true
	end

	local humanId = 0
	for playerId = 1, 4 do
		if not occupied[playerId] then
			if humanId ~= 0 then
				return 0, "four_slot_multiple_candidates"
			end
			humanId = playerId
		end
	end
	if humanId <= 0 then
		return 0, "four_slot_no_candidate"
	end
	return humanId, "campaign_four_slot_complement"
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
	if state.attackMission ~= false and not state.armed and state.quant % ARM_RETRY_QUANTS == 0 then
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
