-- Battle Zones uses lobby unitMode "2022s". Build a safe copy of the
-- Code:X conquest purchase tree, excluding purchase IDs absent from this roster.
require([[/script/multiplayer/units/prc/conquest.prc]])

local blocked = {

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

if Purchases and Purchases["conquest.prc"] then
	local purchases = copyFilteredPurchaseTree(Purchases["conquest.prc"])
	Purchases["2022s.prc"] = purchases
	Purchases["mid.prc"] = purchases
end
