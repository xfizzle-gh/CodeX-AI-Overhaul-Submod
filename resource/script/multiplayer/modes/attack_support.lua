-- Mate transfer-seed probe (#105 research).
--
-- PURPOSE
--   Test whether receiving one genuinely native human Conquest squad through the
--   normal transfer UI gives the extra Team-A Mate any usable native unit catalog.
--
-- SAFETY
--   * no Spawn / SpawnAt calls
--   * no unitset / preset changes
--   * no utility.lua / logic stack
--   * no QueryScene
--   * no automatic support ownership or wave arming
--   * enemy Dynamic Conquest bot remains untouched
--
-- Native run #1 proved the engine performs the manual transfer actor-by-actor,
-- but that transfer does not create a new BotApi.Scene.Squads entry on this Mate.
-- Therefore this probe does not use Scene.Squads as a transfer detector. Instead
-- it samples IsUnitAvailable periodically for a short post-GameStart window. The
-- engine log remains the authority proving that the manual player->Mate transfer
-- occurred during that window.

local PREFIX = "CODEX_MATE_SEED_PROBE"
local CHECK_MS = 3000
local MAX_CHECKS = 10

local CANDIDATES = {
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
    finished = false,
    checkIndex = 0,
    before = {},
}

local function safeAvailability(label)
    local cmd = commands()
    local result = {}
    if not cmd or not cmd.IsUnitAvailable then
        emit(label, "IsUnitAvailable_api_missing")
        return result
    end

    for _, unit in ipairs(CANDIDATES) do
        local ok, available = pcall(function()
            return cmd:IsUnitAvailable(unit)
        end)
        if not ok then
            emit(label, "unit", unit, "lua_error", tostring(available))
            result[unit] = nil
        else
            result[unit] = available == true
            emit(label, "unit", unit, "IsUnitAvailable", tostring(available))
        end
    end
    return result
end

local function countAvailability(before, after)
    local flipped = 0
    local availableAfter = 0
    for _, unit in ipairs(CANDIDATES) do
        if after[unit] == true then
            availableAfter = availableAfter + 1
            if before[unit] ~= true then
                flipped = flipped + 1
            end
        end
    end
    return availableAfter, flipped
end

local function finish(after, reason)
    if state.finished then return end
    state.finished = true

    local availableAfter, flipped = countAvailability(state.before, after)
    emit(
        "result",
        "available_after", availableAfter,
        "false_to_true", flipped,
        "checks", state.checkIndex,
        "reason", reason,
        "native_spawn_calls", "disabled"
    )

    if flipped > 0 then
        emit("conclusion", "TRANSFER_SEEDED_NATIVE_CATALOG", "next", "guarded_spawn_context_probe")
    elseif availableAfter > 0 then
        emit("conclusion", "CATALOG_ALREADY_AVAILABLE_OR_UNCHANGED", "next", "inspect_baseline_evidence")
    else
        -- Do not claim the transfer failed to seed unless game.log separately proves
        -- that a player->Mate transfer actually happened during this polling window.
        emit(
            "conclusion",
            "NO_CATALOG_CHANGE_OBSERVED",
            "transfer_must_be_verified_in_engine_log",
            "next", "mate_only_context_required_if_transfer_confirmed"
        )
    end
end

local function scheduleNext(generation)
    local ev = events()
    if not ev or not ev.SetQuantTimer then
        emit("not_armed", "SetQuantTimer_missing")
        state.armed = false
        return
    end

    ev:SetQuantTimer(function()
        if generation ~= state.generation or not state.armed or state.finished then return end

        state.checkIndex = state.checkIndex + 1
        local label = "poll_" .. tostring(state.checkIndex)
        local after = safeAvailability(label)
        local _, flipped = countAvailability(state.before, after)

        if flipped > 0 then
            finish(after, "availability_flip")
            return
        end

        if state.checkIndex >= MAX_CHECKS then
            finish(after, "poll_window_complete")
            return
        end

        scheduleNext(generation)
    end, CHECK_MS)
end

local function onGameStart()
    state.generation = state.generation + 1
    state.armed = false
    state.finished = false
    state.checkIndex = 0
    state.before = {}

    local i = instance()
    local c = conquest()
    emit(
        "GameStart",
        "playerId", tostring(i.playerId),
        "team", tostring(i.team),
        "army", tostring(i.army),
        "attacking", tostring(c.Attacking),
        "auto_spawn", "disabled",
        "auto_transfer", "disabled",
        "support_waves", "disabled"
    )

    if c.Attacking ~= true then
        emit("gate_skip", "human_attack_only")
        return
    end
    if tostring(i.team or "") ~= "a" or tostring(i.army or "") ~= "rusa" then
        emit("gate_skip", "team_a_rusa_only")
        return
    end

    state.armed = true
    state.before = safeAvailability("before_transfer")
    emit(
        "armed",
        "manual_transfer_seed_probe",
        "check_ms", CHECK_MS,
        "max_checks", MAX_CHECKS,
        "native_spawn_calls", "disabled"
    )
    scheduleNext(state.generation)
end

local function onGameEnd()
    emit(
        "GameEnd",
        "finished", tostring(state.finished),
        "checks", tostring(state.checkIndex),
        "native_spawn_calls", "disabled"
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
    ev:Subscribe(ev.GameEnd, safeEvent("GameEnd", onGameEnd))
    emit("subscribed")
else
    emit("not_armed", "BotApi.Events_missing")
end
