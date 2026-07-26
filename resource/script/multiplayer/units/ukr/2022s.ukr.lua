-- Minimal 2022s diagnostic buy list.
-- Full squads and support detachments are classified separately.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "ter_22_1(ukr)"},
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "93th_alcatraz_rifle(ukr)"},
			{priority = 1.0, type = {"Infantry", "Team", "Class1"}, unit = "93th_alcatraz_mg_pkm(ukr)"},
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "47th_inf_rifle(ukr)"},
			{priority = 0.5, type = {"Infantry", "Team", "Class1"}, unit = "ukr_22_5(ukr)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_squad_skirmish_ukr(ukr)"},
		}
	}
}

Purchases["2022s.ukr"] = skirmish
Purchases["mid.ukr"] = skirmish
