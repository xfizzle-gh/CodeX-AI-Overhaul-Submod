-- Battle Zones uses lobby unitMode "2022s". Build a safe copy of the
-- Code:X conquest purchase tree, excluding purchase IDs absent from this roster.
require([[/script/multiplayer/units/rusa/conquest.rusa]])

local blocked = {
	["squad_rus90_bmp1(rusa)"] = true,
	["sto_22_2(rusa)"] = true,
	["sto_22_3(rusa)"] = true,
}

local function copyFilteredPurchaseTree(source)
	local result = {}
	for _, group in ipairs(source or {}) do
		local copy = {}
		for key, value in pairs(group) do
			if key ~= "Units" then
				copy[key] = value
			end
		end
		copy.Units = {}
		for _, unit in ipairs(group.Units or {}) do
			if not blocked[unit.unit] then
				table.insert(copy.Units, unit)
			end
		end
		table.insert(result, copy)
	end
	return result
end

if Purchases and Purchases["conquest.rusa"] then
	local purchases = copyFilteredPurchaseTree(Purchases["conquest.rusa"])
	Purchases["2022s.rusa"] = purchases
	Purchases["mid.rusa"] = purchases
end
