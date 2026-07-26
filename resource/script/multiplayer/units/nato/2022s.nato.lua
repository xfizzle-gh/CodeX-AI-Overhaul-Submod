-- Minimal 2022s diagnostic buy list.
-- Do not import the full Code:X conquest table while isolating the spawn crash.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "squad_usmc_weapon_at(nato)"},
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "squad_usmc_mg(nato)"},
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "squad_usmc_eng(nato)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_squad_skirmish_nato(nato)"},
		}
	}
}

Purchases["2022s.nato"] = skirmish
Purchases["mid.nato"] = skirmish
