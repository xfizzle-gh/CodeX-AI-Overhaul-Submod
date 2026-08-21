-- File created by Hawka
require([[/script/multiplayer/modes/bot.ai_logic]])
-- =================== Vanilla Variable Redefines ==================
-- Time from start of match AI will wait before attempting to buy a unit.
StartSpawnTime = {
 -- Bot is defender
 DefenseMin = 5 * 1000, 
 DefenseMax = 7 * 1000,
 -- Bot is attacker
 AttackMin = 6 * 60000, 
 AttackMax = 8 * 60000,
}

-- Time from last purchase AI will wait before attempting to buy a new unit.
SpawnCooldownTime = {
 -- Time between each wave
 DCGWaveOffMin = 2 * 60000, 
 DCGWaveOffMax = 2.5 * 60000,
 -- Time between each spawn
 DCGMin = 2 * 1000, 
 DCGMax = 7 * 1000,
}


-- Number of possible units than can be in a wave attack
WaveUnit = {
 Min = 7,
 Max = 10,
}

-- value to modify total defender ai infantry values
botDifficultyModifier = 0


-- =================== CE Mechanics Variable Set Functions ==================
challenge_map = false
function CheckIfChallengeMap()
  challenge_map = false
  for i, flag in pairs(BotApi.Scene.Flags) do
    if flag.name == "f99" or flag.name == "f98" or flag.name == "f97" or flag.name == "f96" or flag.name == "f95" then
      challenge_map = true

      break
    end
  end
  print("challenge map check = ", challenge_map)
end

local function checkMapAIMovementLogic(flagName)
  print("checking flags to disable custom waypoints")
  if followWaypointGraphs == true then 
    for i = 1, 5, 1 do 
        if flagName == "f1" .. i then
        followWaypointGraphs = false
        print("flag name ", flagName, " disable waypoints")
        break
        end
    end
  end
  
  if followWaypointGraphs == true then 
    for f = 5, 9, 1 do 
        if flagName == "f9" .. f then
        followWaypointGraphs = false
        print("challenge map, disable waypoints")
        break
        end
    end
  end
end

local function checkVarPercentage(varName, varPecetage)
  math.randomseed(os.time())
  if varPecetage == nil then varPecetage = 0 end
  if varPecetage >= math.random() then
    varPecetage = 1
  else 
    varPecetage = 0
  end
  BotApi.Scene:SetVar(varName, varPecetage)

  print(varName .. " = ", varPecetage)
end

local function checkRearAttackPercentage()
  local chance = enableRearAttackMechanics
  if chance == nil then chance = 0 end
  if chance >= math.random() then
    enableRearAttackMechanics = 1
  else 
    enableRearAttackMechanics = 0
  end
  BotApi.Scene:SetVar("enable_rear_attack_mechanic", enableRearAttackMechanics)

  print("enable_rear_attack_mechanic" .. " = ", enableRearAttackMechanics)
end

