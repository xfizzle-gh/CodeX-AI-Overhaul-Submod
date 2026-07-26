-- Lobby unitMode 2022s; reuse Code:X conquest purchase trees.
require([[/script/multiplayer/units/prc/conquest.prc]])
if Purchases and Purchases["conquest.prc"] then
	Purchases["2022s.prc"] = Purchases["conquest.prc"]
	Purchases["mid.prc"] = Purchases["conquest.prc"]
end
