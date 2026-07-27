require([[/script/multiplayer/modes/battlezones_roles]])

-- Curated 2022s Battle Zones buy list.
-- Base squads remain primary; doctrine assets become eligible only after the
-- matching National Defense or Western Mechanized doctrine is selected.
local skirmish = {
	{
		Repeat = 0,
		Units = {
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

			-- National Defense Forces doctrine assets.
			{priority = 0.10, type = {"Doctrine", "Infantry", "Squad", "Class1"}, unit = "93th_alcatraz_rifle_gp25(ukr)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "AT", "Team", "Class1"}, unit = "93th_at_stugna(ukr)"},
			{priority = 0.06, type = {"Doctrine", "Infantry", "AA", "Team", "Class1"}, unit = "ukr_manpad_operator(ukr)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "Recon", "Armored", "Class1"}, unit = "squad_ukr93_razv_novator(ukr)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "Ifv", "Class2"}, unit = "squad_ukr93_mech_bmp2(ukr)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Class2"}, unit = "squad_ukr93_t64bv(ukr)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Class3"}, unit = "squad_ukr93_t80bv(ukr)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Artillery", "Class2"}, unit = "squad_ukr93_2s1(ukr)"},
			{priority = 0.03, type = {"Doctrine", "Tank", "Artillery", "Class3"}, unit = "squad_ukr47_bm21(ukr)"},
			{priority = 0.03, type = {"Doctrine", "Air", "Transport", "Class2"}, unit = "mi17_ukr"},
			{priority = 0.02, type = {"Doctrine", "Air", "Sortie", "Class3"}, unit = "su-25sm_support_ukr"},

			-- Western Mechanized Corps doctrine assets.
			{priority = 0.10, type = {"Doctrine", "Infantry", "Squad", "AT", "Class1"}, unit = "47th_assault_at4(ukr)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "AT", "Team", "Class1"}, unit = "47th_inf_javelin(ukr)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "Recon", "Class1"}, unit = "47th_inf_razv_211(ukr)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "Ifv", "Class2"}, unit = "squad_ukr47_m2a2_arat_2022(ukr)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "Ifv", "Class2"}, unit = "squad_ukr93_mech_cv90(ukr)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Class3"}, unit = "squad_ukr47_m1a1_sa(ukr)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Class3"}, unit = "leopord_2a6"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Class3"}, unit = "challenger2"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Artillery", "Class2"}, unit = "squad_ukr47_m109(ukr)"},
			{priority = 0.03, type = {"Doctrine", "Tank", "Artillery", "Class3"}, unit = "m270_ukr"},
			{priority = 0.05, type = {"Doctrine", "Cannon", "Artillery", "Class2"}, unit = "squad_kraken_m777(ukr)"},
			{priority = 0.02, type = {"Doctrine", "Air", "Sortie", "Class3"}, unit = "su-25sm_at_ukr"},
		}
	}
}

Purchases["2022s.ukr"] = skirmish
Purchases["mid.ukr"] = skirmish