function SetCEMissionVariables(botDefender)
  BotApi.Scene:SetVar("noresusenabled", enabledNoresus)
  print("enabledNoresus == ", enabledNoresus)

  local totalFlags = 0
  for i, flag in pairs(BotApi.Scene.Flags) do
    -- print("i: ", i)
    print("flag name: ", flag.name)
    print("flag occupant: ", flag.occupant)
    
    if followWaypointGraphs then
      checkMapAIMovementLogic(flag.name)
    end
    totalFlags = totalFlags + 1
  end
  print("CE flag loop done count=", totalFlags)

  if followWaypointGraphs then
      BotApi.Scene:SetVar("enable_ai_waypoint_graphs", 1)
  else
      BotApi.Scene:SetVar("enable_ai_waypoint_graphs", 0)
  end
  print("CE post-flag vars begin")

  -- checkVarPercentage("weather_selection", weather_selection_override)

  -- Defaults: these were referenced but never defined in bot.conquest_configuration,
  -- which nil-crashes Lua on human-defense map init right after flag waypoint checks.
  local radioCut = enableCommunicationsCutMechanics
  if radioCut == nil then radioCut = 0 end
  local sabotage = enableSabotageMechanics
  if sabotage == nil then sabotage = 0 end
  local abandon = enableAiAbandonMechanics
  if abandon == nil then abandon = 0 end
  local morale = enableCeMoraleMechanic
  if morale == nil then morale = 0 end
  local moraleDebug = enableCeMoraleDebug
  if moraleDebug == nil then moraleDebug = 0 end
  local moraleAutodemo = enableCeMoraleAutodemo
  if moraleAutodemo == nil then moraleAutodemo = 0 end

  --checkVarPercentage("enable_ce_radio_mechanic", enableRadioMechanics)
  checkVarPercentage("enable_ce_radio_mechanic", radioCut) -- use this to control both variables for now
  checkVarPercentage("enable_ce_cut_communications_mechanic", radioCut)
  checkVarPercentage("ai_sabotage", sabotage)
  checkVarPercentage("enable_ai_abandon_mechanics", abandon)
  checkVarPercentage("enable_ce_morale_mechanic", morale)
  checkVarPercentage("enable_ce_morale_debug", moraleDebug)
  checkVarPercentage("enable_ce_morale_autodemo", moraleAutodemo)
  BotApi.Scene:SetVar("ce_morale_force_shaken", 0)
  BotApi.Scene:SetVar("ce_morale_force_panic", 0)
  BotApi.Scene:SetVar("ce_morale_autodemo_done", 0)
  BotApi.Scene:SetVar("ce_morale_source_tag_seen", 0)
  BotApi.Scene:SetVar("ce_morale_source_quality_seen", 0)
  BotApi.Scene:SetVar("ce_morale_source_command_seen", 0)
  BotApi.Scene:SetVar("ce_morale_autodemo_shaken_done", 0)
  BotApi.Scene:SetVar("ce_morale_late_seen", 0)
  BotApi.Scene:SetVar("ce_morale_diag_mi_alive", 0)
  BotApi.Scene:SetVar("ce_morale_diag_human", 0)
  BotApi.Scene:SetVar("ce_morale_diag_tag_roundtrip", 0)
  BotApi.Scene:SetVar("ce_morale_diag_existing_tag_read", 0)
  BotApi.Scene:SetVar("ce_morale_diag_existing_soldier", 0)
  BotApi.Scene:SetVar("ce_morale_diag_add_action_ran", 0)
  BotApi.Scene:SetVar("ce_morale_diag_added_tag_read", 0)
  BotApi.Scene:SetVar("ce_morale_diag_source_tag_read", 0)
  BotApi.Scene:SetVar("ce_morale_diag_known_tag", 0)
  BotApi.Scene:SetVar("ce_morale_diag_pr_a_source", 0)
  BotApi.Scene:SetVar("ce_morale_diag_canary_present", 0)
  BotApi.Scene:SetVar("ce_morale_diag_inventory_canary", 0)
  BotApi.Scene:SetVar("ce_morale_diag_shaken", 0)
  BotApi.Scene:SetVar("ce_morale_diag_panic", 0)
  BotApi.Scene:SetVar("ce_morale_diag_player_hit", 0)
  BotApi.Scene:SetVar("ce_morale_diag_ai_human", 0)
  BotApi.Scene:SetVar("ce_morale_diag_cmd_link", 0)
  BotApi.Scene:SetVar("ce_morale_diag_cmd_lost", 0)
  BotApi.Scene:SetVar("ce_morale_diag_cmd_shock", 0)
  BotApi.Scene:SetVar("ce_morale_diag_cmd_encourage", 0)
  BotApi.Scene:SetVar("ce_morale_diag_vet_live", 0)
  BotApi.Scene:SetVar("ce_morale_diag_pressure", 0)
  BotApi.Scene:SetVar("ce_morale_diag_recover", 0)
  BotApi.Scene:SetVar("ce_morale_diag_recover_panic", 0)
  BotApi.Scene:SetVar("ce_morale_diag_recover_clear", 0)
  BotApi.Scene:SetVar("ce_morale_diag_suppressed_state", 0)
  BotApi.Scene:SetVar("ce_morale_diag_broken", 0)
  BotApi.Scene:SetVar("ce_morale_diag_retreat", 0)
  BotApi.Scene:SetVar("ce_morale_diag_surrender", 0)
  BotApi.Scene:SetVar("ce_morale_diag_present", 0)
  BotApi.Scene:SetVar("ce_morale_diag_assign", 0)
  BotApi.Scene:SetVar("ce_morale_diag_p0", 0)
  BotApi.Scene:SetVar("ce_morale_diag_drop", 0)
  BotApi.Scene:SetVar("ce_morale_diag_impregnable", 0)
  BotApi.Scene:SetVar("ce_morale_diag_evac", 0)
  BotApi.Scene:SetVar("ce_morale_diag_expire", 0)
  BotApi.Scene:SetVar("ce_morale_diag_held", 0)
  BotApi.Scene:SetVar("ce_morale_diag_delete", 0)
  BotApi.Scene:SetVar("aio_pow_next_id", 0)
  BotApi.Scene:SetVar("aio_pow_seq", 0)
  BotApi.Scene:SetVar("aio_pow_last_evt", 0)
  BotApi.Scene:SetVar("ce_morale_sys_done", 0)
  StartCeMoraleProbeLog()


  -- only run rear attack script if bot is attacking
  BotApi.Scene:SetVar("max_ai_defender_emplacement_count_level_1", AiDefenderCount.Defending.emplacement.defenseLevelOne)
  BotApi.Scene:SetVar("max_ai_defender_emplacement_count_level_2", AiDefenderCount.Defending.emplacement.defenseLevelTwo)
  BotApi.Scene:SetVar("max_ai_defender_emplacement_count_level_3", AiDefenderCount.Defending.emplacement.defenseLevelThree)
  if botDefender then
    enableRearAttackMechanics = 0
    if challenge_map then
      BotApi.Scene:SetVar("max_ai_defender_inf_per_flag_count", AiDefenderCount.Defending.challengeMaps.infantry.perFlag + botDifficultyModifier)
      BotApi.Scene:SetVar("max_ai_defender_at_flag", AiDefenderCount.Defending.infantry.max_ai_defender_at_flag)
      -- BotApi.Scene:SetVar("max_ai_defender_inf_count", AiDefenderCount.Defending.challengeMaps.infantry.max)
      BotApi.Scene:SetVar("max_ai_inf_def_x5_count", AiDefenderCount.Defending.challengeMaps.infantry.x5_cloneClount)
    else
      print("setting ai defender count for bot defending")
      BotApi.Scene:SetVar("max_ai_defender_inf_per_flag_count", AiDefenderCount.Defending.infantry.perFlag + botDifficultyModifier)
      BotApi.Scene:SetVar("max_ai_defender_at_flag", AiDefenderCount.Defending.infantry.max_ai_defender_at_flag)
      -- BotApi.Scene:SetVar("max_ai_defender_inf_count", AiDefenderCount.Defending.infantry.max)
      BotApi.Scene:SetVar("max_ai_inf_def_x5_count", AiDefenderCount.Defending.infantry.x5_cloneClount) 
    end
  else
     if challenge_map then
       BotApi.Scene:SetVar("max_ai_defender_inf_per_flag_count", AiDefenderCount.Attacking.challengeMaps.infantry.perFlag + botDifficultyModifier)
       BotApi.Scene:SetVar("max_ai_defender_at_flag", AiDefenderCount.Attacking.infantry.max_ai_defender_at_flag)
      -- BotApi.Scene:SetVar("max_ai_defender_inf_count", AiDefenderCount.Attacking.challengeMaps.infantry.max)
      BotApi.Scene:SetVar("max_ai_inf_def_x5_count", AiDefenderCount.Attacking.challengeMaps.infantry.x2_cloneClount)
      BotApi.Scene:SetVar("max_ai_defender_emplacement_total_count", AiDefenderCount.Attacking.challengeMaps.emplacement.perFlag * totalFlags)
     else
      print("setting ai emplacement defender count for bot attacking = ", AiDefenderCount.Attacking.emplacement.perFlag * totalFlags)
      print("setting ai defender count for bot attacking = ",  AiDefenderCount.Attacking.infantry.perFlag + botDifficultyModifier)
      BotApi.Scene:SetVar("max_ai_defender_at_flag", AiDefenderCount.Attacking.infantry.max_ai_defender_at_flag)
      -- BotApi.Scene:SetVar("max_ai_defender_inf_count", AiDefenderCount.Attacking.infantry.max)
      BotApi.Scene:SetVar("max_ai_defender_inf_per_flag_count", AiDefenderCount.Attacking.infantry.perFlag + botDifficultyModifier)
      BotApi.Scene:SetVar("max_ai_inf_def_x5_count", AiDefenderCount.Attacking.infantry.x2_cloneClount)
      BotApi.Scene:SetVar("max_ai_defender_emplacement_total_count", AiDefenderCount.Attacking.emplacement.perFlag * totalFlags)
    end
  end
  -- checkVarPercentage("enable_rear_attack_mechanic", enableRearAttackMechanics)
  checkRearAttackPercentage()
  BotApi.Scene:SetVar("max_ai_defender_emplacement_count_level_1", AiDefenderCount.Defending.emplacement.defenseLevelOne)
  BotApi.Scene:SetVar("max_ai_defender_emplacement_count_level_2", AiDefenderCount.Defending.emplacement.defenseLevelTwo)
  BotApi.Scene:SetVar("max_ai_defender_emplacement_count_level_3", AiDefenderCount.Defending.emplacement.defenseLevelThree)
  -- BotApi.Scene:SetVar("force_ai_direct_attack_logic", force_ai_direct_attack_logic)
