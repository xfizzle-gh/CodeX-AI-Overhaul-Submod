-- Curated 2022s skirmish buy list.
-- Rebuild the original Code:X catalog in isolated, testable batches.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "squad_usmc_eng(nato)"},
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "squad_inf2_rifle_m3(nato)"},
			{priority = 0.8, type = {"Infantry", "Team", "Class1"}, unit = "squad_usmc_weapon_at(nato)"},
			{priority = 0.8, type = {"Infantry", "Team", "Class1"}, unit = "squad_usmc_mg(nato)"},
			{priority = 0.75, type = {"Infantry", "Team", "Class1"}, unit = "arf_medic(nato)"},
			{priority = 0.8, type = {"Infantry", "Team", "Class1"}, unit = "squad_usmc_weapon_at_javelin(nato)"},
			{priority = 0.005, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine(nato)"},
			{priority = 0.005, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_vehicle_m2a3(nato)"},
		}
	}
}

Purchases["2022s.nato"] = skirmish
Purchases["mid.nato"] = skirmish
