-- Minimal 2022s diagnostic buy list.
-- Full squads and support detachments are classified separately.
local skirmish = {
	{
		Repeat = 0,
		Units = {
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "squad_pla112_rifle(prc)"},
			{priority = 1.0, type = {"Infantry", "Team", "Class1"}, unit = "squad_pla112_mg(prc)"},
			{priority = 1.0, type = {"Infantry", "Team", "Class1"}, unit = "squad_pla112_pf98(prc)"},
			{priority = 1.0, type = {"Infantry", "Squad", "Class1"}, unit = "squad_pla112_rifle_dzj08(prc)"},
			{priority = 0.5, type = {"Infantry", "Team", "Class1"}, unit = "squad_pla112_recon(prc)"},
			{priority = 0.01, type = {"Doctrine", "Squad", "Class1"}, unit = "doctrine_squad_skirmish_prc(prc)"},
		}
	}
}

Purchases["2022s.prc"] = skirmish
Purchases["mid.prc"] = skirmish
