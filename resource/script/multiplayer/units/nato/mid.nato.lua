-- Lobby unitMode 2022s; reuse Code:X conquest purchase trees.
require([[/script/multiplayer/units/nato/conquest.nato]])
if Purchases and Purchases["conquest.nato"] then
	Purchases["2022s.nato"] = Purchases["conquest.nato"]
	Purchases["mid.nato"] = Purchases["conquest.nato"]
end
