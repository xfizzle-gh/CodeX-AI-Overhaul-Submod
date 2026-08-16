-- Mate unitset provisioning probe (#104).
--
-- Research-only branch from main. The campaign preset temporarily provisions BOT
-- slots from the normal 2022s skirmish unitset while the human remains on the
-- conquest unitset. This probe asks one question only: does that give the extra
-- Team-A Mate a real engine unit catalog?
--
-- ZERO NATIVE SPAWN CALLS BY DESIGN. PR #103 proved that calling SpawnAt/Spawn on
-- this special Mate with an invalid context can hard-crash the engine outside Lua
-- error handling. #104 therefore records availability and raw spawn-point fields
-- only. A later experiment may spawn only after both catalog and spawn context are
-- understood.

local PREFIX = "CODEX_MATE_UNITSET_PROBE"
local START_DELAY_MS = 3000

local RUSA_CANDIDATES = {
    "rus90_inf_rifle(rusa)",
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
    checked = false,
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

local function inspectProvisioning(generation)
    if generation ~= state.generation or not state.armed or state.checked then return end
    state.checked = true

    local i = instance()
    local c = conquest()
    local cmd = commands()

    if tostring(i.army or "") ~= "rusa" then
        emit("gate_skip", "rusa_only", "army", tostring(i.army))
        return
    end

    emit(
        "context",
        "playerId", tostring(i.playerId),
        "team", tostring(i.team),
        "army", tostring(i.army),
        "expected_bot_unitset", "2022s",
        "spawnPointName", tostring(i.spawnPointName),
        "PlayerSpawnPoint", tostring(c.PlayerSpawnPoint),
        "SpawnAt_api", tostring(cmd and cmd.SpawnAt ~= nil),
        "Spawn_api", tostring(cmd and cmd.Spawn ~= nil),
        "native_spawn_calls", "disabled"
    )

    local availableCount = 0
    local unknownCount = 0
    for _, unit in ipairs(RUSA_CANDIDATES) do
        local available = unitAvailability(cmd, unit)
        if available == true then
            availableCount = availableCount + 1
        elseif available == nil then
            unknownCount = unknownCount + 1
        end
    end

    if availableCount > 0 then
        emit(
            "provision_result", "PASSED",
            "available_count", availableCount,
            "unknown_count", unknownCount,
            "next_step", "validate_spawn_context_before_native_call"
        )
    else
        emit(
            "provision_result", "FAILED",
            "available_count", 0,
            "unknown_count", unknownCount,
            "reason", "mate_still_has_no_available_units",
            "native_spawn_calls", "suppressed"
        )
    end
end

local function onGameStart()
    state.generation = state.generation + 1
    state.armed = false
    state.checked = false

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
        "mode", "mate_unitset_provision_2022s",
        "research_only", true,
        "human_unitset", "conquest",
        "bot_unitset", "2022s",
        "parked_templates", "disabled",
        "mi_support_waves", "disabled",
        "ownership_transfer", "disabled",
        "native_spawn_calls", "disabled"
    )

    local ev = events()
    local generation = state.generation
    if ev and ev.SetQuantTimer then
        ev:SetQuantTimer(function() inspectProvisioning(generation) end, START_DELAY_MS)
    else
        inspectProvisioning(generation)
    end
end

local function onGameEnd()
    emit("GameEnd", "checked", tostring(state.checked))
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
    ev:Subscribe(ev.GameEnd, safeEvent("GameEnd", onGameEnd))
    emit("subscribed")
else
    emit("not_armed", "BotApi.Events_missing")
end
