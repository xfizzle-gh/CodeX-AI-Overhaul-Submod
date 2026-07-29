-- Function to require and initialize the appropriate game mode .lua file.
-- Campaign CTF routes Team A support bots into the attack-mate diagnostic
-- controller without ever loading the unsafe conquest/utility stack for them.

local ROUTER_PREFIX = "CODEX_ATTACK_MATE_ROUTER"

local function routerLog(...)
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

local function isCampaignTeamABot(identity)
    return identity.gameMode == "campaign_capture_the_flag"
       and identity.team == "a"
       and not identity.isHuman
end

local function shouldRouteAttackMate(identity)
    if not isCampaignTeamABot(identity) then return false end

    -- Live proof showed FirstPlayerId can point at a Team A AI process, not the
    -- human commander. Never use FirstPlayerId to exclude a bot from this route.
    -- On human attacks every Team A bot must avoid conquest.lua/utility.lua.
    if identity.attacking == true then return true end

    -- On defense the engine-owned DefenderBot remains on normal Code:X conquest
    -- logic. Any additional Team A slot is isolated in the read-only probe so it
    -- cannot purchase a second defense army.
    if identity.defenderBotId > 0 and identity.playerId == identity.defenderBotId then
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
    local isDefenderBot = identity.defenderBotId > 0 and identity.playerId == identity.defenderBotId
    routerLog(
        "classify",
        "playerId", identity.playerId,
        "team", identity.team,
        "army", identity.army,
        "gameMode", identity.gameMode,
        "attacking", tostring(identity.attacking),
        "firstPlayerId", identity.firstPlayerId,
        "firstEnemyId", identity.firstEnemyId,
        "defenderBotId", identity.defenderBotId,
        "isDefenderBot", tostring(isDefenderBot)
    )

    if shouldRouteAttackMate(identity) then
        routerLog(
            "route", "attacker_mate",
            "playerId", identity.playerId,
            "reason", identity.attacking == true and "team_a_attack_safe_route" or "extra_team_a_defense_isolation"
        )
        require("resource/script/multiplayer/modes/attacker_mate")
        return
    end

    local mode = gameModeMap[identity.gameMode]
    if not mode then
        routerLog("route_failed", "unknown_game_mode", identity.gameMode, "playerId", identity.playerId)
        return
    end

    local gameModeScriptPath = "resource/script/multiplayer/modes/" .. mode
    routerLog(
        "route", mode,
        "playerId", identity.playerId,
        "reason", isDefenderBot and "defenderbot_normal_controller" or "non_team_a_or_non_campaign"
    )
    require(gameModeScriptPath)

    -- Load Battle Zones-specific reliability and purchasing overrides only after
    -- the base mode has registered its shared functions and event handlers.
    if identity.gameMode == "battle_zones" or identity.gameMode == "ammunition" then
        require([[/script/multiplayer/modes/battlezones_overhaul]])
    end

    if initialize then
        initialize()
    end
end

initializeBotAI()