end

function KillGeneralSquadTagCheckTimer()
  if Context.GeneralSquadTagCheckTimer then 
    BotApi.Events:KillQuantTimer(Context.GeneralSquadTagCheckTimer)
    Context.GeneralSquadTagCheckTimer = nil
  end
end

function KillInitialSceneCheckTimer()
  if Context.InitialSceneTimerCheck then
    BotApi.Events:KillQuantTimer(Context.InitialSceneTimerCheck)
    Context.InitialSceneTimerCheck = nil
  end
end

function KillAiSpawnMoveTimer()
  if Context.AiSpawnMoveTimer then
    BotApi.Events:KillQuantTimer(Context.AiSpawnMoveTimer)
    Context.AiSpawnMoveTimer = nil
  end
end

aiSpawnStrategy = 0
sceneVariableSquad = nil

function SelectAiSpawnStrategy()
  print("in SelectAiSpawnStrategy function")
  KillAiSpawnMoveTimer()
  local delay = checkAiSpawnMoveDelay or (3 * 60 * 1000)
  local function loop(callback)
    Context.AiSpawnMoveTimer = BotApi.Events:SetQuantTimer(
      function()
        Context.AiSpawnMoveTimer = nil
        if math.random() < 0.5 then
          local prev = aiSpawnStrategy
          if enableRearAttackMechanics == 1 then
            aiSpawnStrategy = math.random(0, 3)
          else
            aiSpawnStrategy = math.random(0, 2)
          end
          print("Ai spawn strategy = ", aiSpawnStrategy)
          if aiSpawnStrategy == 3 then
            followWaypointGraphs = false
            BotApi.Scene:SetVar("enable_ai_waypoint_graphs", 0)
          else
            followWaypointGraphs = true
            BotApi.Scene:SetVar("enable_ai_waypoint_graphs", 1)
          end
          if prev == 3 or aiSpawnStrategy == 3 then
            BotApi.Scene:SetVar("change_ai_spawns", 1)
          end
          BotApi.Scene:SetVar("ai_spawn_strategy", aiSpawnStrategy)
        end
        callback(callback)
      end, delay)
  end
  loop(loop)
