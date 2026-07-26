require([[/script/multiplayer/modes/battlezones_roles]])

-- Minimal 2022s diagnostic buy list.
-- Full squads and support detachments are classified separately.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "rus90_inf_rifle(rusa)"},
			{priority = 0.70, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "rus90_inf_mg(rusa)"},
			{priority = 0.70, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "rus90_inf_at(rusa)"},
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "rus90_inf_assault(rusa)"},
			{priority = 0.40, type = {"Infantry", "Team", "Medic", "Support", "Class1"}, unit = "rus_22_5(rusa)"},
			{priority = 0.45, type = {"Infantry", "Team", "Sniper", "Recon", "Support", "Class1"}, unit = "rus90_inf_sniper(rusa)"},
			{priority = 0.75, type = {"Infantry", "Squad", "Class1"}, unit = "lud_22_1(rusa)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine(rusa)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_squad_dsh(rusa)"},
		}
	}
}

Purchases["2022s.rusa"] = skirmish
Purchases["mid.rusa"] = skirmish
