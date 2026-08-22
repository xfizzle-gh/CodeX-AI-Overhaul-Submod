-- File created by Hawka
require([[/conquest_configuration/bot.conquest_configuration]])
-- Noresus AI battalion type (only used for Noresus mod)
StrategyTemplates = {
	{-- Index 1
		-- enableHumanWaveTactic = 1,
		StrategyName = "Human Wave Strategy",
		StrategyUnitTypes = {
			"Infantry",
			"Team",
			"AT",
		},
		BotInfantry = 1.2,
		BotTeamInfantry = 1.12,
		BotATInfantry = 1.15,
		BotInfantrySignaller = 1.0,
		BotArtillery = 0.95,
		BotMortars = 1.0,
		BotEmplacements = 0.95,
		BotTanks = 0.95,
		BotHeavyTanks = 0.9,
		BotArmored = 1.0,
		BotSPGs = 0.95,
		BotTankDestroyers = 0.95,
		BotAircraft = 0.9,
		BotReconAircraft = 0.95,
		BotParatroopers = 0.25,
		forceUnitCount = {
			min = 2,
			max = 3,
		},
		-- BotInfantry = {
		-- 	min = 70,
		-- 	max = 140,
		-- },
		-- BotATInfantry = {
		-- 	min = 4,
		-- 	max = 8,
		-- },
		-- BotInfantrySignaller = {
		-- 	min = 1,
		-- 	max = 3,
		-- },
		-- BotArtillery = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotMortars = {
		-- 	min = 1,
		-- 	max = 3,
		-- },
		-- BotEmplacements = {
		-- 	min = 1,
		-- 	max = 5,
		-- },
		-- BotTanks = {
		-- 	min = 4,
		-- 	max = 6,
		-- },
		-- BotHeavyTanks = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotSPGs = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotTankDestroyers = {
		-- 	min = 2,
		-- 	max = 4,
		-- },
		-- BotAircraft = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotReconAircraft = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotArmored = {
		-- 	min = 2,
		-- 	max = 5,
		-- },
	},
	{-- Index 2
		-- enableArtyTactic = 1,
		StrategyName = "Artillery Strategy",
		StrategyUnitTypes = {
			"Artillery",
			"Cannon",
		},
		BotInfantry = 1.15,
		BotTeamInfantry = 1.1,
		BotATInfantry = 1.12,
		BotInfantrySignaller = 1.1,
		BotArtillery = 1.15,
		BotMortars = 1.1,
		BotEmplacements = 1.0,
		BotTanks = 0.95,
		BotHeavyTanks = 0.9,
		BotArmored = 0.95,
		BotSPGs = 1.05,
		BotTankDestroyers = 0.95,
		BotAircraft = 0.9,
		BotReconAircraft = 1.0,
		BotParatroopers = 0.25,
		forceUnitCount = {
			min = 1,
			max = 2,
		},
		-- BotInfantry = {
		-- 	min = 40,
		-- 	max = 95,
		-- },
		-- BotATInfantry = {
		-- 	min = 1,
		-- 	max = 4,
		-- },
		-- BotInfantrySignaller = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotArtillery = {
		-- 	min = 1,
		-- 	max = 3,
		-- },
		-- BotMortars = {
		-- 	min = 3,
		-- 	max = 6,
		-- },
		-- BotEmplacements = {
		-- 	min = 0,
		-- 	max = 1,
		-- },
		-- BotTanks = {
		-- 	min = 2,
		-- 	max = 3,
		-- },
		-- BotHeavyTanks = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotSPGs = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotTankDestroyers = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotAircraft = {
		-- 	min = 0,
		-- 	max = 0,
		-- },
		-- BotReconAircraft = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotArmored = {
		-- 	min = 2,
		-- 	max = 5,
		-- },
	},
	{-- Index 3
		-- enableAirborneTactic = 1,
		StrategyName = "Airborne Strategy",
		StrategyUnitTypes = {
			"Sortie",
			"Aircraft",
		},
		StrategyExcludeUnitTypes = {
			"Air",
		},
		BotInfantry = 1.15,
		BotTeamInfantry = 1.1,
		BotATInfantry = 1.12,
		BotInfantrySignaller = 1.0,
		BotArtillery = 0.95,
		BotMortars = 1.0,
		BotEmplacements = 0.95,
		BotTanks = 0.95,
		BotHeavyTanks = 0.9,
		BotArmored = 0.95,
		BotSPGs = 0.95,
		BotTankDestroyers = 0.95,
		BotAircraft = 1.15,
		BotReconAircraft = 1.05,
		BotParatroopers = 0.3,
		forceUnitCount = {
			min = 1,
			max = 2,
		},
		-- BotInfantry = {
		-- 	min = 50,
		-- 	max = 75,
		-- },
		-- BotATInfantry = {
		-- 	min = 2,
		-- 	max = 5,
		-- },
		-- BotInfantrySignaller = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotArtillery = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotMortars = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotEmplacements = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotTanks = {
		-- 	min = 2,
		-- 	max = 6,
		-- },
		-- BotHeavyTanks = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotSPGs = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotTankDestroyers = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotAircraft = {
		-- 	min = 4,
		-- 	max = 12,
		-- },
		-- BotReconAircraft = {
		-- 	min = 1,
		-- 	max = 4,
		-- },
		-- BotArmored = {
		-- 	min = 2,
		-- 	max = 5,
		-- },
	},
	{-- Index 4
		StrategyName = "Heavy Armor Strategy",
		StrategyUnitTypes = {
			"Tank",
			"Heavy",
		},
		forceUnitCount = {
			min = 1,
			max = 3,
		},
		BotInfantry = 1.15,
		BotTeamInfantry = 1.1,
		BotATInfantry = 1.12,
		BotInfantrySignaller = 0.95,
		BotArtillery = 0.95,
		BotMortars = 0.95,
		BotEmplacements = 0.9,
		BotTanks = 1.1,
		BotHeavyTanks = 1.2,
		BotArmored = 0.95,
		BotSPGs = 1.0,
		BotTankDestroyers = 0.95,
		BotAircraft = 0.9,
		BotReconAircraft = 0.95,
		BotParatroopers = 0.25,
		-- BotInfantry = {
		-- 	min = 30,
		-- 	max = 75,
		-- },
		-- BotATInfantry = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotInfantrySignaller = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotArtillery = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotMortars = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotEmplacements = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotTanks = {
		-- 	min = 6,
		-- 	max = 12,
		-- },
		-- BotHeavyTanks = {
		-- 	min = 3,
		-- 	max = 5,
		-- },
		-- BotSPGs = {
		-- 	min = 3,
		-- 	max = 5,
		-- },
		-- BotTankDestroyers = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotAircraft = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotReconAircraft = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotArmored = {
		-- 	min = 2,
		-- 	max = 5,
		-- },
	},
	{-- Index 5
		-- enableTankTactic = 1,
		StrategyName = "Armor Strategy",
		StrategyUnitTypes = {
			"Tank",
			"Ifv",
		},
		BotInfantry = 1.15,
		BotTeamInfantry = 1.1,
		BotATInfantry = 1.12,
		BotInfantrySignaller = 0.95,
		BotArtillery = 0.95,
		BotMortars = 0.95,
		BotEmplacements = 0.9,
		BotTanks = 1.15,
		BotHeavyTanks = 1.05,
		BotArmored = 1.05,
		BotSPGs = 0.95,
		BotTankDestroyers = 0.95,
		BotAircraft = 0.9,
		BotReconAircraft = 0.95,
		BotParatroopers = 0.25,
		forceUnitCount = {
			min = 1,
			max = 3,
		},
		-- BotInfantry = {
		-- 	min = 30,
		-- 	max = 60,
		-- },
		-- BotATInfantry = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotInfantrySignaller = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotArtillery = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotMortars = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotEmplacements = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotTanks = {
		-- 	min = 8,
		-- 	max = 14,
		-- },
		-- BotHeavyTanks = {
		-- 	min = 1,
		-- 	max = 3,
		-- },
		-- BotSPGs = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotTankDestroyers = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotAircraft = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotReconAircraft = {
		-- 	min = 2,
		-- 	max = 4,
		-- },
		-- BotArmored = {
		-- 	min = 1,
		-- 	max = 3,
		-- },
	},
	{-- Index 6
		StrategyName = "Infantry Strategy",
		StrategyUnitTypes = {
			"Infantry",
			"Team",
			"AT",
		},
		BotInfantry = 1.2,
		BotTeamInfantry = 1.12,
		BotATInfantry = 1.15,
		BotInfantrySignaller = 1.0,
		BotArtillery = 0.95,
		BotMortars = 1.0,
		BotEmplacements = 0.95,
		BotTanks = 0.95,
		BotHeavyTanks = 0.9,
		BotArmored = 1.0,
		BotSPGs = 0.95,
		BotTankDestroyers = 0.95,
		BotAircraft = 0.9,
		BotReconAircraft = 0.95,
		BotParatroopers = 0.25,
		forceUnitCount = {
			min = 2,
			max = 3,
		},
		-- BotInfantry = {
		-- 	min = 60,
		-- 	max = 120,
		-- },
		-- BotATInfantry = {
		-- 	min = 5,
		-- 	max = 10,
		-- },
		-- BotInfantrySignaller = {
		-- 	min = 2,
		-- 	max = 4,
		-- },
		-- BotArtillery = {
		-- 	min = 1,
		-- 	max = 3,
		-- },
		-- BotMortars = {
		-- 	min = 3,
		-- 	max = 4,
		-- },
		-- BotEmplacements = {
		-- 	min = 2,
		-- 	max = 3,
		-- },
		-- BotTanks = {
		-- 	min = 1,
		-- 	max = 3,
		-- },
		-- BotHeavyTanks = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotSPGs = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotTankDestroyers = {
		-- 	min = 2,
		-- 	max = 5,
		-- },
		-- BotAircraft = {
		-- 	min = 1,
		-- 	max = 1,
		-- },
		-- BotReconAircraft = {
		-- 	min = 1,
		-- 	max = 2,
		-- },
		-- BotArmored = {
		-- 	min = 2,
		-- 	max = 5,
		-- },
	},
}

