-- Attack-mate slot proof and diagnostics.
--
-- This controller deliberately does not purchase, spawn, transfer, or command
-- units. Its only job is to prove that the extra Team A bot exists on human
-- attack missions and report what the engine exposes through BotApi.

local PREFIX = "CODEX_ATTACK_MATE_PROBE"

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

local function events()
    return (BotApi and BotApi.Events) or nil
end

-- GetVar is not proven on this BotApi surface: guard every read and degrade to
-- a printable placeholder instead of letting the report path die.
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

-- NEVER read Instance.spawnPointName or Conquest.PlayerSpawnPoint here: on the
-- extra Team A slot (no spawn point assigned) the native getter null-derefs and
-- kills the whole process with an access violation pcall cannot catch.
-- (Proven live 2026-07-29, game v1.065: crash in lua.start immediately after
-- "route attacker_mate playerId 1", before module_loaded could print.)
local function identity()
    local i = instance()
    local c = conquest()
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

local function collectSquads(trace)
    if trace then log("trace_sq", "pre_scene") end
    local sc = scene()
    local squads = {}
    if not sc then return squads end
    if trace then log("trace_sq", "pre_squads_read") end
    if type(sc.Squads) ~= "table" then return squads end
    if trace then log("trace_sq", "iterating") end
    local seen = 0
    for key, squad in pairs(sc.Squads) do
        seen = seen + 1
        if trace and seen <= 3 then log("trace_sq", "entry", tostring(key)) end
        squads[#squads + 1] = {
            key = tostring(key),
            value = tostring(squad),
        }
    end
    table.sort(squads, function(a, b)
        if a.key == b.key then return a.value < b.value end
        return a.key < b.key
    end)
    return squads
end

local function collectFlags(trace)
    if trace then log("trace_fl", "pre_scene") end
    local sc = scene()
    local flags = {}
    if not sc then return flags end
    if trace then log("trace_fl", "pre_flags_read") end
    if type(sc.Flags) ~= "table" then return flags end
    if trace then log("trace_fl", "iterating") end
    local seen = 0
    for key, flag in pairs(sc.Flags) do
        seen = seen + 1
        local traceEntry = trace and seen <= 3
        if flag then
            if traceEntry then log("trace_fl", "entry_key", tostring(key)) end
            local name = tostring(flag.name or "")
            if traceEntry then log("trace_fl", "got_name") end
            local owner = tostring(flag.occupant or flag.owner or "unknown")
            if traceEntry then log("trace_fl", "got_owner") end
            local priority = tostring(flag.priority or flag.prio or "unknown")
            if traceEntry then log("trace_fl", "got_prio") end
            flags[#flags + 1] = {
                key = tostring(key),
                name = name,
                owner = owner,
                priority = priority,
            }
            if traceEntry then log("trace_fl", "entry_ok", tostring(key)) end
        end
    end
    table.sort(flags, function(a, b)
        if a.name == b.name then return a.key < b.key end
        return a.name < b.name
    end)
    return flags
end

