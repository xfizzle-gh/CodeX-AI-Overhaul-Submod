require([[/script/multiplayer/modes/battlezones_roles]])

-- Minimal 2022s diagnostic buy list.
-- Full squads and support detachments are classified separately.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			{priority = 0.90, type = {"Infantry", "Squad", "Class1"}, unit = "ter_22_1(ukr)"},
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "93th_alcatraz_rifle(ukr)"},
			{priority = 0.70, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "93th_alcatraz_mg_pkm(ukr)"},
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "47th_inf_rifle(ukr)"},
			{priority = 0.40, type = {"Infantry", "Team", "Medic", "Support", "Class1"}, unit = "ukr_22_5(ukr)"},
			{priority = 0.55, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "47th_inf_at(ukr)"},
			{priority = 0.75, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "47th_assault_nlaw(ukr)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine(ukr)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_squad_47th(ukr)"},
		}
	}
}

Purchases["2022s.ukr"] = skirmish
Purchases["mid.ukr"] = skirmish