end

function setAiSpawnIndex(SpawnPointIndex)
  if aiSpawnStrategy == 1 then
    if SpawnPointIndex == 0 then
      return 3
    end
    return 0
  elseif aiSpawnStrategy == 2 then
    if SpawnPointIndex == 1 then
      return 2
    end
    return 1
  end
  return SpawnPointIndex + 1
end

sceneVarRequested = false

function SpawnSceneVariable()
  sceneVarRequested = false
end

function StartSceneCheckTimer()
  if not sceneVariableSquad then return end
  BotApi.Events:SetQuantTimer(function()
    if BotApi.Scene:IsSquadTagged(sceneVariableSquad, "_ce_map_scripts_running") then
      followWaypointGraphs = true
      BotApi.Scene:SetVar("enable_ai_waypoint_graphs", 1)
    else
      followWaypointGraphs = false
      BotApi.Scene:SetVar("enable_ai_waypoint_graphs", 0)
    end
    print("CUSTOM WAYPOINTS = ", followWaypointGraphs)
  end, 1000)
end

function CheckSceneVariable(squad)
  if not squad then return end
  if followWaypointGraphs then
    if BotApi.Scene:IsSquadTagged(squad, "_lua_waypoint_graph_disabled") then
      followWaypointGraphs = false
    end
  elseif BotApi.Scene:IsSquadTagged(squad, "_lua_waypoint_graph_enabled") then
    followWaypointGraphs = true
  end
