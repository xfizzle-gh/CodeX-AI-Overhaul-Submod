-- Issue #97: native Mate birth -> terrain FoW proof.
--
-- #102 proved a genuine Dynamic Conquest actor keeps bright terrain FoW after it is
-- handed to the Team-A Mate. #101 proved automatic ownership handoff itself is easy,
-- but also proved that a pre-authored mission actor never acquires native terrain-FoW
-- registration merely by changing owners.
--
-- This experiment attacks only the remaining boundary: actor birth. It gives the Mate
-- one hidden, zero-cost RUSA Conquest definition at research stage 0, verifies the engine
-- says that exact unit is available, and only then requests one native SpawnAt. The old
-- parked Player-0 attack-support wave system is disabled for this run so any visible
-- support squad can only come from the native GameSpawn lifecycle.
--
-- SAFETY: an earlier #103 run hard-crashed native code after SpawnAt was called for an
-- unavailable unit. IsUnitAvailable == true and CanSpawn == true are therefore mandatory
-- before SpawnAt. There is no Spawn fallback and no utility.lua load on this Mate slot.

local PREFIX = "CODEX_NATIVE_STAGE0"
local UNIT = "codex_native_support_stage0(rusa)"
local START_DELAY_MS = 3000
local SETTLE_MS = 3000
local MAX_SQUAD_SIZE = 7

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

local function conquest()
    return (BotApi and BotApi.Conquest) or {}
end

local function scene()
    return (BotApi and BotApi.Scene) or nil
end

local function events()
    return (BotApi and BotApi.Events) or nil
end

local function commands()
    return (BotApi and BotApi.Commands) or nil
end

local function safeCall(label, fn)
    local ok, result = pcall(fn)
    if not ok then
        emit(label, "lua_error", tostring(result))
        return false, nil
    end
    return true, result
end

local function setVar(name, value)
    local sc = scene()
    if not sc or not sc.SetVar then return false end
    local ok = pcall(function() sc:SetVar(name, value) end)
    return ok
end

local function disableLegacyAttackSupport()
    -- No parked support may enter this experiment. Keep #107's no-truck gates too.
    setVar("attack_support_ready", 0)
    setVar("attack_support_use_mi", 0)
    setVar("attack_support_motor_left", 0)
    setVar("attack_support_hmmwv_left", 0)
    setVar("attack_support_motor_test", 0)
    setVar("transport_as_done", 1)
end

local state = {
    generation = 0,
    armed = false,
    attempted = false,
    awaitingSpawn = false,
    spawnedSquad = nil,
    settled = false,
}

local function checkAvailability(cmd)
    if not cmd or not cmd.IsUnitAvailable then
        emit("availability", "api_missing", "native_call", "suppressed")
        return false
    end
    local ok, value = safeCall("availability_error", function()
        return cmd:IsUnitAvailable(UNIT)
    end)
    emit("availability", UNIT, "ok", tostring(ok), "value", tostring(value))
    return ok and value == true
end

local function checkCanSpawn(cmd)
    if not cmd or not cmd.CanSpawn then
        emit("can_spawn", "api_missing", "native_call", "suppressed")
        return false
    end
    -- Current utility.lua calls BotApi.Commands:CanSpawn() with no arguments.
    local ok, value = safeCall("can_spawn_error", function()
        return cmd:CanSpawn()
    end)
    emit("can_spawn", "ok", tostring(ok), "value", tostring(value))
    return ok and value == true
end

local function attemptNativeBirth(generation)
    if generation ~= state.generation or not state.armed or state.attempted then return end
    state.attempted = true
    disableLegacyAttackSupport()

    local i = instance()
    local cmd = commands()
    emit("spawn_context",
        "playerId", tostring(i.playerId),
        "team", tostring(i.team),
        "army", tostring(i.army),
        "unit", UNIT,
        "SpawnAt", tostring(cmd and cmd.SpawnAt ~= nil))

    if not checkAvailability(cmd) then
        emit("birth_failed", "unit_unavailable", "native_call", "suppressed")
        return
    end
    if not checkCanSpawn(cmd) then
        emit("birth_failed", "cannot_spawn", "native_call", "suppressed")
        return
    end
    if not cmd.SpawnAt then
        emit("birth_failed", "SpawnAt_missing")
        return
    end

    state.awaitingSpawn = true
    emit("spawn_request", "unit", UNIT, "maxSquadSize", MAX_SQUAD_SIZE, "spawnPointIndex", 0)
    local ok, value = safeCall("SpawnAt_error", function()
        return cmd:SpawnAt(UNIT, MAX_SQUAD_SIZE, 0)
    end)
    emit("SpawnAt", "ok", tostring(ok), "result", tostring(value))

    if not ok or value ~= true then
        state.awaitingSpawn = false
        emit("birth_failed", "SpawnAt_rejected")
        return
    end
    emit("spawn_request_accepted", "awaiting_GameSpawn", true)
