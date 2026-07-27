require([[/script/multiplayer/modes/battlezones_roles]])

-- Curated 2022s Battle Zones buy list.
-- Base squads remain primary; doctrine assets become eligible only after the
-- matching Ground Forces or Airborne/Naval doctrine is selected.
local skirmish = {
	{
		Repeat = 0,
		Units = {
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

			-- Ground Forces doctrine assets.
			{priority = 0.10, type = {"Doctrine", "Infantry", "Squad", "AT", "Class1"}, unit = "rus4_inf_rifle_rpg27(rusa)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "Engineer", "AT", "Class1"}, unit = "rus90_saperi_at(rusa)"},
			{priority = 0.06, type = {"Doctrine", "Infantry", "AA", "Team", "Class1"}, unit = "rus_manpad_operator(rusa)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "Recon", "Ifv", "Class1"}, unit = "squad_rus90_razv_bmp2(rusa)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "Ifv", "Class2"}, unit = "squad_rus90_bmp2m(rusa)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Class3"}, unit = "t90m_2024_ubh"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Support", "Class3"}, unit = "bmpt_72"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Artillery", "Class2"}, unit = "squad_rus90_2s3(rusa)"},
			{priority = 0.03, type = {"Doctrine", "Tank", "Artillery", "Class3"}, unit = "squad_rus90_tos1(rusa)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "AA", "Class2"}, unit = "2s6"},
			{priority = 0.02, type = {"Doctrine", "Air", "Sortie", "Class3"}, unit = "su-25sm_at"},

			-- Airborne and Naval Infantry doctrine assets.
			{priority = 0.10, type = {"Doctrine", "Infantry", "Squad", "AT", "Class1"}, unit = "vdv76_inf_rpg28(rusa)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "AT", "Team", "Class1"}, unit = "vdv76_inf_metis(rusa)"},
			{priority = 0.10, type = {"Doctrine", "Infantry", "Squad", "AT", "Class1"}, unit = "rus155_inf_rpg28(rusa)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "Recon", "Armored", "Class1"}, unit = "squad_vdv45_razv_typhoon(rusa)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "Ifv", "Class2"}, unit = "squad_vdv76_mech_bmd4(rusa)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "Ifv", "Class2"}, unit = "squad_rus155_mech_bmp3(rusa)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Light", "Class2"}, unit = "squad_vdv76_2s25(rusa)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Class3"}, unit = "squad_rus155_t80bv_2025(rusa)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Artillery", "Class2"}, unit = "squad_vdv76_2s9(rusa)"},
			{priority = 0.03, type = {"Doctrine", "Tank", "Artillery", "Class3"}, unit = "squad_rus155_bm21(rusa)"},
			{priority = 0.03, type = {"Doctrine", "Air", "Transport", "Class2"}, unit = "mi17_rus"},
			{priority = 0.02, type = {"Doctrine", "Air", "Class3"}, unit = "squad_rus155_ka52k(rusa)"},
		}
	}
}

Purchases["2022s.rusa"] = skirmish
Purchases["mid.rusa"] = skirmish
