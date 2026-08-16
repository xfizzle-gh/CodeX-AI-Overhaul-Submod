-- Attack support controller (human ATTACK missions).
-- Identity + TMAI-referenced battlefield command only. Do NOT require utility.lua /
-- logic/main.lua here: that path AVs on the attack support slot (no spawn deck), even
-- with a spawnPoint nil-guard (proven 2026-07-29 log: crash in lua.event.notify2).
--
-- Unit delivery remains MI: attack_support_waves.inc (real-breed pool, MOVE in).
-- Lua Spawn is not viable on this slot (IsUnitAvailable always false; utility load
-- crashes). Mission participation stays gated in MI by support_mission_enabled$.
--
-- Battlefield command is intentionally modeled on the useful parts of TMAI v0.17:
-- settle before first tasking, managed groups, distinct objective distribution,
-- recently-lost counterattack priority, captured-point holds, a small reserve, and
-- suppression of redundant move orders. We keep the already-proven BotApi
-- CaptureFlag/SeekAndDestroy transport rather than invent TMAI's Lua->MI strategy bus
-- on a slot where that exact interface has not been established.
--
-- This slot also carries the ENGINE-STATE MIRROR. Every MIRROR_QUANTS quants it writes
-- one game.log line per wave engine plus faction_support_army$.

local PREFIX = "CODEX_ATTACK_SUPPORT"
local COMMANDER_PREFIX = "CODEX_TMAI_SUPPORT"
local DEBUG_LOG = true