-- Function to pick a random number from the set
local function PickRandomNumber(set)
	math.randomseed(os.time())
    -- Generate a random index
    local index = math.random(1, #set)
    -- Return the randomly selected number
    return set[index]
end

function SelectAiStrategyTemplate(botDefender) 
	local index = math.random(1, #StrategyTemplates)
	local strategy = StrategyTemplates[index]

	if neBattalionType then
		print("AI battalion = ", neBattalionType)
		if neBattalionType == "INF" then
			index = PickRandomNumber({1,3,6})
		elseif neBattalionType == "MOT" then
			index = PickRandomNumber({1,6})
		elseif neBattalionType == "MEC" then
			index = PickRandomNumber({6})
		elseif neBattalionType == "LT" then
			index = PickRandomNumber({5})
		elseif neBattalionType == "MT" then
			index = PickRandomNumber({5})
		elseif neBattalionType == "HT" then
			index = PickRandomNumber({4,5})
		elseif neBattalionType == "ART" then
			index = PickRandomNumber({2})
		end
		strategy = StrategyTemplates[index]
	end
	if testing and strategyIndexOverride then 
		index = strategyIndexOverride
		strategy = StrategyTemplates[index] 
	end
	print("Getting the AI strategy name...")
	if strategy.StrategyName then
		print("Loaded AI strategy ", strategy.StrategyName)
	else
		print("AI strategy has no name!")
	end
	print("Strategy index = ", index)
	BotApi.Scene:SetVar("ai_strategy_selection", index)
	return strategy
end
