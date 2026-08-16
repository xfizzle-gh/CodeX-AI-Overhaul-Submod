-- Native allied-support birth/FoW probe.
--
-- Goal: create one extra support squad through the Mate's genuine BotApi Conquest
-- spawn path, then let the Mate command that natively registered squad after the
-- same ~3 second settle used by the successful TMAI manual-transfer control.
--
-- This deliberately does NOT use the parked Player-0 support pool and does NOT
-- arm the existing MI support waves. The normal enemy Conquest bot is untouched.
--
-- Safety: #103 proved SpawnAt/Spawn can hard-crash this synthetic Mate when a unit
-- is unavailable. Therefore IsUnitAvailable == true is an absolute gate. There is
-- no forced native call and no fallback around a false/unknown availability result.

local PREFIX = "CODEX_NATIVE_SUPPORT_BIRTH"
local START_DELAY_MS = 3000
local SETTLE_MS = 3000
local MAX_SQUAD_SIZE = 7
local RUSA_UNIT = "codex_native_support_rifle(rusa)"

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

local function setVar(name, value)
    local sc = scene()
    if not sc or not sc.SetVar then return false end
    local ok = pcall(function() sc:SetVar(name, value) end)
    return ok
end

local function failClosedLegacySupport()
    -- This branch is a native-birth isolation test. Keep every pre-authored friendly
    -- support path inert so a visible unit can only come from the GameSpawn below.
    setVar("attack_support_ready", 0)
    setVar("attack_support_use_mi", 0)
    setVar("attack_support_motor_left", 0)
    setVar("attack_support_hmmwv_left", 0)
    setVar("attack_support_motor_test", 0)
    setVar("transport_as_done", 1)
end

local function unitAvailable(cmd)
    if not cmd or not cmd.IsUnitAvailable then
        emit("unit_check", RUSA_UNIT, "IsUnitAvailable", "api_missing")
        return false
    end
    local ok, value = safeCall("unit_check_error", function()
        return cmd:IsUnitAvailable(RUSA_UNIT)
    end)
    if not ok then return false end
    emit("unit_check", RUSA_UNIT, "IsUnitAvailable", tostring(value))
    return value == true
end

local function trySpawn()
    local cmd = commands()
    if not unitAvailable(cmd) then
        emit("spawn_skip", RUSA_UNIT, "reason", "unit_unavailable_or_unknown", "native_call", "suppressed")
        return
    end

    state.awaitingSpawn = true
    state.requestedUnit = RUSA_UNIT
    emit("spawn_request", RUSA_UNIT, "availability_gate", "passed")

    if cmd and cmd.SpawnAt then
        local ok, result = safeCall("SpawnAt_error", function()
            return cmd:SpawnAt(RUSA_UNIT, MAX_SQUAD_SIZE, 0)
        end)
        emit("SpawnAt", "ok", tostring(ok), "result", tostring(result))
        if ok and result == true then return end
    end

    if cmd and cmd.Spawn then
        local ok, result = safeCall("Spawn_error", function()
            return cmd:Spawn(RUSA_UNIT, MAX_SQUAD_SIZE)
        end)
        emit("Spawn", "ok", tostring(ok), "result", tostring(result))
        if ok and result == true then return end
    end

    state.awaitingSpawn = false
    state.requestedUnit = nil
    emit("spawn_failed", RUSA_UNIT, "all_native_requests_rejected")
end

local function chooseFlag()
    local sc = scene()
    if not sc or type(sc.Flags) ~= "table" then return nil end
    local names = {}
    for _, flag in pairs(sc.Flags) do
        if flag and flag.name then names[#names + 1] = tostring(flag.name) end
    end
    table.sort(names)
    return names[1]
end

local function commandAfterSettle(generation, squad)
    if generation ~= state.generation or squad ~= state.spawnedSquad then return end
    local cmd = commands()
    local target = chooseFlag()
    if cmd and target and cmd.CaptureFlag then
        local ok, result = safeCall("order_error", function()
            return cmd:CaptureFlag(squad, target)
        end)
        if ok and result ~= false then
            emit("settled", "after_ms", SETTLE_MS, "order", "CaptureFlag", "target", target, "squadId", tostring(squad))
            return
        end
    end
    if cmd and cmd.SeekAndDestroy then
        local ok = safeCall("order_error", function()
            cmd:SeekAndDestroy(squad)
            return true
        end)
        emit("settled", "after_ms", SETTLE_MS, "order", "SeekAndDestroy", "squadId", tostring(squad), "ok", tostring(ok))
    else
        emit("settled", "after_ms", SETTLE_MS, "order", "none", "squadId", tostring(squad))
    end
end

local function attempt(generation)
    if generation ~= state.generation or not state.armed or state.attempted then return end
    state.attempted = true
    failClosedLegacySupport()
    trySpawn()
end

local function onGameSpawn(args)
    if not state.armed or not state.awaitingSpawn or state.spawnedSquad then return end
    if not args or not args.squadId then
        emit("GameSpawn", "missing_squadId")
        return
    end

    state.spawnedSquad = args.squadId
    state.awaitingSpawn = false
    emit(
        "GameSpawn",
        "requestedUnit", tostring(state.requestedUnit),
        "squadId", tostring(args.squadId),
        "native_birth_confirmed", true,
        "owner", tostring(instance().playerId)
    )

    local ev = events()
    local generation = state.generation
    if ev and ev.SetQuantTimer then
        ev:SetQuantTimer(function() commandAfterSettle(generation, args.squadId) end, SETTLE_MS)
    else
        commandAfterSettle(generation, args.squadId)
    end
end

local function onGameStart()
    state.generation = state.generation + 1
    state.armed = false
    state.attempted = false
    state.awaitingSpawn = false
    state.requestedUnit = nil
    state.spawnedSquad = nil

    failClosedLegacySupport()

    local i = instance()
    local c = conquest()
    emit(
        "GameStart",
        "playerId", tostring(i.playerId),
        "team", tostring(i.team),
        "army", tostring(i.army),
        "attacking", tostring(c.Attacking),
        "firstPlayerId", tostring(c.FirstPlayerId),
        "firstEnemyId", tostring(c.FirstEnemyId),
        "defenderBotId", tostring(c.DefenderBotId)
    )

    if c.Attacking ~= true or tostring(i.team or "") ~= "a" then
        emit("gate_skip", "not_team_a_human_attack")
        return
    end
    if tostring(i.army or "") ~= "rusa" then
        emit("gate_skip", "rusa_only", "army", tostring(i.army))
        return
    end

    state.armed = true
    emit(
        "armed",
        "native_conquest_birth", true,
        "candidate", RUSA_UNIT,
        "min_stage", 0,
        "parked_support", "disabled",
        "auto_transfer", "not_required_native_mate_owner",
        "tmai_settle_ms", SETTLE_MS
    )

    local ev = events()
    local generation = state.generation
    if ev and ev.SetQuantTimer then
        ev:SetQuantTimer(function() attempt(generation) end, START_DELAY_MS)
    else
        attempt(generation)
    end
end

local function onQuant()
    if state.armed then failClosedLegacySupport() end
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
    ev:Subscribe(ev.Quant, safeEvent("Quant", onQuant))
    ev:Subscribe(ev.GameEnd, safeEvent("GameEnd", onGameEnd))
    emit("subscribed")
else
    emit("not_armed", "BotApi.Events_missing")
end
