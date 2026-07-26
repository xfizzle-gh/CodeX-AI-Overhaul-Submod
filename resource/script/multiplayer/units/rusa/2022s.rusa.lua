-- Lobby unitMode 2022s; reuse Code:X conquest purchase trees.
require([[/script/multiplayer/units/rusa/conquest.rusa]])
if Purchases and Purchases["conquest.rusa"] then
	Purchases["2022s.rusa"] = Purchases["conquest.rusa"]
	Purchases["mid.rusa"] = Purchases["conquest.rusa"]
end