local function squadSummary(squads)
    if #squads == 0 then return "none" end
    local out = {}
    for _, squad in ipairs(squads) do
        out[#out + 1] = squad.key .. "=" .. squad.value
    end
    return table.concat(out, ",")
end

local function flagSummary(flags)
    if #flags == 0 then return "none" end
    local out = {}
    for _, flag in ipairs(flags) do
        out[#out + 1] = flag.name .. ":" .. flag.owner .. ":p" .. flag.priority
    end
    return table.concat(out, ",")
end

local state = {
    quant = 0,
    lastSquadCount = -1,
    lastFlagSummary = "",
    reportRuns = 0,
    heartbeatEvery = 100,
}

local function publishAttackMateIdentity(id)
    if id.attacking ~= true then return end
    local sc = scene()
    if not sc or not sc.SetVar then
        log("identity_publish_skipped", "reason", "Scene.SetVar_missing")
        return
    end
    sc:SetVar("id_attacker_mate", id.playerId)
    sc:SetVar("attacker_mate_ready", 1)
    log("identity_published", "id_attacker_mate", id.playerId, "attacker_mate_ready", 1)
end

local function report(source, force)
    state.reportRuns = state.reportRuns + 1
    local trace = state.reportRuns <= 5
    local id = identity()
    if trace then log("trace", state.reportRuns, source, "pre_collect_squads") end
    local squads = collectSquads(trace)
    if trace then log("trace", state.reportRuns, source, "post_collect_squads") end
    if trace then log("trace", state.reportRuns, source, "pre_collect_flags") end
    local flags = collectFlags(trace)
    if trace then log("trace", state.reportRuns, source, "post_collect_flags") end
    local flagsText = flagSummary(flags)

    if force or #squads ~= state.lastSquadCount then
        log(
            "scene_squads",
            "source", source,
            "count", #squads,
            "entries", squadSummary(squads)
        )
        state.lastSquadCount = #squads
    end

    if force or flagsText ~= state.lastFlagSummary then
        log(
            "scene_flags",
            "source", source,
            "count", #flags,
            "entries", flagsText
        )
        state.lastFlagSummary = flagsText
    end

    if force then
        log(
            "identity",
            "playerId", id.playerId,
            "team", id.team,
            "army", id.army,
            "difficulty", id.difficulty,
            "gameMode", id.gameMode,
            "attacking", tostring(id.attacking),
            "firstPlayerId", id.firstPlayerId,
            "firstEnemyId", id.firstEnemyId,
            "defenderBotId", id.defenderBotId
        )
        log(
            "policy",
            "diagnostics_only",
            "purchase", "disabled",
            "spawn", "disabled",
            "transfer", "disabled",
            "orders", "disabled"
        )
        log(
            "mi_probe_state",
            "started", readVar("attack_mate_probe_started"),
            "transferred", readVar("attack_mate_probe_transferred"),
            "retasked", readVar("attack_mate_probe_retasked"),
            "stage", readVar("attack_mate_probe_stage"),
            "owner_fail", readVar("allied_attack_owner_fail")
        )
    end

    if trace then log("trace", state.reportRuns, source, "report_done") end
end

local function onGameStart()
    local id = identity()
    log("game_start", "playerId", id.playerId, "attacking", tostring(id.attacking))
    publishAttackMateIdentity(id)
    report("GameStart", true)
end

local function onQuant()
    if state.quant % state.heartbeatEvery == 0 then log("quant_alive", state.quant) end
    state.quant = state.quant + 1
    if state.quant % 20 == 0 then
        report("Quant", false)
    end
    if state.quant % 200 == 0 then
        report("QuantSummary", true)
    end
end

local function onGameEnd()
    report("GameEnd", true)
    log("game_end", "quant", state.quant)
end

local function safeEvent(name, fn)
    return function(...)
        local ok, err = pcall(fn, ...)
        if not ok then
            log("event_error_suppressed", "event", name, "reason", tostring(err))
        end
    end
end

log("boot", "stage", 1, "pre_identity")
local id = identity()
log("boot", "stage", 2, "identity_ok")
log(
    "module_loaded",
    "playerId", id.playerId,
    "team", id.team,
    "attacking", tostring(id.attacking),
    "defenderBotId", id.defenderBotId
)

log("boot", "stage", 3, "pre_events")
local ev = events()
log("boot", "stage", 4, "events_fetched", "has_subscribe", tostring(ev and ev.Subscribe ~= nil))
if ev and ev.Subscribe then
    log("boot", "stage", 5, "pre_subscribe_gamestart")
    ev:Subscribe(ev.GameStart, safeEvent("GameStart", onGameStart))
    log("boot", "stage", 6, "pre_subscribe_quant")
    ev:Subscribe(ev.Quant, safeEvent("Quant", onQuant))
    log("boot", "stage", 7, "pre_subscribe_gameend")
    ev:Subscribe(ev.GameEnd, safeEvent("GameEnd", onGameEnd))
    log("probe_armed", "diagnostics_only", true)
else
    log("probe_not_armed", "reason", "BotApi.Events_missing")
end
