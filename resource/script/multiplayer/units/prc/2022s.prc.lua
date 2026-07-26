require([[/script/multiplayer/modes/battlezones_roles]])

-- Minimal 2022s diagnostic buy list.
-- Full squads and support detachments are classified separately.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			{priority = 1.00, type = {"Infantry", "Squad", "Class1"}, unit = "squad_pla112_rifle(prc)"},
			{priority = 0.70, type = {"Infantry", "Team", "MG", "Support", "Class1"}, unit = "squad_pla112_mg(prc)"},
			{priority = 0.70, type = {"Infantry", "Team", "AT", "Support", "Class1"}, unit = "squad_pla112_pf98(prc)"},
			{priority = 1.00, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "squad_pla112_rifle_dzj08(prc)"},
			{priority = 0.45, type = {"Infantry", "Team", "Recon", "Support", "Class1"}, unit = "squad_pla112_recon(prc)"},
			{priority = 0.45, type = {"Infantry", "Team", "Sniper", "Recon", "Support", "Class1"}, unit = "squad_pla112_sniper(prc)"},
			{priority = 0.75, type = {"Infantry", "Squad", "AT", "Class1"}, unit = "squad_pla112_rifle_hj12(prc)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_squad_skirmish_prc(prc)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_squad_skirmish_prc_139(prc)"},
		}
	}
}

Purchases["2022s.prc"] = skirmish
Purchases["mid.prc"] = skirmish
