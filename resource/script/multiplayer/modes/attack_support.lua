-- Native Mate spawn/FoW experiment.
--
-- Fresh from main. No parked Player-0 support actors, no MI support-wave arming,
-- no ownership permutation. The extra Team-A Mate attempts one genuine BotApi
-- SpawnAt/Spawn request for an existing RUSA Conquest unit ID and records the
-- GameSpawn event. This answers whether a unit born directly on the Mate through
-- the native spawn API participates in the human team's terrain-FoW mask.

local PREFIX = "CODEX_MATE_NATIVE_SPAWN"
local START_DELAY_MS = 3000
local ORDER_DELAY_MS = 3000
local MAX_SQUAD_SIZE = 7

local RUSA_CANDIDATES = {
    "rus90_inf_rifle(rusa)",
    "rus4_inf_rifle_rpg27(rusa)",
    "lud_22_1(rusa)",
}

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

local function events()
    return (BotApi and BotApi.Events) or nil
end

local function commands()
    return (BotApi and BotApi.Commands) or nil
end

local state = {
    generation = 0,
    armed = false,
    attempted = false,
    awaitingSpawn = false,
    requestedUnit = nil,
    spawnedSquad = nil,
}

local function safeCall(label, fn)
    local ok, result = pcall(fn)
    if not ok then
        emit(label, "lua_error", tostring(result))
        return false, nil
    end
    return true, result
end

local function unitAvailability(cmd, unit)
    if not cmd or not cmd.IsUnitAvailable then
        emit("unit_check", unit, "IsUnitAvailable", "api_missing")
        return nil
    end
    local ok, value = safeCall("unit_check_error", function()
        return cmd:IsUnitAvailable(unit)
    end)
    if not ok then return nil end
    emit("unit_check", unit, "IsUnitAvailable", tostring(value))
    return value == true
end

local function tryNativeSpawn(unit)
    local cmd = commands()
    if not cmd then
        emit("spawn_failed", unit, "Commands_missing")
        return false
    end

    state.awaitingSpawn = true
    state.requestedUnit = unit

    if cmd.SpawnAt then
        local ok, result = safeCall("SpawnAt_error", function()
            return cmd:SpawnAt(unit, MAX_SQUAD_SIZE, 0)
        end)
        emit("SpawnAt", "unit", unit, "ok", tostring(ok), "result", tostring(result))
        if ok and result == true then return true end
    else
        emit("SpawnAt", "unit", unit, "api_missing")
    end

    if cmd.Spawn then
        local ok, result = safeCall("Spawn_error", function()
            return cmd:Spawn(unit, MAX_SQUAD_SIZE)
        end)
        emit("Spawn", "unit", unit, "ok", tostring(ok), "result", tostring(result))
        if ok and result == true then return true end
    else
        emit("Spawn", "unit", unit, "api_missing")
    end

    state.awaitingSpawn = false
    state.requestedUnit = nil
    return false
end

local function attemptSpawn(generation)
    if generation ~= state.generation or not state.armed or state.attempted then return end
    state.attempted = true

    local i = instance()
    if tostring(i.army or "") ~= "rusa" then
        emit("gate_skip", "rusa_only", "army", tostring(i.army))
        return
    end

    local cmd = commands()
    emit(
        "spawn_context",
        "playerId", tostring(i.playerId),
        "team", tostring(i.team),
        "army", tostring(i.army),
        "spawnAt", tostring(cmd and cmd.SpawnAt ~= nil),
        "spawn", tostring(cmd and cmd.Spawn ~= nil),
        "gameSpawn_event", tostring(events() and events().GameSpawn ~= nil)
    )

    for _, unit in ipairs(RUSA_CANDIDATES) do
        unitAvailability(cmd, unit)
        emit("spawn_request", "unit", unit, "native_mate", true)
        if tryNativeSpawn(unit) then
            emit("spawn_request_accepted", "unit", unit)
            return
        end
    end

    emit("spawn_failed", "all_candidates_rejected")
end

local function issuePostSpawnOrder(generation, squad)
    if generation ~= state.generation or squad ~= state.spawnedSquad then return end
    local cmd = commands()
    if cmd and cmd.SeekAndDestroy then
        local ok = safeCall("order_error", function()
            cmd:SeekAndDestroy(squad)
            return true
        end)
        emit("order", "SeekAndDestroy", "squadId", tostring(squad), "ok", tostring(ok))
    else
        emit("order", "SeekAndDestroy", "api_missing")
    end
end

local function onGameSpawn(args)
    if not state.armed or not state.awaitingSpawn or state.spawnedSquad then return end
    if not args or not args.squadId then
        emit("GameSpawn", "missing_squadId")
        return
    end

    local squad = args.squadId
    state.spawnedSquad = squad
    state.awaitingSpawn = false
    emit(
        "GameSpawn",
        "requestedUnit", tostring(state.requestedUnit),
        "squadId", tostring(squad),
        "native_birth_confirmed", true
    )

    local ev = events()
    local generation = state.generation
    if ev and ev.SetQuantTimer then
        ev:SetQuantTimer(function() issuePostSpawnOrder(generation, squad) end, ORDER_DELAY_MS)
    else
        issuePostSpawnOrder(generation, squad)
    end
end

local function onGameStart()
    state.generation = state.generation + 1
    state.armed = false
    state.attempted = false
    state.awaitingSpawn = false
    state.requestedUnit = nil
    state.spawnedSquad = nil

    local i = instance()
    local c = conquest()
    emit(
        "GameStart",
        "playerId", tostring(i.playerId),
        "team", tostring(i.team),
        "army", tostring(i.army),
        "attacking", tostring(c.Attacking)
    )

    if c.Attacking ~= true then
        emit("gate_skip", "not_human_attack")
        return
    end
    if tostring(i.team or "") ~= "a" then
        emit("gate_skip", "not_team_a")
        return
    end

    state.armed = true
    emit(
        "armed",
        "mode", "native_mate_spawn",
        "parked_templates", "disabled",
        "mi_support_waves", "disabled",
        "ownership_transfer", "disabled"
    )

    local ev = events()
    local generation = state.generation
    if ev and ev.SetQuantTimer then
        ev:SetQuantTimer(function() attemptSpawn(generation) end, START_DELAY_MS)
    else
        attemptSpawn(generation)
    end
end

local function onGameEnd()
    emit(
        "GameEnd",
        "attempted", tostring(state.attempted),
        "requestedUnit", tostring(state.requestedUnit),
        "spawnedSquad", tostring(state.spawnedSquad)
    )
end

local function safeEvent(name, fn)
    return function(...)
        local ok, err = pcall(fn, ...)
        if not ok then emit("event_error", name, tostring(err)) end
    end
end

local i = instance()
emit("module_loaded", "playerId", tostring(i.playerId), "team", tostring(i.team), "army", tostring(i.army))
local ev = events()
if ev and ev.Subscribe then
    ev:Subscribe(ev.GameStart, safeEvent("GameStart", onGameStart))
    ev:Subscribe(ev.GameSpawn, safeEvent("GameSpawn", onGameSpawn))
    ev:Subscribe(ev.GameEnd, safeEvent("GameEnd", onGameEnd))
    emit("subscribed")
else
    emit("not_armed", "BotApi.Events_missing")
end
