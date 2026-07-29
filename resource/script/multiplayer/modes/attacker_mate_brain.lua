-- Attack-mate strategic brain (allied attack waves).
--
-- Decides which flag each friendly attack-mate squad should push, and issues the
-- order. It never purchases, spawns, transfers or teleports anything: unit supply
-- is the mission script's job (resource/map/multi/allied_attack_waves.inc).
--
-- NOT WIRED INTO THE ROUTER. bot.main.lua does not require this file, and even if
-- it did the module arms nothing unless the mission sets allied_attack_enabled.
-- Loadable-but-inert is the intended state until the wave engine is deployed.
--
-- Determinism: every player in a multiplayer session runs this logic, so it must
-- reach the same conclusion everywhere. The RNG is deliberately never touched in
-- this file: spread comes from a string hash of squad key + flag name, and from a
-- standing assignment table that persists across scan pulses.

local PREFIX = "CODEX_ATTACK_MATE_BRAIN"

-- How often the poll loop scans (in quants).
local SCAN_PERIOD = 2
-- Re-issue an unchanged order only after this many quants.
local ORDER_REFRESH_QUANTS = 70
-- Hard ceiling on orders emitted in a single pulse.
local MAX_ORDERS_PER_PULSE = 24
-- Squads already standing on a flag cost this much when scoring it, so the
-- brain spreads pressure instead of stacking every squad on one point.
local CROWDING_PENALTY = 0.35
-- Deterministic jitter band (fraction of score) to break exact ties.
local JITTER_BAND = 0.08

local function log(...)
    local out = {PREFIX .. ":"}
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

local function commands()
    return (BotApi and BotApi.Commands) or nil
end

local function events()
    return (BotApi and BotApi.Events) or nil
end

-- Small, stable, order-independent string hash. Same input -> same number on
-- every machine and every Lua build (integer arithmetic only, bounded range).
local function stringHash(text)
    text = tostring(text or "")
    local h = 5381
    for i = 1, #text do
        h = (h * 33 + text:byte(i)) % 1000003
    end
    return h
end

-- Deterministic per-(squad, flag) jitter in [0, JITTER_BAND].
local function jitterFor(squadKey, flagName)
    local h = stringHash(tostring(squadKey) .. "|" .. tostring(flagName))
    return (h % 1000) / 1000 * JITTER_BAND
end

local state = {
    quant = 0,
    armed = false,
    team = "",
    enemyTeam = "",
    -- squadKey -> {flag = name, quant = <quant of last order>, owner = <flag owner then>}
    assignments = {},
    -- flagName -> number of squads currently standing on that assignment.
    load = {},
    ordersSent = 0,
    ordersFailed = 0,
}

local function isEnabled()
    local sc = scene()
    if not sc or not sc.GetVar then
        return false, "Scene.GetVar_missing"
    end
    local ok, value = pcall(function() return sc:GetVar("allied_attack_enabled") end)
    if not ok then
        return false, "GetVar_error"
    end
    if value == nil then
        return false, "var_nil"
    end
    if tonumber(value or 0) ~= 1 then
        return false, "var_off"
    end
    return true, "enabled"
end

local function resolveTeams()
    local i = instance()
    local team = tostring(i.team or "")
    local enemy = ""
    if team == "a" then
        enemy = "b"
    elseif team == "b" then
        enemy = "a"
    elseif team ~= "" then
        enemy = tostring(conquest().EnemyTeam or "")
    end
    state.team = team
    state.enemyTeam = enemy
end

local function flagOwner(flag)
    return tostring(flag.occupant or flag.owner or "")
end

local function flagPriority(flag)
    return tonumber(flag.priority or flag.prio or 1) or 1
end

-- Deterministic snapshot of the flag list, sorted by name.
local function collectFlags()
    local sc = scene()
    local flags = {}
    if not sc or type(sc.Flags) ~= "table" then return flags end
    for key, flag in pairs(sc.Flags) do
        if flag then
            local name = tostring(flag.name or key)
            flags[#flags + 1] = {
                key = tostring(key),
                name = name,
                owner = flagOwner(flag),
                priority = flagPriority(flag),
            }
        end
    end
    table.sort(flags, function(a, b)
        if a.name == b.name then return a.key < b.key end
        return a.name < b.name
    end)
    return flags
