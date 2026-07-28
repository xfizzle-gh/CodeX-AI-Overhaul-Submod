-- Config file created by Hawka
-- This file can be used for configurations of Conquest Enhanced as well as enable testing mode. 
-- However, not all configurations are tested so CHANGE AT YOUR OWN RISK.
-- IF YOU MODIFY THIS FILE, ALL PLAYERS FOR A COOP MATCH MUST HAVE THE SAME CONFIG FILE.You WILL get a de-sync if all the files and values do not match.
-- If you have any questions or requests for the file, you can message Hawka on steam or the GOH discord.

-- ================================= Standard Settings =========================================
	-- Prints more information in game.log for debugging purposes
	printDebug = true
	printTempDebug = false

	-- This enables testing mode for debugging, and is not advised for regular play. Make sure this is off unless you are specifically testing for something. 
	-- testing = false
	
	-- This is the strategy the ai will use to spawn units. If you want to randomly select one, just set it to nil
	-- strategyIndexOverride = 3

	-- weather_selection_override = 0

	-- Time from start of match AI will wait before attempting to buy a unit in mins
	oneFlagOffsetTime = {
	    -- Bot is defender
    	DefenseMin = 0, 
    	DefenseMax = 0,
    	-- Bot is attacker
    	AttackMin = 7, 
    	AttackMax = 8,
	}

	twoFlagOffsetTime = oneFlagOffsetTime
	threeFlagOffsetTime = {
	     -- Bot is defender
    	DefenseMin = 0, 
    	DefenseMax = 1,
    	-- Bot is attacker
    	AttackMin = 8, 
    	AttackMax = 9,
	}
	fourFlagOffsetTime = {
	     -- Bot is defender
    	DefenseMin = 1, 
    	DefenseMax = 1,
    	-- Bot is attacker
    	AttackMin = 9, 
    	AttackMax = 10,
	}
	fiveFlagOffsetTime = {
	     -- Bot is defender
    	DefenseMin = 1, 
    	DefenseMax = 1,
    	-- Bot is attacker
    	AttackMin = 10, 
    	AttackMax = 11,
	}

	-- Time when the AI reinforcements are called in when testing is set to true
	firstWaveOffsetTimeForTesting = 1.5

	-- Time from last purchase AI will wait before attempting to buy a new unit.
	DCGWaveOffOverwrite = {
		-- Time between each wave when bot is attacking
		AttackMinWaveOff = 3.0 * 60000,
		AttackMaxWaveOff = 5.0 * 60000,
     	-- Time between each wave when bot is defending
     	DefenseMinWaveOff = 3.5 * 60000, 
     	DefenseMaxWaveOff = 5.0 * 60000,
     }

    -- Number of possible units than can be in a wave attack in conquest
	WaveUnitOverride = {
	    -- Bot is attacker
	    AttackMin = 4,
	    AttackMax = 7,
	    -- Bot is defender
	    DefendMin = 3,
	    DefendMax = 5,
	}

-- ================================= Advanced Settings =========================================
	-- Percentage chance (between 0 and 1) that the AI will spawn from the player's side randomly during battle
	enableRearAttackMechanics = 1.0

	-- Enable possibility for certain divisions to perform certain strategies (This is checked at the start of every AI wave)
	enableAiStrategy = 0.3

	-- Number of AI Defender Infantry and vehicles that are spawned for a attack or defense mission.
	AiDefenderCount = {
		-- Bot is attacker
		Attacking = {
			emplacement = {
				max = -1,
				perFlag = 1
			},
			infantry = {
				x2_cloneClount = 1,
				perFlag = 4,
				max_ai_defender_at_flag = 3

			},
			challengeMaps = {
				emplacement = {
				max = 6,
				perFlag = 1
				},
				infantry = {
					x2_cloneClount = 1,
					perFlag = 4,
					max_ai_defender_at_flag = 3
				},
			},
			difficultyModifier = {
				heroic = -2,
				hard = -1,
				normal = 0,
				easy = 2,
			}
		}, 
		-- Bot is defender
		Defending = {
			emplacement = {
				defenseLevelOne = 4,
				defenseLevelTwo = 6,
				defenseLevelThree = 8
			},

			infantry = {
				x5_cloneClount = 1,
				perFlag = 8,
				max_ai_defender_at_flag = 4
			},
			challengeMaps = {
				emplacement = {
				defenseLevelOne = 6,
				defenseLevelTwo = 10,
				defenseLevelThree = 14
				},
				infantry = {
					x5_cloneClount = 4,
					perFlag = 16,
					max_ai_defender_at_flag = 8
				},
			},
			difficultyModifier = {
				heroic = 2,
				hard = 1,
				normal = 0,
				easy = -3,
			}
		}
	}

	-- Set to true when noresus mod is enabled with CE
	enabledNoresus = 0

	-- Percentage chances (0..1). Missing values used to nil-crash SetCEMissionVariables on map load.
	enableCommunicationsCutMechanics = 0
	enableSabotageMechanics = 0
	enableAiAbandonMechanics = 0
-- =============================== Logging DO NOT MODIFY =======================================
	require([[/conquest_configuration/bot.mod_configuration]])

	-- print("Initial player conquest configuration:")
	-- print("Testing mode: ", testing)
	-- print("Verbose: ", verbose)
	-- print("Default first wave offset time for 1 flag missions: ", oneFlagOffsetTime)
	-- print("Default first wave offset time for 2 flag missions: ", twoFlagOffsetTime)
	-- print("Default first wave offset time for 3 flag missions: ", threeFlagOffsetTime)
	-- print("Default first wave offset time for 4 flag missions: ", fourFlagOffsetTime)
	-- print("Default first wave offset time for 5 flag missions: ", fiveFlagOffsetTime)
	-- print("First wave offset time for testing: ", firstWaveOffsetTimeForTesting)
	-- print("Division to test: ", testingDivision)
	-- print("Chance to change first wave offset: ", chanceToOffsetFirstWave)
	-- print("Lower bound of first wave offset in seconds: ", lowerBoundFirstWaveOffset)
	-- print("Upper bound of first wave offset in seconds: ", upperBoundFirstWaveOffset)

	-- print("Typhoon wave mode chance: ", chanceToSetTyphoonWaveMode)
	-- print("Default typhoon wave mode interval: ", typhoonWaveInterval)
	-- print("Default typhoon wave mode duration: ", typhoonWaveDuration)
	-- print("Typhoon wave mode dynamic toggle chance: ", chanceToSetTyphoonWaveModeToggle)
	-- print("Default typhoon wave mode dynamic toggle: ", typhoonWaveToggleInterval)

	-- for faction, division in pairs(maxNumberOfDivisions) do
	-- 	for j, k in pairs(maxNumberOfDivisions.Faction) do
	-- 		print("Max number of ", j, " divisions: ", k)
	-- 	end
	-- end
