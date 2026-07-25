
-- Function to require and initialize the appropriate game mode .lua file
local function initializeBotAI()
    local gameModeMap = {
        campaign_capture_the_flag = "conquest",
        battle_zones = "battlezones",
        ammunition = "battlezones", -- ammuntion aka "domination"
        evacuation = "laststand",
        frontlines = "frontlines",
    }

    local gameModeScriptPath = "resource/script/multiplayer/modes/" .. gameModeMap[BotApi.Instance.gameMode]
    require(gameModeScriptPath)
    if initialize then 
        initialize() 
    end
end

initializeBotAI()