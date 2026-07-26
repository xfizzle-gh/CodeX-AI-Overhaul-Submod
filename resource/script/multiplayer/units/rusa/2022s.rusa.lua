-- Minimal 2022s diagnostic buy list.
-- Do not import the full Code:X conquest table while isolating the spawn crash.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "rus90_inf_rifle(rusa)"},
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "rus90_inf_mg(rusa)"},
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "rus90_inf_at(rusa)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_squad_skirmish_rusa(rusa)"},
		}
	}
}

Purchases["2022s.rusa"] = skirmish
Purchases["mid.rusa"] = skirmish
