require([[/script/multiplayer/modes/battlezones_roles]])

-- Minimal 2022s diagnostic buy list.
-- Full squads and support detachments are classified separately.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			{priority = 0.70, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "squad_usmc_weapon_at(nato)"},
			{priority = 0.70, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "squad_usmc_mg(nato)"},
			{priority = 0.90, type = {"Infantry", "Squad", "Engineer", "Class1"}, unit = "squad_usmc_eng(nato)"},
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "squad_inf2_rifle_m3(nato)"},
			{priority = 0.40, type = {"Infantry", "Team", "Medic", "Support", "Class1"}, unit = "arf_medic(nato)"},
			{priority = 0.55, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "squad_usmc_weapon_at_javelin(nato)"},
			{priority = 0.75, type = {"Infantry", "Squad", "Class1"}, unit = "squad_inf2_rifle(nato)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine(nato)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_vehicle_m2a3(nato)"},
		}
	}
}

Purchases["2022s.nato"] = skirmish
Purchases["mid.nato"] = skirmish
