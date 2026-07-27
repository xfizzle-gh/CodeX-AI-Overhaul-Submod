require([[/script/multiplayer/modes/battlezones_roles]])

-- Curated 2022s Battle Zones buy list.
-- Base squads remain primary. Doctrine choices are ground-only and become
-- eligible only after the matching Ukrainian doctrine is selected.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			-- Shared Ukrainian MP backbone.
			{priority = 0.90, type = {"Infantry", "Squad", "Class1"}, unit = "ter_22_1(ukr)"},
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "93th_alcatraz_rifle(ukr)"},
			{priority = 0.70, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "93th_alcatraz_mg_pkm(ukr)"},
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "47th_inf_rifle(ukr)"},
			{priority = 0.40, type = {"Infantry", "Team", "Medic", "Support", "Class1"}, unit = "ukr_22_5(ukr)"},
			{priority = 0.55, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "47th_inf_at(ukr)"},
			{priority = 0.75, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "47th_assault_nlaw(ukr)"},

			-- Doctrine selection cards.
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine(ukr)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_squad_47th(ukr)"},

			-- National Defense Forces: five ground-only DP choices.
			{priority = 0.08, type = {"Doctrine", "Infantry", "Squad", "Class1"}, unit = "doctrine_squad_93th(ukr)"},
			{priority = 0.07, type = {"Doctrine", "Armored", "Recon", "Class1"}, unit = "doctrine_squad_93th_novator(ukr)"},
			{priority = 0.06, type = {"Doctrine", "Ifv", "Class2"}, unit = "doctrine_vehicle_bmp2_ukr93(ukr)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Class2"}, unit = "doctrine_vehicle_t64bv_ukr93(ukr)"},
			{priority = 0.03, type = {"Doctrine", "Tank", "Artillery", "Class2"}, unit = "doctrine_vehicle_2s1_ukr93(ukr)"},

			-- Western Mechanized Corps: five ground-only DP choices.
			{priority = 0.08, type = {"Doctrine", "Infantry", "Squad", "AT", "Class1"}, unit = "doctrine_squad_47th(ukr)"},
			{priority = 0.07, type = {"Doctrine", "Infantry", "Recon", "Class1"}, unit = "doctrine_squad_211th_recon(ukr)"},
			{priority = 0.06, type = {"Doctrine", "Ifv", "Class2"}, unit = "doctrine_vehicle_Bradley_47th(ukr)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Class3"}, unit = "doctrine_vehicle_m1a1sa(ukr)"},
			{priority = 0.03, type = {"Doctrine", "Tank", "Artillery", "Class2"}, unit = "doctrine_vehicle_m109a6_47(ukr)"},
		}
	}
}

Purchases["2022s.ukr"] = skirmish
Purchases["mid.ukr"] = skirmish
