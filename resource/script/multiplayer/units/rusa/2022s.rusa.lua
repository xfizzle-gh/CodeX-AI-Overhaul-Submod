-- Curated 2022s skirmish buy list.
-- Rebuild the original Code:X catalog in isolated, testable batches.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "rus90_inf_rifle(rusa)"},
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "rus90_inf_mg(rusa)"},
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "rus90_inf_at(rusa)"},
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "rus90_inf_assault(rusa)"},
			{priority = 0.75, type = {"Infantry", "Team", "Class1"}, unit = "rus_22_5(rusa)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_squad_skirmish_rusa(rusa)"},
		}
	}
}

Purchases["2022s.rusa"] = skirmish
Purchases["mid.rusa"] = skirmish
