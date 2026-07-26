-- Lobby unitMode 2022s. Reuse Code:X conquest purchases without mutating them.
require([[/script/multiplayer/units/rusa/conquest.rusa]])

local function clonePurchaseTable(source)
	local result = {}
	for i, purchase in ipairs(source) do
		local copy = {}
		for key, value in pairs(purchase) do
			copy[key] = value
		end
		local units = {}
		for j, unit in ipairs(purchase.Units or {}) do
			units[j] = unit
		end
		copy.Units = units
		result[i] = copy
	end
	return result
end

if Purchases and Purchases["conquest.rusa"] then
	local skirmish = clonePurchaseTable(Purchases["conquest.rusa"])
	table.insert(skirmish[1].Units, {
		priority = 1.0,
		type = {"Infantry", "Doctrine", "Squad", "Class1"},
		unit = "doctrine_squad_skirmish_rusa(rusa)"
	})
	Purchases["2022s.rusa"] = skirmish
	Purchases["mid.rusa"] = skirmish
end