end

function SetFirstWaveOffset(flagCount)
  if flagCount == 1 then
    StartSpawnTime = oneFlagOffsetTime
  elseif flagCount == 2 then
    StartSpawnTime = twoFlagOffsetTime
  elseif flagCount == 3 then
    StartSpawnTime = threeFlagOffsetTime
  elseif flagCount == 4 then
    StartSpawnTime = fourFlagOffsetTime
  elseif flagCount == 5 then
    StartSpawnTime = fiveFlagOffsetTime
  end

  if testing then
    StartSpawnTime.DefenseMin = firstWaveOffsetTimeForTesting 
    StartSpawnTime.DefenseMax = firstWaveOffsetTimeForTesting
    StartSpawnTime.AttackMin = firstWaveOffsetTimeForTesting
    StartSpawnTime.AttackMax = firstWaveOffsetTimeForTesting
  end 

  StartSpawnTime.DefenseMin = StartSpawnTime.DefenseMin * 60000
  StartSpawnTime.DefenseMax = StartSpawnTime.DefenseMax * 60000
  StartSpawnTime.AttackMin = StartSpawnTime.AttackMin * 60000
  StartSpawnTime.AttackMax = StartSpawnTime.AttackMax * 60000
end

function SetCEWaveSettings()
  print("TESTING MODE = ", testing)
  local totalFlags = 0
  for i, flag in pairs(BotApi.Scene.Flags) do
    totalFlags = totalFlags + 1
  end

  if botDefender then
    WaveUnit.Min = WaveUnitOverride.DefendMin
    WaveUnit.Max = WaveUnitOverride.DefendMax
    SpawnCooldownTime.DCGWaveOffMin = DCGWaveOffOverwrite.DefenseMinWaveOff
    SpawnCooldownTime.DCGWaveOffMax = DCGWaveOffOverwrite.DefenseMaxWaveOff
  else
    WaveUnit.Min = WaveUnitOverride.AttackMin
    WaveUnit.Max = WaveUnitOverride.AttackMax
    SpawnCooldownTime.DCGWaveOffMin = DCGWaveOffOverwrite.AttackMinWaveOff
    SpawnCooldownTime.DCGWaveOffMax = DCGWaveOffOverwrite.AttackMaxWaveOff
  end

  if printDebug then 
    print("WaveUnit.Min = ", WaveUnit.Min)
    print("WaveUnit.Max = ", WaveUnit.Max)
  end

  SetFirstWaveOffset(totalFlags)
end

local function readMoraleVar(name)
  local ok, value = pcall(function()
    return BotApi.Scene:GetVar(name)
  end)
  if not ok or value == nil then
    return 0
  end
  return tonumber(value) or 0
end

local function countSquadsTagged(tag)
  local n = 0
  local ok, squads = pcall(function()
    return BotApi.Scene.Squads
  end)
  if ok and type(squads) == "table" then
    for _, squad in pairs(squads) do
      local tok, tagged = pcall(function()
        return BotApi.Scene:IsSquadTagged(squad, tag)
      end)
      if tok and tagged then
        n = n + 1
      end
    end
  end
  return n
end

-- Diagnostic-only POW trail. Uses declared mission vars (same pattern as
-- ce_morale_diag_surrender$ → CE_MORALE_EVENT surrender). Entity tags are
-- invisible to IsSquadTagged; do not poll squad tags for this trail.
-- Entity hex is not BotApi-readable.
local powDiagWatchStarted = false

