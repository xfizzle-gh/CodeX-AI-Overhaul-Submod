-- TMAI manual-transfer parity experiment.
--
-- PURPOSE
--   Reproduce the important TMAI v0.17 lifecycle for a clean native test:
--   the player deploys normal Dynamic Conquest troops, manually transfers a
--   squad to the extra Team-A Mate with the normal transfer UI, then this Mate
--   waits for the transferred squad and commands it.
--
-- This branch intentionally DOES NOT arm CodeX automatic allied-support waves,
-- spawn units, change ownership, or touch the player's campaign roster. If a
-- squad appears in BotApi.Scene.Squads here, it arrived through the normal game
-- lifecycle and was manually handed to this Mate.
--
-- TMAI behaviors mirrored from the v0.17 source audit recorded on issue #97:
--   * ~3 second settle after transfer
--   * managed squad registry + pruning
--   * independent squad/vehicle tasking
--   * spread groups across distinct objectives before stacking reinforcements
--   * recently lost friendly flags receive counterattack priority
--   * suppress identical order spam
--
-- Command transport stays on the already-proven BotApi CaptureFlag primitive
-- with SeekAndDestroy fallback. The FoW test is about native player creation +
-- manual transfer; command transport does not alter actor provenance.

local PREFIX = "CODEX_TMAI_MANUAL"
local SETTLE_MS = 3000
local SCAN_QUANTS = 20

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
    attackMission = false,
    managed = {},
    flagHistory = {},
    recentlyLost = {},
    planDirty = true,
}

local function squadKey(squad)
    return tostring(squad)
end

local function flagRelation(flag)
    local i = instance()
    local owner = tostring(flag and flag.occupant or "")
    local team = tostring(i.team or "")
    local enemy = tostring(i.enemyTeam or "")
    if owner ~= "" and owner == team then return "friendly" end
    if owner ~= "" and owner == enemy then return "enemy" end
    return "neutral"
end

local function updateFlags()
    local sc = scene()
    if not sc or type(sc.Flags) ~= "table" then return end
    for _, flag in pairs(sc.Flags) do
        if flag and flag.name then
            local name = tostring(flag.name)
            local now = flagRelation(flag)
            local before = state.flagHistory[name]
            if before == "friendly" and now ~= "friendly" then
                state.recentlyLost[name] = true
                state.planDirty = true
                emit("flag_lost", name, "now", now)
            elseif before and before ~= "friendly" and now == "friendly" then
                state.recentlyLost[name] = nil
                state.planDirty = true
                emit("flag_captured", name)
            end
            state.flagHistory[name] = now
        end
    end
end

local function settleEntry(key, generation)
    local e = state.managed[key]
    if not e or generation ~= state.generation then return end
    e.settled = true
    state.planDirty = true
    emit("settled", key, "after_ms", SETTLE_MS)
end

local function discoverSquads()
    local sc = scene()
    local ev = events()
    if not sc or type(sc.Squads) ~= "table" then return end

    local seen = {}
    for _, squad in pairs(sc.Squads) do
        local key = squadKey(squad)
        seen[key] = true
        if not state.managed[key] then
            state.managed[key] = {
                squad = squad,
                settled = false,
                lastRole = nil,
                lastTarget = nil,
            }
            state.planDirty = true
            emit("discovered", key, "source", "manual_transfer_or_native_team_unit")
            if ev and ev.SetQuantTimer then
                local generation = state.generation
                ev:SetQuantTimer(function() settleEntry(key, generation) end, SETTLE_MS)
            else
                state.managed[key].settled = true
            end
        end
    end

    for key, _ in pairs(state.managed) do
        if not seen[key] then
            state.managed[key] = nil
            state.planDirty = true
            emit("pruned", key)
        end
    end
end