end

-- Deterministic snapshot of the squad list, sorted by key.
local function collectSquads()
    local sc = scene()
    local squads = {}
    if not sc or type(sc.Squads) ~= "table" then return squads end
    for key, squad in pairs(sc.Squads) do
        if squad ~= nil then
            squads[#squads + 1] = {key = tostring(key), squad = squad}
        end
    end
    table.sort(squads, function(a, b) return a.key < b.key end)
    return squads
end

local function squadTagged(squad, tag)
    local sc = scene()
    if not sc or not sc.IsSquadTagged then return false end
    local ok, tagged = pcall(function() return sc:IsSquadTagged(squad, tag) end)
    return ok and tagged == true
end

local function squadExists(squad)
    local sc = scene()
    if not sc or not sc.IsSquadExists then return true end
    local ok, exists = pcall(function() return sc:IsSquadExists(squad) end)
    if not ok then return true end
    return exists == true
end

-- Standing-assignment bookkeeping. The load table is rebuilt from the surviving
-- assignments each pulse, so a flag's load decays only when a squad actually dies
-- or is reassigned — never merely because a scan pulse happened.
local function reconcileAssignments(liveKeys)
    local load = {}
    local kept = {}
    for key, record in pairs(state.assignments) do
        if liveKeys[key] then
            kept[key] = record
            load[record.flag] = (load[record.flag] or 0) + 1
        end
    end
    state.assignments = kept
    state.load = load
end

local function assign(squadKey, flagName, owner)
    local previous = state.assignments[squadKey]
    if previous and previous.flag then
        state.load[previous.flag] = math.max((state.load[previous.flag] or 1) - 1, 0)
    end
    state.assignments[squadKey] = {flag = flagName, quant = state.quant, owner = owner}
    state.load[flagName] = (state.load[flagName] or 0) + 1
end

-- Enemy-owned flags score highest, neutral second, ally-owned excluded outright.
local function scoreFlag(flag, squadKey)
    local owner = flag.owner
    if owner ~= "" and owner == state.team then
        return nil
    end

    local base
    if state.enemyTeam ~= "" and owner == state.enemyTeam then
        base = 2.0
    elseif owner == "" or owner == "neutral" or owner == "nil" then
        base = 1.0
    elseif owner ~= state.team then
        base = 2.0
    else
        return nil
    end

    local score = base * flagPriority(flag)
    local crowd = state.load[flag.name] or 0
    score = score - (crowd * CROWDING_PENALTY * base)
    score = score + jitterFor(squadKey, flag.name)
    return score
end

local function pickFlag(flags, squadKey)
    local best, bestScore = nil, nil
    for _, flag in ipairs(flags) do
        local score = scoreFlag(flag, squadKey)
        if score ~= nil then
            -- Ties resolve on flag name so the choice is stable everywhere.
            if bestScore == nil or score > bestScore
                or (score == bestScore and best ~= nil and flag.name < best.name) then
                best, bestScore = flag, score
            end
        end
    end
    return best, bestScore
end

local function issueCapture(squad, squadKey, flagName)
    local cmd = commands()
    if not cmd or not cmd.CaptureFlag then
        state.ordersFailed = state.ordersFailed + 1
        log("order_failed", "squad", squadKey, "flag", flagName, "reason", "Commands.CaptureFlag_missing")
        return false
    end
    local ok, err = pcall(function() return cmd:CaptureFlag(squad, flagName) end)
    if not ok then
        state.ordersFailed = state.ordersFailed + 1
        log("order_failed", "squad", squadKey, "flag", flagName, "reason", tostring(err))
        return false
    end
    state.ordersSent = state.ordersSent + 1
    log("order_sent", "squad", squadKey, "flag", flagName, "quant", state.quant)
    return true
end

local function issueSeekAndDestroy(squad, squadKey)
    local cmd = commands()
    if not cmd or not cmd.SeekAndDestroy then
        state.ordersFailed = state.ordersFailed + 1
        log("order_failed", "squad", squadKey, "order", "SeekAndDestroy", "reason", "Commands.SeekAndDestroy_missing")
        return false
    end
    local ok, err = pcall(function() return cmd:SeekAndDestroy(squad) end)
    if not ok then
        state.ordersFailed = state.ordersFailed + 1
        log("order_failed", "squad", squadKey, "order", "SeekAndDestroy", "reason", tostring(err))
        return false
    end
    state.ordersSent = state.ordersSent + 1
    log("order_sent", "squad", squadKey, "order", "SeekAndDestroy", "quant", state.quant)
    return true
end

-- True when the squad's standing order is still good enough to leave alone.
local function orderIsStale(record, flags)
    if not record then return true end
    if (state.quant - (record.quant or 0)) >= ORDER_REFRESH_QUANTS then return true end
    for _, flag in ipairs(flags) do
        if flag.name == record.flag then
            -- Target went ours: the squad has nothing left to do there.
            if flag.owner ~= "" and flag.owner == state.team then return true end
            return false
        end
    end
    -- Target vanished from the flag list.
    return true
end

local function pulse()
    local squads = collectSquads()
    local flags = collectFlags()

    local liveKeys = {}
    for _, entry in ipairs(squads) do
        if squadExists(entry.squad) then
            liveKeys[entry.key] = true
        end
    end
    reconcileAssignments(liveKeys)

    if #flags == 0 then
        return
    end

    local orders = 0
    for _, entry in ipairs(squads) do
        if orders >= MAX_ORDERS_PER_PULSE then break end
        local squad, key = entry.squad, entry.key

        if not liveKeys[key] then
            -- Squad gone; assignment already decayed in reconcileAssignments.
        elseif squadTagged(squad, "_lua_mi")
            or squadTagged(squad, "_lua_ignore")
            or squadTagged(squad, "dead")
            or squadTagged(squad, "repairing") then
            -- Scripted, ignored, dead or repairing squads are never ours to command.
        elseif squadTagged(squad, "_lua_alert") then
            -- Contact: let the squad fight instead of walking it into fire.
            if issueSeekAndDestroy(squad, key) then
                orders = orders + 1
            end
        else
            local record = state.assignments[key]
            if orderIsStale(record, flags) then
                local flag = pickFlag(flags, key)
                if flag then
                    if issueCapture(squad, key, flag.name) then
                        assign(key, flag.name, flag.owner)
                        orders = orders + 1
                    end
                end
            end
        end
    end

    if orders > 0 then
        log("pulse", "quant", state.quant, "squads", #squads, "flags", #flags, "orders", orders)
    end
end

local function onGameStart()
    resolveTeams()
    log("game_start", "team", state.team, "enemyTeam", state.enemyTeam, "attacking", tostring(conquest().Attacking))
end

local function onQuant()
    state.quant = state.quant + 1
    if state.quant % SCAN_PERIOD ~= 0 then return end
    if state.team == "" then resolveTeams() end
    pulse()
end

local function onGameEnd()
    log(
        "game_end",
        "quant", state.quant,
        "orders_sent", state.ordersSent,
        "orders_failed", state.ordersFailed
    )
end

local function safeEvent(name, fn)
    return function(...)
        local ok, err = pcall(fn, ...)
        if not ok then
            log("event_error_suppressed", "event", name, "reason", tostring(err))
        end
    end
end

-- Master gate. Nothing subscribes unless the mission has explicitly enabled the
-- allied attack waves, so the file stays loadable and completely inert otherwise.
local enabled, reason = isEnabled()
if not enabled then
    log("brain_disarmed", "reason", reason)
else
    local ev = events()
    if ev and ev.Subscribe then
        ev:Subscribe(ev.GameStart, safeEvent("GameStart", onGameStart))
        ev:Subscribe(ev.Quant, safeEvent("Quant", onQuant))
        ev:Subscribe(ev.GameEnd, safeEvent("GameEnd", onGameEnd))
        state.armed = true
        log(
            "brain_armed",
            "scan_period", SCAN_PERIOD,
            "refresh_quants", ORDER_REFRESH_QUANTS,
            "max_orders_per_pulse", MAX_ORDERS_PER_PULSE
        )
    else
        log("brain_disarmed", "reason", "BotApi.Events_missing")
    end
end