local function startPowDiagWatch()
  if powDiagWatchStarted then
    return
  end
  powDiagWatchStarted = true
  local seenPresent = false
  local seenAssign = false
  local seenP0 = false
  local seenImpregnable = false
  local seenDrop = false
  local seenEvac = false
  local seenExpire = false
  local seenHeld = false
  local seenDelete = false
  print("CE_POW_DIAG event=watch_armed entity=unreadable breed=unreadable orig_player=unreadable curr_player=unreadable squad=unreadable sensor=unreadable")
  local function watch()
    if not seenPresent and readMoraleVar("ce_morale_diag_present") > 0 then
      seenPresent = true
      print("CE_POW_DIAG event=present entity=unreadable breed=unreadable orig_player=unreadable curr_player=unreadable squad=unreadable sensor=unreadable")
    end
    if not seenAssign and readMoraleVar("ce_morale_diag_assign") > 0 then
      seenAssign = true
      print("CE_POW_DIAG event=assign entity=unreadable breed=unreadable orig_player=unreadable curr_player=unreadable squad=unreadable sensor=unreadable")
    end
    if not seenP0 and readMoraleVar("ce_morale_diag_p0") > 0 then
      seenP0 = true
      print("CE_POW_DIAG event=p0 entity=unreadable breed=unreadable orig_player=unreadable curr_player=0_inferred squad=unreadable sensor=unreadable")
    end
    if not seenImpregnable and readMoraleVar("ce_morale_diag_impregnable") > 0 then
      seenImpregnable = true
      print("CE_POW_DIAG event=impregnable entity=unreadable breed=unreadable orig_player=unreadable curr_player=unreadable squad=unreadable sensor=unreadable")
    end
    if not seenDrop and readMoraleVar("ce_morale_diag_drop") > 0 then
      seenDrop = true
      print("CE_POW_DIAG event=drop entity=unreadable breed=unreadable orig_player=unreadable curr_player=unreadable squad=unreadable sensor=unreadable")
    end
    if not seenEvac and readMoraleVar("ce_morale_diag_evac") > 0 then
      seenEvac = true
      print("CE_POW_DIAG event=evac entity=unreadable breed=unreadable orig_player=unreadable curr_player=unreadable squad=unreadable sensor=unreadable")
    end
    if not seenExpire and readMoraleVar("ce_morale_diag_expire") > 0 then
      seenExpire = true
      print("CE_POW_DIAG event=expire entity=unreadable breed=unreadable orig_player=unreadable curr_player=unreadable squad=unreadable sensor=unreadable")
    end
    if not seenHeld and readMoraleVar("ce_morale_diag_held") > 0 then
      seenHeld = true
      print("CE_POW_DIAG event=held entity=unreadable breed=unreadable orig_player=unreadable curr_player=unreadable squad=unreadable sensor=unreadable")
    end
    if not seenDelete and readMoraleVar("ce_morale_diag_delete") > 0 then
      seenDelete = true
      print("CE_POW_DIAG event=delete entity=unreadable breed=unreadable orig_player=unreadable curr_player=unreadable squad=unreadable sensor=unreadable")
    end
    BotApi.Events:SetQuantTimer(watch, 1000)
  end
  BotApi.Events:SetQuantTimer(watch, 1000)
end

-- Diagnostic-only 2s watcher for native POW tests. Re-gate or remove before production merge.
local function startMoraleEventWatch()
  local seenRetreat = false
  local seenSurrender = false
  local lastPow = -1
  local function watch()
    if not seenRetreat and readMoraleVar("ce_morale_diag_retreat") > 0 then
      seenRetreat = true
      print("CE_MORALE_EVENT retreat")
    end
    local pow = countSquadsTagged("aio_morale_surrendering")
    if pow > 0 then
      BotApi.Scene:SetVar("ce_morale_diag_surrender", 1)
      if pow ~= lastPow then
        print("CE_POW alive=1 surrendering=" .. pow)
        lastPow = pow
      end
    elseif lastPow > 0 then
      print("CE_POW alive=0 surrendering=0")
      lastPow = 0
    end
    if not seenSurrender and (readMoraleVar("ce_morale_diag_surrender") > 0 or pow > 0) then
      seenSurrender = true
      print("CE_MORALE_EVENT surrender")
    end
    BotApi.Events:SetQuantTimer(watch, 2000)
  end
  BotApi.Events:SetQuantTimer(watch, 2000)
end

