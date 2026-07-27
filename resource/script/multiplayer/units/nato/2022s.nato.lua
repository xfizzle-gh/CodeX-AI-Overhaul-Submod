require([[/script/multiplayer/modes/battlezones_roles]])

-- NATO Battle Zones buy list.
-- Both doctrines share inexpensive NATO line infantry and detachments, then
-- unlock their own national squads, vehicles, armor, support and five DP choices.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			-- Shared NATO MP backbone.
			{priority = 1.15, type = {"Infantry", "Squad", "Class1"}, unit = "squad_arf_rifle(nato)"},
			{priority = 0.70, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "squad_arf_rifle_spike(nato)"},
			{priority = 0.65, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "squad_arf_mg(nato)"},
			{priority = 0.65, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "squad_arf_at(nato)"},
			{priority = 0.40, type = {"Infantry", "Team", "Recon", "Support", "Class1"}, unit = "squad_arf_scout(nato)"},
			{priority = 0.35, type = {"Infantry", "Team", "Medic", "Support", "Class1"}, unit = "arf_medic(nato)"},

			-- Doctrine selection cards.
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine(nato)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_vehicle_m2a3(nato)"},

			-- United States: infantry squads and detachments.
			{priority = 0.75, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "squad_usmc_weapon_at(nato)"},
			{priority = 0.70, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "squad_usmc_mg(nato)"},
			{priority = 0.75, type = {"Infantry", "Team", "Engineer", "Class1"}, unit = "squad_usmc_eng(nato)"},
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "squad_inf2_rifle_m3(nato)"},
			{priority = 0.60, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "squad_usmc_weapon_at_javelin(nato)"},
			{priority = 0.80, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "squad_inf2_rifle(nato)"},
			{priority = 0.90, type = {"Infantry", "Squad", "Class1"}, unit = "squad_usmc_rifle(nato)"},
			{priority = 0.65, type = {"Infantry", "Squad", "Class2"}, unit = "squad_inf2_rifle_ngsw(nato)"},
			{priority = 0.55, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "squad_tank1_rifle_mg(nato)"},
			{priority = 0.35, type = {"Infantry", "Team", "Recon", "AT", "Support", "Class1"}, unit = "squad_82nd_lrs(nato)"},

			-- United States: recon and mechanized.
			{priority = 0.45, type = {"Armored", "Recon", "Infantry", "Class1"}, unit = "squad_usmc_lav25(nato)"},
			{priority = 0.50, type = {"Armored", "Infantry", "Class1"}, unit = "squad_inf2_m1126_mk19(nato)"},
			{priority = 0.40, type = {"Ifv", "Infantry", "Class2"}, unit = "squad_inf2_m1296(nato)"},
			{priority = 0.35, type = {"Ifv", "Infantry", "Class2"}, unit = "squad_tank1_m2a3_fireteam_brat(nato)"},

			-- United States: heavy armor.
			{priority = 0.30, type = {"Tank", "Light", "Class2"}, unit = "squad_inf2_m1128(nato)"},
			{priority = 0.28, type = {"Tank", "Light", "Class2"}, unit = "squad_efp_m10(nato)"},
			{priority = 0.22, type = {"Tank", "Class3"}, unit = "squad_tank1_m1a2_sep(nato)"},

			-- United States: support and call-ins.
			{priority = 0.22, type = {"Cannon", "Artillery", "Support", "Class2"}, unit = "squad_usmc_m777(nato)"},
			{priority = 0.18, type = {"Tank", "Artillery", "Support", "Class2"}, unit = "squad_tank1_m109(nato)"},
			{priority = 0.12, type = {"Tank", "Artillery", "Support", "Class3"}, unit = "squad_usmc_m142(nato)"},
			{priority = 0.06, type = {"Air", "Sortie", "Support", "Class3"}, unit = "a-10c_support"},

			-- United States: five doctrine-point choices.
			{priority = 0.08, type = {"Doctrine", "Infantry", "Squad", "Recon", "AT", "Class1"}, unit = "squad_gb3_scouts_ranger(nato)"},
			{priority = 0.06, type = {"Doctrine", "Armored", "AA", "Support", "Class2"}, unit = "squad_tank1_ampv_cuas(nato)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Class3"}, unit = "squad_tank1_m1a2_sepv_armor(nato)"},
			{priority = 0.03, type = {"Doctrine", "Tank", "Artillery", "Class3"}, unit = "squad_tank1_m270(nato)"},
			{priority = 0.02, type = {"Doctrine", "Air", "Class3"}, unit = "ah1z"},

			-- Europe: infantry squads and detachments.
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "squad_pz10_rifle(nato)"},
			{priority = 0.95, type = {"Infantry", "Squad", "Class1"}, unit = "squad_gb3_rifle(nato)"},
			{priority = 0.80, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "squad_gb3_rifle_nlaw(nato)"},
			{priority = 0.55, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "squad_pz10_fireteam_mg(nato)"},
			{priority = 0.55, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "squad_pz10_fireteam_carl(nato)"},
			{priority = 0.65, type = {"Infantry", "Team", "Engineer", "Class1"}, unit = "squad_pz10_eng(nato)"},
			{priority = 0.35, type = {"Infantry", "Team", "Recon", "AT", "Support", "Class1"}, unit = "squad_pz10_rec(nato)"},
			{priority = 0.55, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "squad_gb3_rifle_mg(nato)"},
			{priority = 0.55, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "squad_gb3_rifle_at_javelin(nato)"},

			-- Europe: recon and mechanized.
			{priority = 0.45, type = {"Armored", "Recon", "Class1"}, unit = "squad_pz10_fennek(nato)"},
			{priority = 0.45, type = {"Ifv", "Infantry", "Class1"}, unit = "squad_pz10_ypr765_rifle(nato)"},
			{priority = 0.40, type = {"Ifv", "Infantry", "Class2"}, unit = "squad_pz10_cv9030_rifle(nato)"},
			{priority = 0.42, type = {"Armored", "Infantry", "Class2"}, unit = "squad_gb3_mot_rifle_boxer(nato)"},
			{priority = 0.36, type = {"Ifv", "Infantry", "Class2"}, unit = "squad_gb3_mech_rifle_fv510_milan(nato)"},

			-- Europe: heavy armor.
			{priority = 0.25, type = {"Tank", "Class3"}, unit = "squad_pz10_leopard2a6(nato)"},
			{priority = 0.25, type = {"Tank", "Class3"}, unit = "squad_gb3_tank_challenger(nato)"},
			{priority = 0.24, type = {"Tank", "Class3"}, unit = "leopard_2_pl"},
			{priority = 0.24, type = {"Tank", "Class3"}, unit = "leclerc_sxxi"},

			-- Europe: support and call-ins.
			{priority = 0.22, type = {"Cannon", "Artillery", "Support", "Class1"}, unit = "squad_gb3_art_l118(nato)"},
			{priority = 0.18, type = {"Tank", "Artillery", "Support", "Class2"}, unit = "squad_gb3_art_as90(nato)"},
			{priority = 0.12, type = {"Tank", "Artillery", "Support", "Class3"}, unit = "squad_gb3_art_m270(nato)"},
			{priority = 0.16, type = {"Tank", "AA", "Support", "Class2"}, unit = "otomatic"},
			{priority = 0.06, type = {"Air", "Sortie", "Support", "Class3"}, unit = "tornado_gr4_support_light"},

			-- Europe: five doctrine-point choices.
			{priority = 0.08, type = {"Doctrine", "Infantry", "Team", "Recon", "AT", "Class1"}, unit = "squad_gb3_scouts_nlaw(nato)"},
			{priority = 0.06, type = {"Doctrine", "Ifv", "Infantry", "Class2"}, unit = "squad_pz10_puma_mells_rifle(nato)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Class3"}, unit = "squad_pz10_leopard2a7(nato)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Artillery", "Class2"}, unit = "squad_pz10_pzh2000(nato)"},
			{priority = 0.02, type = {"Doctrine", "Air", "Class3"}, unit = "tiger_heavy"},
		}
	}
}

Purchases["2022s.nato"] = skirmish
Purchases["mid.nato"] = skirmish
