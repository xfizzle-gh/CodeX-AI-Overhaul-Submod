-- Minimal 2022s diagnostic buy list.
-- Full squads and support detachments are classified separately.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			{priority = 1.0, type = {"Infantry", "Team", "Class1"}, unit = "squad_usmc_weapon_at(nato)"},
			{priority = 1.0, type = {"Infantry", "Team", "Class1"}, unit = "squad_usmc_mg(nato)"},
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "squad_usmc_eng(nato)"},
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "squad_inf2_rifle_m3(nato)"},
			{priority = 0.5, type = {"Infantry", "Team", "Class1"}, unit = "arf_medic(nato)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_squad_skirmish_nato(nato)"},
		}
	}
}

Purchases["2022s.nato"] = skirmish
Purchases["mid.nato"] = skirmish
