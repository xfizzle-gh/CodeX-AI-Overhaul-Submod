-- Lobby unitMode 2022s. Reuse Code:X conquest purchases without mutating them.
require([[/script/multiplayer/units/nato/conquest.nato]])

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

if Purchases and Purchases["conquest.nato"] then
	local skirmish = clonePurchaseTable(Purchases["conquest.nato"])
	table.insert(skirmish[1].Units, {
		priority = 1.0,
		type = {"Infantry", "Doctrine", "Squad", "Class1"},
		unit = "doctrine_squad_skirmish_nato(nato)"
	})
	Purchases["2022s.nato"] = skirmish
	Purchases["mid.nato"] = skirmish
end
