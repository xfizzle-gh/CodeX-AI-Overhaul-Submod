-- Code:X 2022s skirmish purchase wrapper.
-- Reuse the parent conquest roster without mutating it.
-- Sorties remain excluded while the skirmish service path is being stabilized.
require([[/script/multiplayer/units/rusa/conquest.rusa]])

local function hasType(unit, wanted)
	for _, value in ipairs(unit.type or {}) do
		if value == wanted then
			return true
		end
	end
	return false
end

local function clonePurchaseTable(source)
	local result = {}

	for i, purchase in ipairs(source) do
		local copy = {}
		for key, value in pairs(purchase) do
			copy[key] = value
		end

		copy.Units = {}
		for _, unit in ipairs(purchase.Units or {}) do
			if not hasType(unit, "Sortie") then
				table.insert(copy.Units, unit)
			end
		end

		result[i] = copy
	end

	return result
end

if Purchases and Purchases["conquest.rusa"] then
	local skirmish = clonePurchaseTable(Purchases["conquest.rusa"])

	-- Match the working Modern Conflict buy-entry convention exactly.
	-- Do not add a custom "Doctrine" type token here.
	table.insert(skirmish[1].Units, {
		priority = 1.0,
		type = {"Infantry", "Squad", "Class1"},
		unit = "doctrine_squad_skirmish_rusa(rusa)"
	})

	Purchases["2022s.rusa"] = skirmish
	Purchases["mid.rusa"] = skirmish
end