function StartCeMoraleProbeLog()
  startPowDiagWatch()
  if readMoraleVar("enable_ce_morale_debug") > 0 or readMoraleVar("enable_ce_morale_autodemo") > 0 then
    startMoraleEventWatch()
  end
  if readMoraleVar("enable_ce_morale_autodemo") <= 0 then
    return
  end
  print("CE_MORALE_PROBE armed waiting")
  local ticks = 0
  local printedFail = false
  local function tick()
    ticks = ticks + 1
    local source = readMoraleVar("ce_morale_source_tag_seen")
    local quality = readMoraleVar("ce_morale_source_quality_seen")
    local late = readMoraleVar("ce_morale_late_seen")
    local done = readMoraleVar("ce_morale_autodemo_done")
    local shaken = readMoraleVar("ce_morale_autodemo_shaken_done")
    local result = "waiting source=" .. source .. " quality=" .. quality .. " shaken=" .. shaken .. " late=" .. late .. " done=" .. done
    if done > 0 and source <= 0 then
      result = "source_fail"
    elseif done > 0 and quality <= 0 then
      result = "source_regular_only"
    elseif done > 0 and late <= 0 then
      result = "late_fail"
    elseif done > 0 and quality > 0 and late > 0 then
      result = "pass quality_ok shaken late_ok panic"
    elseif shaken > 0 then
      result = "in_progress quality_ok shaken waiting_late"
    end
    print("CE_MORALE_PROBE", result)
    local mi = readMoraleVar("ce_morale_diag_mi_alive")
    local human = readMoraleVar("ce_morale_diag_human")
    local roundtrip = readMoraleVar("ce_morale_diag_tag_roundtrip")
    local canary = 0
    local ok, squads = pcall(function()
      return BotApi.Scene.Squads
    end)
    if ok and type(squads) == "table" then
      for _, squad in pairs(squads) do
        local tok, tagged = pcall(function()
          return BotApi.Scene:IsSquadTagged(squad, "aio_morale_diag_roundtrip")
        end)
        if tok and tagged then
          canary = 1
          break
        end
      end
    end
    print("CE_MORALE_DIAG mi_alive=" .. mi .. " human=" .. human .. " tag_roundtrip=" .. roundtrip .. " source=" .. source .. " squad_canary=" .. canary)
    local tag_add = readMoraleVar("ce_morale_diag_add_action_ran")
    local tag_read = readMoraleVar("ce_morale_diag_added_tag_read")
    local known_tag = readMoraleVar("ce_morale_diag_known_tag")
    local pr_a = readMoraleVar("ce_morale_diag_pr_a_source")
    local canary_present = readMoraleVar("ce_morale_diag_canary_present")
    local inventory = readMoraleVar("ce_morale_diag_inventory_canary")
    local shaken_apply = readMoraleVar("ce_morale_diag_shaken")
    local panic_apply = readMoraleVar("ce_morale_diag_panic")
    local player_hit = readMoraleVar("ce_morale_diag_player_hit")
    local player_excluded = 0
    if (shaken_apply > 0 or panic_apply > 0) and player_hit <= 0 then
      player_excluded = 1
    end
    print("CE_MORALE_ARCH mi=" .. mi .. " human=" .. human .. " tag_add=" .. tag_add .. " tag_read=" .. tag_read .. " known_tag=" .. known_tag .. " pr_a_source=" .. pr_a .. " canary_present=" .. canary_present .. " inventory_canary=" .. inventory .. " shaken=" .. shaken_apply .. " panic=" .. panic_apply .. " player_excluded=" .. player_excluded)
    local ai_human = readMoraleVar("ce_morale_diag_ai_human")
    local cmd_link = readMoraleVar("ce_morale_diag_cmd_link")
    local pressure = readMoraleVar("ce_morale_diag_pressure")
    local recover = readMoraleVar("ce_morale_diag_recover")
    local recover_panic = readMoraleVar("ce_morale_diag_recover_panic")
    local suppressed_state = readMoraleVar("ce_morale_diag_suppressed_state")
    local broken = readMoraleVar("ce_morale_diag_broken")
    local retreat = readMoraleVar("ce_morale_diag_retreat")
    local surrender = readMoraleVar("ce_morale_diag_surrender")
    local recover_clear = readMoraleVar("ce_morale_diag_recover_clear")
    local cmd_lost = readMoraleVar("ce_morale_diag_cmd_lost")
    local cmd_shock = readMoraleVar("ce_morale_diag_cmd_shock")
    local cmd_encourage = readMoraleVar("ce_morale_diag_cmd_encourage")
    local vet_live = readMoraleVar("ce_morale_diag_vet_live")
    print("CE_MORALE_SYS mi=" .. mi .. " human=" .. human .. " tag_add=" .. tag_add .. " tag_read=" .. tag_read .. " known_tag=" .. known_tag .. " pr_a=" .. pr_a .. " canary=" .. canary_present .. " inv=" .. inventory .. " ai=" .. ai_human .. " pressure=" .. pressure .. " suppressed=" .. suppressed_state .. " shaken=" .. shaken_apply .. " recover=" .. recover .. " recover_panic=" .. recover_panic .. " recover_clear=" .. recover_clear .. " panic=" .. panic_apply .. " player_ex=" .. player_excluded .. " cmd_link=" .. cmd_link .. " cmd_lost=" .. cmd_lost .. " cmd_shock=" .. cmd_shock .. " cmd_encourage=" .. cmd_encourage .. " vet_live=" .. vet_live .. " broken=" .. broken .. " retreat=" .. retreat .. " surrender=" .. surrender)
    if ticks >= 2 then
      local fails = {}
      if human > 0 and tag_add <= 0 then
        fails[#fails + 1] = "TAG_ADD_FAIL"
      end
      if tag_add > 0 and tag_read <= 0 then
        fails[#fails + 1] = "TAG_READ_FAIL"
      end
      if human > 0 and pr_a <= 0 then
        fails[#fails + 1] = "PR_A_SOURCE_FAIL"
      end
      if human > 0 and canary_present <= 0 then
        fails[#fails + 1] = "CANARY_ABSENT"
      end
      if canary_present > 0 and inventory <= 0 then
        fails[#fails + 1] = "INVENTORY_CANARY_FAIL"
      end
      if ai_human <= 0 then
        fails[#fails + 1] = "AI_ABSENT"
      end
      if ticks >= 12 and shaken_apply <= 0 then
        fails[#fails + 1] = "SHAKEN_APPLY_FAIL"
      end
      if ticks >= 12 and panic_apply <= 0 then
        fails[#fails + 1] = "PANIC_APPLY_FAIL"
      end
      if suppressed_state <= 0 and recover_panic <= 0 and panic_apply > 0 then
        fails[#fails + 1] = "RECOVER_PANIC_FAIL"
      end
      if suppressed_state <= 0 and recover_clear <= 0 and shaken_apply > 0 then
        fails[#fails + 1] = "RECOVER_FAIL"
      end
      if #fails > 0 and not printedFail then
        printedFail = true
        print("CE_MORALE_SYS_FAIL " .. table.concat(fails, " "))
      end
    end
    if ticks < 24 then
      BotApi.Events:SetQuantTimer(tick, 5000)
    end
  end
  BotApi.Events:SetQuantTimer(tick, 5000)
end

-- =================== Noresus Mechanics ==================
function NoresusOnGameStart()
  if enabledNoresus == 0 then
    print("Skipping Noresus setup logic")
    return
  end
  -- INITIAL STATS
  local salva = io.open("stats.start", "r")
  if salva==nil then
    salva = io.open("stats.start", "w")
    salva:write("first")
    salva:close()
  else
    playerCondition="COWARD"
    salva = io.open("stats.start", "w")
    salva:write(playerCondition)
    salva:close()
    OnGameStop()
  end
end

function NoresusOnGameEnd()
  if enabledNoresus == 0 then
    print("Skipping Noresus teardown logic")
    return
  end
  -- CREA IL FILE CON I RISULTATI DELLE BASI
  if playerCondition~="COWARD" then
    salva = io.open("stats.end", "w")
    for i, flag in pairs(BotApi.Scene.Flags) do
      local rig=""  
      for r, v in pairs(flag) do  -- base e team (2 cicli)
        rig=rig..v..";"
      end
      rig=string.sub(rig,1,-2)
      rig=rig.."\n"
      salva:write(rig)
    end
    salva:close()   
  end
end

-- =================== Double Queue Data Structure ==================
DoubleQueue = {}
function DoubleQueue.new ()
  return {first = 0, last = -1}
end

function DoubleQueue.pushRight (list, value)
  local last = list.last + 1
  list.last = last
  list[last] = value
end

function DoubleQueue.popLeft (list)
  local first = list.first
  if first > list.last then error("list is empty") end
  local value = list[first]
  list[first] = nil        -- to allow garbage collection
  list.first = first + 1
  return value
end

function DoubleQueue.size(list)
  local first = list.first
  if first > list.last then return 0 
  else return math.abs(list.last - first) + 1 end
end
