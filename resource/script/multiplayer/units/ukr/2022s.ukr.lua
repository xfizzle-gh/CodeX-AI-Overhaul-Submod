-- Battle Zones uses lobby unitMode "2022s". Build a safe copy of the
-- Code:X conquest purchase tree, excluding purchase IDs absent from this roster.
require([[/script/multiplayer/units/ukr/conquest.ukr]])

local blocked = {
	["2s22_bohdana"] = true,
	["47th_inf_sniper(ukr)"] = true,
	["47th_inf_sniper_m107(ukr)"] = true,
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

if Purchases and Purchases["conquest.ukr"] then
	local purchases = copyFilteredPurchaseTree(Purchases["conquest.ukr"])
	Purchases["2022s.ukr"] = purchases
	Purchases["mid.ukr"] = purchases
end
