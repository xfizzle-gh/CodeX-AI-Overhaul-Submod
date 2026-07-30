-- Attack support controller (human ATTACK missions).
-- Identity + orders only. Do NOT require utility.lua / logic/main.lua here:
-- that path AVs on the attack support slot (no spawn deck) even with spawnPoint nil-guard
-- (proven 2026-07-29 log: crash in lua.event.notify2 right after Loading utility.lua).
--
-- Unit delivery is MI: attack_support_waves.inc (real-breed pool, MOVE in).
-- Lua Spawn is not viable on this slot (IsUnitAvailable always false; utility load crashes).
-- No enable var gates this: attack support is on by default on every human attack
-- mission, and publishing the identity below is what arms the MI wave engine.
--
-- This slot also carries the ENGINE-STATE MIRROR. Every MIRROR_QUANTS quants it writes
-- one game.log line per wave engine - attack_support, enemy_defense (plus its garrison
-- anchors), defense_support, enemy_attack - and the resolved faction_support_army$.
-- Always on and log-only, because the on-screen diagnostics in those engines are gated
-- behind support_debug$ and default to off, so the log is all a shipped run leaves
-- behind. Reads go through readVar, which pcall-guards GetVar.

local PREFIX = "CODEX_ATTACK_SUPPORT"

local DEBUG_LOG = true

-- Two writers: emit always writes, log is the DEBUG_LOG-gated chatter. The engine-state
-- mirror below goes through the ungated one because it is the only remaining way to read
-- the four wave engines on a shipped build - their timers are gated on support_debug$.
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

-- GetVar is not proven on this BotApi surface, and this slot is the one that AVs when
-- a native getter is touched wrong. So every read is pcall-guarded and degrades to a
-- printable placeholder rather than taking the report path - or the process - down:
--   "na"  no Scene at all
--   "err" the guarded GetVar raised
--   "nil" the var read back as nil (undeclared, or never written)
-- This is the probe-era pattern verbatim; it is the only var read ever proven here.
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


-- Motorized insert budget (cmd 19). MI-owned; mirrored for log diagnostics.
local function mirrorMotor()
	-- Both sides: the friendly vars stay 0 on defence missions and vice versa, so a
	-- one-sided mirror is blind on half the campaign. Report both plus the test gates.
	emit("motor_left", readVar("attack_support_motor_left"),
		"wave_cmd", readVar("attack_support_wave_cmd"),
		"test", readVar("attack_support_motor_test"),
		"test_done", readVar("attack_support_motor_test_done"))
	-- WHERE a batch went, not just that a wave fired. place$: 0 = map-edge entry pad,
	-- 1/2/3 = active flag (garrison). entry_rr$ names the pad used for edge batches.
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
		"e2_combo_helo_fail", readVar("support_e2_combo_helo_fail"),
		"e2_lz", readVar("support_e2_lz"),
		"e2_flag", readVar("support_e2_flag"))
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

-- ENGINE-STATE MIRROR. One line per wave engine into game.log every MIRROR_QUANTS
-- quants, always on. The on-screen diagnostics are gated behind support_debug$ so a
-- player sees nothing, which leaves the log as the only place a run can be read back:
-- whether each engine armed, how far into its budget it is, and which faction pool the
-- friendly waves are drawing from. This slot is loaded on every campaign_capture_the_flag
-- mission, attack or defence, so all four quadrants report from the same place.
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