local function emitWithPrefix(prefix, ...)
	local out = { prefix .. ":" }
	for n = 1, select("#", ...) do
		out[#out + 1] = tostring(select(n, ...))
	end
	print(table.concat(out, " "))
end

local function emit(...)
	emitWithPrefix(PREFIX, ...)
end

local function log(...)
	if not DEBUG_LOG then return end
	emit(...)
end

local function commanderLog(...)
	if not DEBUG_LOG then return end
	emitWithPrefix(COMMANDER_PREFIX, ...)
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
		enemyTeam = tostring(i.enemyTeam or ""),
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

-- TMAI v0.17 waits roughly three seconds after a transferred unit appears before
-- tasking it. The engine's own millisecond QuantTimer gives us the same lifecycle
-- without loading utility.lua on this fragile slot.
local TMAI_SETTLE_MS = 3000
local COMMANDER_SCAN_QUANTS = 20
local MAX_CAPTURE_HOLD_GROUPS = 4
local MAX_RESERVE_GROUPS = 1

local state = {
	quant = 0,
	identityPublished = false,
	attackMission = nil,
	generation = 0,
	managed = {},
	flagHistory = {},
	recentlyLost = {},
	newlyCaptured = {},
	planDirty = false,
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

local function flagRelation(flag, id)
	local occupant = tostring((flag and flag.occupant) or "")
	if id.team ~= "" and occupant == id.team then return "friendly" end
	if id.enemyTeam ~= "" and occupant == id.enemyTeam then return "enemy" end
	return "neutral"
end

local function updateFlagState()
	local sc = scene()
	if not sc or type(sc.Flags) ~= "table" then return {} end
	local id = identity()
	local flags = {}
	local present = {}
	for _, flag in pairs(sc.Flags) do
		if flag and flag.name then
			local name = tostring(flag.name)
			local relation = flagRelation(flag, id)
			present[name] = true
			local previous = state.flagHistory[name]
			if previous and previous == "friendly" and relation ~= "friendly" then
				state.recentlyLost[name] = true
				state.newlyCaptured[name] = nil
				state.planDirty = true
				commanderLog("flag_lost", name, "to", relation)
			elseif previous and previous ~= "friendly" and relation == "friendly" then
				state.recentlyLost[name] = nil
				state.newlyCaptured[name] = true
				state.planDirty = true
				commanderLog("flag_captured", name, "from", previous)
			elseif relation == "friendly" then
				state.recentlyLost[name] = nil
			end
			if previous and previous ~= relation then
				state.planDirty = true
			end
			state.flagHistory[name] = relation
			flags[#flags + 1] = { name = name, relation = relation }
		end
	end
	for name, _ in pairs(state.flagHistory) do
		if not present[name] then
			state.flagHistory[name] = nil
			state.recentlyLost[name] = nil
			state.newlyCaptured[name] = nil
			state.planDirty = true
		end
	end
	return flags
end

local function sortedManagedSettled()
	local entries = {}
	for _, entry in pairs(state.managed) do
		if entry.settled then entries[#entries + 1] = entry end
	end
	table.sort(entries, function(a, b) return a.key < b.key end)
	return entries
end

local function sortAttackFlags(flags)
	local relationRank = { enemy = 1, neutral = 2 }
	table.sort(flags, function(a, b)
		local al = state.recentlyLost[a.name] == true
		local bl = state.recentlyLost[b.name] == true
		if al ~= bl then return al end
		local ar = relationRank[a.relation] or 9
		local br = relationRank[b.relation] or 9
		if ar ~= br then return ar < br end
		return a.name < b.name
	end)
end

local function sortFriendlyHoldFlags(flags)
	table.sort(flags, function(a, b)
		local an = state.newlyCaptured[a.name] == true
		local bn = state.newlyCaptured[b.name] == true
		if an ~= bn then return an end
		return a.name < b.name
	end)
end

local function issueFlagOrder(entry, flagName, role)
	if entry.lastRole == role and entry.lastTarget == flagName then
		return false
	end
	local c = cmds()
	if not c then
		commanderLog("order_failed", entry.key, role, flagName, "Commands_missing")
		return false
	end
	if c.CaptureFlag then
		local ok, result = pcall(function() return c:CaptureFlag(entry.squad, flagName) end)
		if ok and result ~= false then
			entry.lastRole = role
			entry.lastTarget = flagName
			entry.orderCount = (entry.orderCount or 0) + 1
			commanderLog("order", entry.key, role, flagName, "transport", "CaptureFlag",
				"count", entry.orderCount)
			return true
		end
		commanderLog("capture_failed", entry.key, role, flagName, tostring(result))
	end
	if c.SeekAndDestroy then
		local ok, result = pcall(function() return c:SeekAndDestroy(entry.squad) end)
		if ok and result ~= false then
			-- Remember the intended assignment even when the transport falls back so a
			-- periodic scan does not spam SeekAndDestroy every few quants.
			entry.lastRole = role
			entry.lastTarget = flagName
			entry.orderCount = (entry.orderCount or 0) + 1
			commanderLog("order", entry.key, role, flagName, "transport", "SeekAndDestroy_fallback",
				"count", entry.orderCount)
			return true
		end
		commanderLog("seek_failed", entry.key, role, flagName, tostring(result))
	end
	return false
end

local function setReserve(entry)
	if entry.lastRole == "reserve" then return end
	entry.lastRole = "reserve"
	entry.lastTarget = nil
	commanderLog("reserve", entry.key, "hold_current_position")
end

local function assignDesired(desired, entry, role, target)
	desired[entry.key] = { role = role, target = target }
end

local function buildPlan(flags, entries)
	local attackFlags = {}
	local newlyCapturedFriendly = {}
	for _, flag in ipairs(flags) do
		if flag.relation ~= "friendly" then
			attackFlags[#attackFlags + 1] = flag
		elseif state.newlyCaptured[flag.name] then
			newlyCapturedFriendly[#newlyCapturedFriendly + 1] = flag
		end
	end
	sortAttackFlags(attackFlags)
	sortFriendlyHoldFlags(newlyCapturedFriendly)

	local desired = {}
	local nextEntry = 1

	-- TMAI-style first pass: spread independent command groups across distinct
	-- capturable objectives instead of bunching every support squad onto one flag.
	for _, flag in ipairs(attackFlags) do
		local entry = entries[nextEntry]
		if not entry then break end
		assignDesired(desired, entry, state.recentlyLost[flag.name] and "counterattack" or "attack", flag.name)
		nextEntry = nextEntry + 1
	end

	-- TMAI explicitly leaves infantry behind on newly captured ground. We cannot
	-- safely classify infantry vs vehicles on this stripped-down bot slot, so the
	-- delivered MI squad remains the atomic command group and holds are capped.
	local holdCount = 0
	if #newlyCapturedFriendly > 0 then
		for _, flag in ipairs(newlyCapturedFriendly) do
			while nextEntry <= #entries and holdCount < MAX_CAPTURE_HOLD_GROUPS do
				assignDesired(desired, entries[nextEntry], "hold", flag.name)
				nextEntry = nextEntry + 1
				holdCount = holdCount + 1
				-- Spread holds before stacking a second group on the same point.
				if holdCount < #newlyCapturedFriendly then break end
			end
		end
	end

	-- Keep one uncommitted group when possible, matching TMAI's reserve concept.
	local remaining = #entries - nextEntry + 1
	local reserveCount = math.min(MAX_RESERVE_GROUPS, math.max(0, remaining))
	local activeRemaining = remaining - reserveCount

	-- If groups remain after the distinct-objective pass and captured-point holds,
	-- reinforce capturable objectives by least current load. This prevents a large
	-- support wave from idling while still keeping one true reserve.
	local reinforceLoads = {}
	for _, flag in ipairs(attackFlags) do reinforceLoads[flag.name] = 1 end
	while activeRemaining > 0 and #attackFlags > 0 and nextEntry <= #entries do
		local best = attackFlags[1]
		for _, flag in ipairs(attackFlags) do
			if (reinforceLoads[flag.name] or 0) < (reinforceLoads[best.name] or 0) then best = flag end
		end
		assignDesired(desired, entries[nextEntry], "reinforce", best.name)
		reinforceLoads[best.name] = (reinforceLoads[best.name] or 0) + 1
		nextEntry = nextEntry + 1
		activeRemaining = activeRemaining - 1
	end

	while nextEntry <= #entries do
		assignDesired(desired, entries[nextEntry], "reserve", nil)
		nextEntry = nextEntry + 1
	end
	return desired, #attackFlags, holdCount, reserveCount
end

local function applyCommanderPlan(reason)
	if state.attackMission ~= true then return end
	local flags = updateFlagState()
	local entries = sortedManagedSettled()
	local desired, attackCount, holdCount, reserveCount = buildPlan(flags, entries)
	for _, entry in ipairs(entries) do
		local d = desired[entry.key]
		if d then
			if d.role == "reserve" then
				setReserve(entry)
			else
				issueFlagOrder(entry, d.target, d.role)
			end
		end
	end
	state.planDirty = false
	commanderLog("plan", reason, "groups", #entries, "capturable", attackCount,
		"holds", holdCount, "reserve", reserveCount)
end

local function settleManaged(key, generation)
	if generation ~= state.generation then return end
	local entry = state.managed[key]
	if not entry then return end
	entry.settled = true
	state.planDirty = true
	commanderLog("settled", key, "after_ms", TMAI_SETTLE_MS)
	applyCommanderPlan("settled")
end

local function scheduleSettle(entry)
	local ev = events()
	local generation = state.generation
	if ev and ev.SetQuantTimer then
		local ok, timer = pcall(function()
			return ev:SetQuantTimer(function() settleManaged(entry.key, generation) end, TMAI_SETTLE_MS)
		end)
		if ok then
			entry.settleTimer = timer
			commanderLog("discovered", entry.key, "settle_ms", TMAI_SETTLE_MS)
			return
		end
		commanderLog("settle_timer_failed", entry.key, tostring(timer))
	end
	-- Fail open to battlefield control if this stripped slot ever lacks timers.
	entry.settled = true
	state.planDirty = true
	commanderLog("settled", entry.key, "fallback", "timer_unavailable")
end

local function discoverAndPruneSquads()
	local sc = scene()
	if not sc or type(sc.Squads) ~= "table" then return end
	local current = {}
	local changed = false
	for _, squad in pairs(sc.Squads) do
		local key = tostring(squad)
		current[key] = true
		if not state.managed[key] then
			local entry = {
				key = key,
				squad = squad,
				settled = false,
				lastRole = nil,
				lastTarget = nil,
				orderCount = 0,
			}
			state.managed[key] = entry
			changed = true
			scheduleSettle(entry)
		end
	end
	for key, _ in pairs(state.managed) do
		if not current[key] then
			state.managed[key] = nil
			changed = true
			commanderLog("pruned", key, "not_in_scene_squads")
		end
	end
	if changed then state.planDirty = true end
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
	state.identityPublished = false
	state.attackMission = nil
	state.generation = state.generation + 1
	state.managed = {}
	state.flagHistory = {}
	state.recentlyLost = {}
	state.newlyCaptured = {}
	state.planDirty = false
	local id = identity()
	log("game_start", "playerId", id.playerId, "attacking", tostring(id.attacking), "army", id.army)
	publishIdentity(id, false)
	if id.attacking == true then
		log("mode", "mi_wave_delivery", "lua_spawn", "disabled_av_safe")
		commanderLog("armed", "reference", "TMAI_v0.17", "settle_ms", TMAI_SETTLE_MS,
			"transport", "CaptureFlag_with_SeekAndDestroy_fallback")
		updateFlagState()
		discoverAndPruneSquads()
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
	if state.attackMission == true and state.quant % COMMANDER_SCAN_QUANTS == 0 then
		discoverAndPruneSquads()
		updateFlagState()
		if state.planDirty then applyCommanderPlan("state_change") end
	end
	if DEBUG_LOG and state.quant % 200 == 0 then
		local managedCount = 0
		local settledCount = 0
		for _, entry in pairs(state.managed) do
			managedCount = managedCount + 1
			if entry.settled then settledCount = settledCount + 1 end
		end
		log("heartbeat", "q", state.quant)
		commanderLog("heartbeat", "q", state.quant, "managed", managedCount, "settled", settledCount)
	end
	if state.quant % MIRROR_QUANTS == 0 then
		mirrorMotor()
		mirrorEngineState()
	end
end

local function onGameEnd()
	log("game_end", "q", state.quant)
	commanderLog("game_end", "q", state.quant)
end

local function safeEvent(name, fn)
	return function(...)
		local ok, err = pcall(fn, ...)
		if not ok then
			log("event_error", name, tostring(err))
			commanderLog("event_error", name, tostring(err))
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
	log("armed", "identity_tmai_commander_mi_waves")
else
	log("not_armed", "BotApi.Events_missing")
end
