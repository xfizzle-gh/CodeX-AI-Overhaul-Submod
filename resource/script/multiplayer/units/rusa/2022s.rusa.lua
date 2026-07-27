require([[/script/multiplayer/modes/battlezones_roles]])

-- Curated 2022s Battle Zones buy list.
-- Shared infantry remains primary. Doctrine-specific ground units and five
-- doctrine-point choices become eligible after the matching doctrine is selected.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			-- Shared Russian MP backbone.
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "rus90_inf_rifle(rusa)"},
			{priority = 0.70, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "rus90_inf_mg(rusa)"},
			{priority = 0.70, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "rus90_inf_at(rusa)"},
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "rus90_inf_assault(rusa)"},
			{priority = 0.40, type = {"Infantry", "Team", "Medic", "Support", "Class1"}, unit = "rus_22_5(rusa)"},
			{priority = 0.45, type = {"Infantry", "Team", "Sniper", "Recon", "Support", "Class1"}, unit = "rus90_inf_sniper(rusa)"},
			{priority = 0.75, type = {"Infantry", "Squad", "Class1"}, unit = "lud_22_1(rusa)"},

			-- Doctrine selection cards.
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine(rusa)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_squad_dsh(rusa)"},

			-- Ground Forces normal-MP ground roster.
			{priority = 0.85, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "rus4_inf_rifle_rpg27(rusa)"},
			{priority = 0.55, type = {"Infantry", "Engineer", "AT", "Class1"}, unit = "rus90_saperi_at(rusa)"},
			{priority = 0.18, type = {"Ifv", "Class2"}, unit = "squad_rus90_bmp2m(rusa)"},
			{priority = 0.08, type = {"Tank", "Artillery", "Support", "Class2"}, unit = "squad_rus90_2s3(rusa)"},

			-- Ground Forces: five ground-only DP choices.
			{priority = 0.08, type = {"Doctrine", "Infantry", "AA", "Team", "Class1"}, unit = "rus_manpad_operator(rusa)"},
			{priority = 0.07, type = {"Doctrine", "Armored", "Recon", "Class1"}, unit = "squad_rus90_razv_bmp2(rusa)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Support", "Class2"}, unit = "bmpt_72"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Class3"}, unit = "t90m_2024_ubh"},
			{priority = 0.04, type = {"Doctrine", "Tank", "AA", "Class2"}, unit = "2s6"},

			-- Airborne and Naval Infantry normal-MP ground roster.
			{priority = 0.85, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "vdv76_inf_rpg28(rusa)"},
			{priority = 0.60, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "vdv76_inf_metis(rusa)"},
			{priority = 0.18, type = {"Ifv", "Class2"}, unit = "squad_vdv76_mech_bmd4(rusa)"},
			{priority = 0.08, type = {"Tank", "Artillery", "Support", "Class2"}, unit = "squad_vdv76_2s9(rusa)"},

			-- Airborne and Naval Infantry: five ground-only DP choices.
			{priority = 0.08, type = {"Doctrine", "Infantry", "Squad", "AT", "Class1"}, unit = "rus155_inf_rpg28(rusa)"},
			{priority = 0.07, type = {"Doctrine", "Armored", "Recon", "Class1"}, unit = "squad_vdv45_razv_typhoon(rusa)"},
			{priority = 0.06, type = {"Doctrine", "Ifv", "Class2"}, unit = "squad_rus155_mech_bmp3(rusa)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Light", "Class2"}, unit = "squad_vdv76_2s25(rusa)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Class3"}, unit = "squad_rus155_t80bv_2025(rusa)"},
		}
	}
}

Purchases["2022s.rusa"] = skirmish
Purchases["mid.rusa"] = skirmish
