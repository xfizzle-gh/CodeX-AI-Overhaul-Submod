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
-- The user manually transfers one normal RUSA squad to the Mate. We measure
-- IsUnitAvailable before transfer and again after the first Mate-owned squad has
-- settled for ~3 seconds. If availability flips false -> true, a native transfer
-- may seed enough context for a later guarded spawn experiment. If it stays false,
-- manual transfer is not a deck/bootstrap mechanism.

local PREFIX = "CODEX_MATE_SEED_PROBE"
local SETTLE_MS = 3000
local SCAN_QUANTS = 20

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
    quant = 0,
    armed = false,
    firstSquadKey = nil,
    postTransferChecked = false,
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
            local yes = available == true
            result[unit] = yes
            emit(label, "unit", unit, "IsUnitAvailable", tostring(available))
        end
    end
    return result
end

local function summarize(before, after)
    local flipped = 0
    local availableAfter = 0
    for _, unit in ipairs(CANDIDATES) do
        if after[unit] == true then
            availableAfter = availableAfter + 1
            if before[unit] ~= true then flipped = flipped + 1 end
        end
    end

    emit(
        "result",
        "available_after", availableAfter,
        "false_to_true", flipped,
        "native_spawn_calls", "disabled"
    )

    if flipped > 0 then
        emit("conclusion", "TRANSFER_SEEDED_NATIVE_CATALOG", "next", "guarded_spawn_context_probe")
    elseif availableAfter > 0 then
        emit("conclusion", "CATALOG_ALREADY_AVAILABLE_OR_UNCHANGED", "next", "inspect_baseline_evidence")
    else
        emit("conclusion", "TRANSFER_DID_NOT_SEED_NATIVE_CATALOG", "next", "mate_only_context_required")
    end
end

local function postTransferCheck(key, generation)
    if generation ~= state.generation or not state.armed or state.postTransferChecked then return end
    if state.firstSquadKey ~= key then return end

    local sc = scene()
    local stillPresent = false
    if sc and type(sc.Squads) == "table" then
        for _, squad in pairs(sc.Squads) do
            if tostring(squad) == key then
                stillPresent = true
                break
            end
        end
    end

    if not stillPresent then
        emit("post_transfer_skip", "first_squad_missing", key)
        state.firstSquadKey = nil
        return
    end

    state.postTransferChecked = true
    emit("settled", key, "after_ms", SETTLE_MS)
    local after = safeAvailability("after_transfer")
    summarize(state.before, after)
end

local function discoverFirstTransferredSquad()
    if state.firstSquadKey or state.postTransferChecked then return end
    local sc = scene()
    if not sc or type(sc.Squads) ~= "table" then return end

    for _, squad in pairs(sc.Squads) do
        local key = tostring(squad)
        state.firstSquadKey = key
        emit("discovered", key, "source", "manual_native_transfer")

        local ev = events()
        local generation = state.generation
        if ev and ev.SetQuantTimer then
            ev:SetQuantTimer(function() postTransferCheck(key, generation) end, SETTLE_MS)
        else
            postTransferCheck(key, generation)
        end
        return
    end
end

local function onGameStart()
    state.generation = state.generation + 1
    state.quant = 0
    state.armed = false
    state.firstSquadKey = nil
    state.postTransferChecked = false
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
    emit("armed", "manual_transfer_seed_probe", "settle_ms", SETTLE_MS, "native_spawn_calls", "disabled")
end

local function onQuant()
    if not state.armed or state.postTransferChecked then return end
    state.quant = state.quant + 1
    if state.quant % SCAN_QUANTS ~= 0 then return end
    discoverFirstTransferredSquad()
end

local function onGameEnd()
    emit("GameEnd", "post_transfer_checked", tostring(state.postTransferChecked))
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
    ev:Subscribe(ev.Quant, safeEvent("Quant", onQuant))
    ev:Subscribe(ev.GameEnd, safeEvent("GameEnd", onGameEnd))
    emit("subscribed")
else
    emit("not_armed", "BotApi.Events_missing")
end
