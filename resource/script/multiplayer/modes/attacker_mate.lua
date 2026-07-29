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

local function positiveId(primary, fallback)
    primary = tonumber(primary or 0) or 0
    fallback = tonumber(fallback or 0) or 0
    if primary > 0 then return primary end
    if fallback > 0 then return fallback end
    return 0
end

local function identity()
    local i = instance()
    local c = conquest()
    return {
        playerId = tonumber(i.playerId or 0) or 0,
        team = tostring(i.team or ""),
        army = tostring(i.army or ""),
        difficulty = tostring(i.difficulty or ""),
        gameMode = tostring(i.gameMode or ""),
        spawnPointName = tostring(i.spawnPointName or ""),
        attacking = c.Attacking,
        firstPlayerId = positiveId(c.FirstPlayerId, i.CampaignFirstPlayerId),
        firstEnemyId = positiveId(c.FirstEnemyId, i.CampaignFirstEnemyId),
        defenderBotId = positiveId(c.DefenderBotId, i.CampaignDefenderBotId),
        playerSpawnPoint = tostring(c.PlayerSpawnPoint or ""),
    }
end

local function collectSquads()
    local sc = scene()
    local squads = {}
    if not sc or type(sc.Squads) ~= "table" then return squads end
    for key, squad in pairs(sc.Squads) do
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

local function collectFlags()
    local sc = scene()
    local flags = {}
    if not sc or type(sc.Flags) ~= "table" then return flags end
    for key, flag in pairs(sc.Flags) do
        if flag then
            flags[#flags + 1] = {
                key = tostring(key),
                name = tostring(flag.name or ""),
                owner = tostring(flag.occupant or flag.owner or "unknown"),
                priority = tostring(flag.priority or flag.prio or "unknown"),
            }
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
    local id = identity()
    local squads = collectSquads()
    local flags = collectFlags()
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
            "defenderBotId", id.defenderBotId,
            "spawnPointName", id.spawnPointName,
            "playerSpawnPoint", id.playerSpawnPoint
        )
        log(
            "policy",
            "diagnostics_only",
            "purchase", "disabled",
            "spawn", "disabled",
            "transfer", "disabled",
            "orders", "disabled"
        )
    end
end

local function onGameStart()
    local id = identity()
    log("game_start", "playerId", id.playerId, "attacking", tostring(id.attacking))
    publishAttackMateIdentity(id)
    report("GameStart", true)
end

local function onQuant()
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

local id = identity()
log(
    "module_loaded",
    "playerId", id.playerId,
    "team", id.team,
    "attacking", tostring(id.attacking),
    "defenderBotId", id.defenderBotId
)

local ev = events()
if ev and ev.Subscribe then
    ev:Subscribe(ev.GameStart, safeEvent("GameStart", onGameStart))
    ev:Subscribe(ev.Quant, safeEvent("Quant", onQuant))
    ev:Subscribe(ev.GameEnd, safeEvent("GameEnd", onGameEnd))
    log("probe_armed", "diagnostics_only", true)
else
    log("probe_not_armed", "reason", "BotApi.Events_missing")
end