local function sortedGroups()
    local groups = {}
    for key, e in pairs(state.managed) do
        if e.settled then
            groups[#groups + 1] = { key = key, entry = e }
        end
    end
    table.sort(groups, function(a, b) return a.key < b.key end)
    return groups
end

local function sortedObjectives()
    local sc = scene()
    local objectives = {}
    if not sc or type(sc.Flags) ~= "table" then return objectives end
    for _, flag in pairs(sc.Flags) do
        if flag and flag.name then
            local relation = flagRelation(flag)
            if relation ~= "friendly" then
                objectives[#objectives + 1] = {
                    name = tostring(flag.name),
                    relation = relation,
                    lost = state.recentlyLost[tostring(flag.name)] == true,
                }
            end
        end
    end
    table.sort(objectives, function(a, b)
        if a.lost ~= b.lost then return a.lost end
        if a.relation ~= b.relation then
            if a.relation == "enemy" then return true end
            if b.relation == "enemy" then return false end
        end
        return a.name < b.name
    end)
    return objectives
end

local function issue(entry, role, target)
    if entry.lastRole == role and entry.lastTarget == target then return end
    local cmd = commands()
    if not cmd then return end

    local ordered = false
    if target and cmd.CaptureFlag then
        local ok, result = pcall(function()
            return cmd:CaptureFlag(entry.squad, target)
        end)
        ordered = ok and result ~= false
    end
    if not ordered and cmd.SeekAndDestroy then
        local ok = pcall(function() cmd:SeekAndDestroy(entry.squad) end)
        ordered = ok
    end
    if ordered then
        entry.lastRole = role
        entry.lastTarget = target
        emit("order", role, "target", tostring(target), "squad", tostring(entry.squad))
    else
        emit("order_failed", role, "target", tostring(target), "squad", tostring(entry.squad))
    end
end

local function plan()
    if not state.planDirty then return end
    state.planDirty = false

    local groups = sortedGroups()
    local objectives = sortedObjectives()
    emit("plan", "groups", #groups, "objectives", #objectives)
    if #groups == 0 then return end

    if #objectives == 0 then
        -- All objectives are friendly. Do not spam movement; let local AI fight/hold.
        emit("hold_all_friendly", "groups", #groups)
        return
    end

    -- First pass: spread distinct groups across distinct objectives.
    local distinct = math.min(#groups, #objectives)
    for i = 1, distinct do
        local obj = objectives[i]
        issue(groups[i].entry, obj.lost and "counterattack" or "attack", obj.name)
    end

    -- Excess groups reinforce objectives round-robin instead of dogpiling one flag.
    for i = distinct + 1, #groups do
        local obj = objectives[((i - distinct - 1) % #objectives) + 1]
        issue(groups[i].entry, "reinforce", obj.name)
    end
end

local function onGameStart()
    state.generation = state.generation + 1
    state.quant = 0
    state.managed = {}
    state.flagHistory = {}
    state.recentlyLost = {}
    state.planDirty = true

    local i = instance()
    local c = conquest()
    state.attackMission = c.Attacking == true
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
    if state.attackMission then
        emit("armed", "manual_transfer_only", "settle_ms", SETTLE_MS)
    else
        emit("idle", "not_human_attack")
    end
end

local function onQuant()
    if not state.attackMission then return end
    state.quant = state.quant + 1
    if state.quant % SCAN_QUANTS ~= 0 then return end
    discoverSquads()
    updateFlags()
    plan()
end

local function onGameEnd()
    emit("GameEnd", "managed", tostring(#sortedGroups()))
end

local function safeEvent(name, fn)
    return function(...)
        local ok, err = pcall(fn, ...)
        if not ok then emit("event_error", name, tostring(err)) end
    end
end

local ev = events()
local i = instance()
emit("module_loaded", "playerId", tostring(i.playerId), "team", tostring(i.team))
if ev and ev.Subscribe then
    ev:Subscribe(ev.GameStart, safeEvent("GameStart", onGameStart))
    ev:Subscribe(ev.Quant, safeEvent("Quant", onQuant))
    ev:Subscribe(ev.GameEnd, safeEvent("GameEnd", onGameEnd))
    emit("subscribed")
else
    emit("not_armed", "BotApi.Events_missing")
end
