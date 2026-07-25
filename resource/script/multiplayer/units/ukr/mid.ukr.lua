-- Lobby unitMode 2022s; reuse Code:X conquest purchase trees.
require([[/script/multiplayer/units/ukr/conquest.ukr]])
if Purchases and Purchases["conquest.ukr"] then
	Purchases["2022s.ukr"] = Purchases["conquest.ukr"]
	Purchases["mid.ukr"] = Purchases["conquest.ukr"]
end
