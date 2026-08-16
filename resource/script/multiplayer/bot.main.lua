-- Function to require and initialize the appropriate game mode .lua file.
-- Campaign CTF uses the normal engine-managed conquest bots only. Attack-support
-- ownership is published by attack_support_handoff.lua from the normal enemy
-- mission-authority bot; there is no extra Team-A AI player.

local ROUTER_PREFIX = "CODEX_ATTACK_SUPPORT_ROUTER"

local ROUTER_DEBUG = true
local function routerLog(...)
    if not ROUTER_DEBUG then return end
    local out = {ROUTER_PREFIX .. ":"}
    for n = 1, select("#", ...) do
        out[#out + 1] = tostring(select(n, ...))
    end
    print(table.concat(out, " "))
end

local function positiveId(primary, fallback)
    primary = tonumber(primary or 0) or 0
    fallback = tonumber(fallback or 0) or 0
    if primary > 0 then return primary end
    if fallback > 0 then return fallback end
    return 0
end

local function campaignIdentity()
    local instance = (BotApi and BotApi.Instance) or {}
    local conquest = (BotApi and BotApi.Conquest) or {}
    return {
        gameMode = tostring(instance.gameMode or ""),
        team = tostring(instance.team or ""),
        army = tostring(instance.army or ""),
        difficulty = tostring(instance.difficulty or ""),
        playerId = tonumber(instance.playerId or 0) or 0,
        firstPlayerId = positiveId(conquest.FirstPlayerId, instance.CampaignFirstPlayerId),
        firstEnemyId = positiveId(conquest.FirstEnemyId, instance.CampaignFirstEnemyId),
        defenderBotId = positiveId(conquest.DefenderBotId, instance.CampaignDefenderBotId),
        attacking = conquest.Attacking,
        isHuman = instance.isHuman == true or tostring(instance.isHuman or "") == "true",
    }
end

local function safeRequire(path)
    local ok, err = pcall(require, path)
    if not ok then
        routerLog("require_failed", path, tostring(err))
        return false
    end
    return true
end

local function initializeBotAI()
    local gameModeMap = {
        campaign_capture_the_flag = "conquest",
        battle_zones = "battlezones",
        ammunition = "battlezones", -- ammunition aka domination
        evacuation = "laststand",
        frontlines = "frontlines",
    }

    local identity = campaignIdentity()
    routerLog(
        "classify",
        "playerId", identity.playerId,
        "team", identity.team,
        "army", identity.army,
        "gameMode", identity.gameMode,
        "attacking", tostring(identity.attacking),
        "isHuman", tostring(identity.isHuman),
        "firstPlayerId", identity.firstPlayerId,
        "firstEnemyId", identity.firstEnemyId,
        "defenderBotId", identity.defenderBotId
    )

    -- Never run bot controllers on the human client slot. Doing so can null-deref
    -- engine BotApi fields (spawnPointName / Events) and hard-crash the process.
    if identity.isHuman then
        routerLog("route_skip", "human_player", "playerId", identity.playerId)
        return
    end

    local mode = gameModeMap[identity.gameMode]
    if not mode then
        routerLog("route_failed", "unknown_game_mode", identity.gameMode, "playerId", identity.playerId)
        return
    end

    local gameModeScriptPath = "resource/script/multiplayer/modes/" .. mode
    routerLog("route", mode, "playerId", identity.playerId)
    if not safeRequire(gameModeScriptPath) then
        return
    end

    -- On campaign CTF, the normal mission-authority conquest bot also publishes the
    -- human player's FirstPlayerId as the owner for MI-delivered attack support.
    -- The handoff module self-gates to human-attack missions and FirstEnemyId only.
    if identity.gameMode == "campaign_capture_the_flag" then
        safeRequire("resource/script/multiplayer/modes/attack_support_handoff")
    end

    -- Load Battle Zones-specific reliability and purchasing overrides only after
    -- the base mode has registered its shared functions and event handlers.
    if identity.gameMode == "battle_zones" or identity.gameMode == "ammunition" then
        safeRequire([[/script/multiplayer/modes/battlezones_overhaul]])
    end

    if initialize then
        local ok, err = pcall(initialize)
        if not ok then
            routerLog("initialize_failed", tostring(err))
        end
    end
end

initializeBotAI()
