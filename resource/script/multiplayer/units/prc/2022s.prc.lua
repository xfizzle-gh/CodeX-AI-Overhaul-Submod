require([[/script/multiplayer/modes/battlezones_roles]])

-- Curated 2022s Battle Zones buy list.
-- Shared infantry remains primary. Doctrine-specific ground units and five
-- doctrine-point choices become eligible after the matching doctrine is selected.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			-- Shared PRC MP backbone.
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "squad_pla112_rifle(prc)"},
			{priority = 0.70, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "squad_pla112_mg(prc)"},
			{priority = 0.70, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "squad_pla112_pf98(prc)"},
			{priority = 1.00, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "squad_pla112_rifle_dzj08(prc)"},
			{priority = 0.45, type = {"Infantry", "Team", "Recon", "Support", "Class1"}, unit = "squad_pla112_recon(prc)"},
			{priority = 0.45, type = {"Infantry", "Team", "Sniper", "Recon", "Support", "Class1"}, unit = "squad_pla112_sniper(prc)"},
			{priority = 0.75, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "squad_pla112_rifle_hj12(prc)"},

			-- Doctrine selection cards.
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_squad_skirmish_prc(prc)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_squad_skirmish_prc_139(prc)"},

			-- 112th Rapid Combined Arms normal-MP ground roster.
			{priority = 0.60, type = {"Infantry", "Team", "Marksman", "Class1"}, unit = "squad_pla112_qbu11(prc)"},
			{priority = 0.55, type = {"Infantry", "Engineer", "AT", "Class1"}, unit = "squad_pla112_eng_at(prc)"},
			{priority = 0.18, type = {"Armored", "AT", "Class2"}, unit = "squad_pla112_zbl08_hj73(prc)"},
			{priority = 0.08, type = {"Tank", "Artillery", "Support", "Class2"}, unit = "squad_pla112_plz05(prc)"},

			-- 112th Rapid Combined Arms: five ground-only DP choices.
			{priority = 0.07, type = {"Doctrine", "Ifv", "AT", "Class2"}, unit = "squad_pla112_04a_hj12(prc)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Class2"}, unit = "squad_pla112_ztz96a(prc)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Class3"}, unit = "squad_pla112_ztz99a(prc)"},
			{priority = 0.06, type = {"Doctrine", "Armored", "AT", "Class2"}, unit = "squad_pla112_aft10(prc)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "AA", "Class2"}, unit = "pgz_09"},

			-- 139th Heavy Combined Arms normal-MP ground roster.
			{priority = 0.85, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "squad_pla139_rifle_hj12(prc)"},
			{priority = 0.70, type = {"Infantry", "Squad", "Support", "Class1"}, unit = "squad_pla139_weapon(prc)"},
			{priority = 0.55, type = {"Infantry", "Engineer", "AT", "Class1"}, unit = "squad_pla139_eng_at(prc)"},
			{priority = 0.18, type = {"Ifv", "AT", "Class2"}, unit = "squad_pla139_86a_hj12(prc)"},
			{priority = 0.08, type = {"Tank", "Artillery", "Support", "Class2"}, unit = "squad_pla139_plz07(prc)"},

			-- 139th Heavy Combined Arms: five ground-only DP choices.
			{priority = 0.07, type = {"Doctrine", "Ifv", "Class2"}, unit = "squad_pla139_04a(prc)"},
			{priority = 0.05, type = {"Doctrine", "Tank", "Class2"}, unit = "squad_pla139_ztz96ap(prc)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "Class3"}, unit = "wz1001"},
			{priority = 0.06, type = {"Doctrine", "Armored", "AT", "Class2"}, unit = "squad_pla139_aft09(prc)"},
			{priority = 0.04, type = {"Doctrine", "Tank", "AA", "Class2"}, unit = "hq_7b"},
		}
	}
}

Purchases["2022s.prc"] = skirmish
Purchases["mid.prc"] = skirmish
