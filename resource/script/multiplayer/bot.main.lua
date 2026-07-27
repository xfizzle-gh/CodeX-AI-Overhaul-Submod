-- Function to require and initialize the appropriate game mode .lua file

local function initializeBotAI()
    local gameModeMap = {
        campaign_capture_the_flag = "conquest",
        battle_zones = "battlezones",
        ammunition = "battlezones", -- ammunition aka domination
        evacuation = "laststand",
        frontlines = "frontlines",
    }

    local gameModeScriptPath = "resource/script/multiplayer/modes/" .. gameModeMap[BotApi.Instance.gameMode]
    require(gameModeScriptPath)

    -- Load Battle Zones-specific reliability and purchasing overrides only after
    -- the base mode has registered its shared functions and event handlers.
    if BotApi.Instance.gameMode == "battle_zones" or BotApi.Instance.gameMode == "ammunition" then
        require([[/script/multiplayer/modes/battlezones_overhaul]])
    end

    if initialize then
        initialize()
    end
end

initializeBotAI()