end

local function pickFlagName()
    local sc = scene()
    if not sc or type(sc.Flags) ~= "table" then return nil end
    local names = {}
    for _, flag in pairs(sc.Flags) do
        if flag and flag.name then names[#names + 1] = tostring(flag.name) end
    end
    if #names == 0 then return nil end
    return names[math.random(#names)]
end

local function commandSpawnedSquad(generation, squad)
    if generation ~= state.generation or squad ~= state.spawnedSquad then return end
    state.settled = true
    local cmd = commands()
    if not cmd then
        emit("commander", "commands_missing")
        return
    end

    local flagName = pickFlagName()
    if flagName and cmd.CaptureFlag then
        local ok, result = safeCall("order_error", function()
            return cmd:CaptureFlag(squad, flagName)
        end)
        if ok then
            emit("commander", "settled_ms", SETTLE_MS, "order", "CaptureFlag",
                "squadId", tostring(squad), "target", flagName, "result", tostring(result))
            return
        end
    end

    if cmd.SeekAndDestroy then
        local ok, result = safeCall("order_error", function()
            return cmd:SeekAndDestroy(squad)
        end)
        emit("commander", "settled_ms", SETTLE_MS, "order", "SeekAndDestroy",
            "squadId", tostring(squad), "ok", tostring(ok), "result", tostring(result))
    else
        emit("commander", "no_order_api")
    end
end

local function onGameSpawn(args)
    if not state.armed or not state.awaitingSpawn or state.spawnedSquad then return end
    if not args or not args.squadId then
        emit("GameSpawn", "missing_squadId")
        return
    end

    state.spawnedSquad = args.squadId
    state.awaitingSpawn = false
    emit("GameSpawn", "unit", UNIT, "squadId", tostring(args.squadId),
        "native_birth_confirmed", true, "owner", tostring(instance().playerId))

    local ev = events()
    local generation = state.generation
    local squad = args.squadId
    if ev and ev.SetQuantTimer then
        ev:SetQuantTimer(function() commandSpawnedSquad(generation, squad) end, SETTLE_MS)
    else
        commandSpawnedSquad(generation, squad)
    end
end

local function onGameStart()
    state.generation = state.generation + 1
    state.armed = false
    state.attempted = false
    state.awaitingSpawn = false
    state.spawnedSquad = nil
    state.settled = false
    disableLegacyAttackSupport()

    local i = instance()
    local c = conquest()
    emit("GameStart",
        "playerId", tostring(i.playerId),
        "team", tostring(i.team),
        "army", tostring(i.army),
        "gameMode", tostring(i.gameMode),
        "attacking", tostring(c.Attacking))

    if tostring(i.gameMode or "") ~= "campaign_capture_the_flag" then
        emit("gate_skip", "not_conquest")
        return
    end
    if c.Attacking ~= true then
        emit("gate_skip", "not_human_attack")
        return
    end
    if tostring(i.team or "") ~= "a" then
        emit("gate_skip", "not_team_a")
        return
    end
    if tostring(i.army or "") ~= "rusa" then
        emit("gate_skip", "rusa_proof_only")
        return
    end

    state.armed = true
    emit("armed",
        "native_birth", true,
        "stage0_unit", UNIT,
        "parked_templates", "disabled",
        "manual_transfer", "not_required",
        "settle_ms", SETTLE_MS,
        "availability_fail_closed", true)

    local ev = events()
    local generation = state.generation
    if ev and ev.SetQuantTimer then
        ev:SetQuantTimer(function() attemptNativeBirth(generation) end, START_DELAY_MS)
    else
        attemptNativeBirth(generation)
    end
end

local function onQuant()
    -- Reassert only the isolation gates. No polling, QueryScene, or repeated spawn attempts.
    disableLegacyAttackSupport()
end

local function onGameEnd()
    emit("GameEnd",
        "attempted", tostring(state.attempted),
        "spawnedSquad", tostring(state.spawnedSquad),
        "settled", tostring(state.settled))
end

local function safeEvent(name, fn)
    return function(...)
        local ok, err = pcall(fn, ...)
        if not ok then emit("event_error", name, tostring(err)) end
    end
end

local i = instance()
emit("module_loaded", "playerId", tostring(i.playerId), "team", tostring(i.team), "army", tostring(i.army))
disableLegacyAttackSupport()

local ev = events()
if ev and ev.Subscribe then
    ev:Subscribe(ev.GameStart, safeEvent("GameStart", onGameStart))
    ev:Subscribe(ev.GameSpawn, safeEvent("GameSpawn", onGameSpawn))
    ev:Subscribe(ev.Quant, safeEvent("Quant", onQuant))
    ev:Subscribe(ev.GameEnd, safeEvent("GameEnd", onGameEnd))
    emit("subscribed", "one_shot_native_stage0")
else
    emit("not_armed", "BotApi.Events_missing")
end
