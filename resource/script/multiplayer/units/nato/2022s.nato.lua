-- Battle Zones uses lobby unitMode "2022s". Build a safe copy of the
-- Code:X conquest purchase tree, excluding purchase IDs absent from this roster.
require([[/script/multiplayer/units/nato/conquest.nato]])

local blocked = {
	["vilkas"] = true,
	["wiesel1_gun"] = true,
	["wiesel1_tow"] = true,
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

if Purchases and Purchases["conquest.nato"] then
	local purchases = copyFilteredPurchaseTree(Purchases["conquest.nato"])
	Purchases["2022s.nato"] = purchases
	Purchases["mid.nato"] = purchases
end
