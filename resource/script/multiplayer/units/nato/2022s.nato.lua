require([[/script/multiplayer/modes/battlezones_roles]])

-- Curated 2022s Battle Zones buy list.
-- Both NATO doctrines share an inexpensive line-infantry backbone. National
-- infantry and doctrine-point combined arms remain doctrine-specific.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			-- Shared NATO MP backbone available under either doctrine.
			{priority = 1.15, type = {"Infantry", "Squad", "Class1"}, unit = "squad_arf_rifle(nato)"},
			{priority = 0.70, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "squad_arf_rifle_spike(nato)"},
			{priority = 0.65, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "squad_arf_mg(nato)"},
			{priority = 0.65, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "squad_arf_at(nato)"},
			{priority = 0.40, type = {"Infantry", "Team", "Recon", "Support", "Class1"}, unit = "squad_arf_scout(nato)"},
			{priority = 0.35, type = {"Infantry", "Team", "Medic", "Support", "Class1"}, unit = "arf_medic(nato)"},

			-- United States Joint Force base MP roster.
			{priority = 0.70, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "squad_usmc_weapon_at(nato)"},
			{priority = 0.70, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "squad_usmc_mg(nato)"},
			{priority = 0.90, type = {"Infantry", "Squad", "Engineer", "Class1"}, unit = "squad_usmc_eng(nato)"},
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "squad_inf2_rifle_m3(nato)"},
			{priority = 0.55, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "squad_usmc_weapon_at_javelin(nato)"},
			{priority = 0.75, type = {"Infantry", "Squad", "Class1"}, unit = "squad_inf2_rifle(nato)"},

			-- Doctrine selection cards.
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine(nato)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_vehicle_m2a3(nato)"},

			-- United States Joint Force doctrine-gated MP infantry and detachments.
			{priority = 0.90, type = {"Infantry", "Squad", "Class1"}, unit = "squad_usmc_rifle(nato)"},
			{priority = 0.80, type = {"Infantry", "Squad", "Class1"}, unit = "squad_inf2_rifle_ngsw(nato)"},
			{priority = 0.55, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "squad_tank1_rifle_mg(nato)"},
			{priority = 0.45, type = {"Infantry", "Team", "Recon", "AT", "Support", "Class1"}, unit = "squad_82nd_lrs(nato)"},
			{priority = 0.35, type = {"Infantry", "Squad", "Recon", "AT", "Class2"}, unit = "squad_gb3_scouts_ranger(nato)"},

			-- United States Joint Force doctrine-point combined arms.
			{priority = 0.08, type = {"Doctrine", "Infantry", "Armored", "Class1"}, unit = "squad_inf2_m1126_mk19(nato)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "Ifv", "Class2"}, unit = "squad_tank1_m2a3_fireteam_brat(nato)"},
			{priority = 0.06, type = {"Doctrine", "Infantry", "AA", "Support", "Class2"}, unit = "squad_tank1_ampv_cuas(nato)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Class3"}, unit = "squad_tank1_m1a2_sepv_armor(nato)"},
			{priority = 0.05, type = {"Doctrine", "Cannon", "Artillery", "Class2"}, unit = "squad_usmc_m777(nato)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Artillery", "Class2"}, unit = "squad_tank1_m109(nato)"},
			{priority = 0.03, type = {"Doctrine", "Tank", "Artillery", "Class3"}, unit = "squad_tank1_m270(nato)"},
			{priority = 0.02, type = {"Doctrine", "Air", "Class3"}, unit = "ah1z"},

			-- European Coalition doctrine-gated MP infantry and detachments.
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "squad_pz10_rifle(nato)"},
			{priority = 0.90, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "squad_gb3_rifle_nlaw(nato)"},
			{priority = 0.55, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "squad_pz10_fireteam_mg(nato)"},
			{priority = 0.50, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "squad_pz10_fireteam_carl(nato)"},
			{priority = 0.65, type = {"Infantry", "Team", "Engineer", "Class1"}, unit = "squad_pz10_eng(nato)"},
			{priority = 0.35, type = {"Infantry", "Team", "Recon", "AT", "Support", "Class1"}, unit = "squad_pz10_rec(nato)"},
			{priority = 0.55, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "squad_gb3_rifle_mg(nato)"},
			{priority = 0.50, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "squad_gb3_rifle_at_javelin(nato)"},

			-- European Coalition doctrine-point combined arms.
			{priority = 0.08, type = {"Doctrine", "Infantry", "Ifv", "Class2"}, unit = "squad_pz10_puma_mells_rifle(nato)"},
			{priority = 0.08, type = {"Doctrine", "Infantry", "Ifv", "Class2"}, unit = "squad_gb3_mech_rifle_fv510_milan(nato)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Class3"}, unit = "squad_pz10_leopard2a7(nato)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Class3"}, unit = "squad_gb3_tank_challenger_tes(nato)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Class3"}, unit = "leopard_2_pl"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Class3"}, unit = "leclerc_sxxi"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Artillery", "Class2"}, unit = "squad_pz10_pzh2000(nato)"},
			{priority = 0.03, type = {"Doctrine", "Tank", "Artillery", "Class3"}, unit = "squad_gb3_art_m270(nato)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "AA", "Class2"}, unit = "otomatic"},
			{priority = 0.02, type = {"Doctrine", "Air", "Class3"}, unit = "tiger_heavy"},
		}
	}
}

Purchases["2022s.nato"] = skirmish
Purchases["mid.nato"] = skirmish
