local PriorityOverlay = {
	["t_55m_fin"] = 0.2,
	["leopard_1a5_n"] = 0.35,
	["leopard_c2_mexas"] = 0.35,
	["leopord_2a4_ger"] = 1.4,
	["leopard_2_pl"] = 1.5,
	["leopord_2a4m"] = 1.5,
	["leopard-2a5"] = 1.5,
	["leopord_2a6_n"] = 1.7,
	["leopard_2a7+"] = 1.8,
	["m1a1_n"] = 1.6,
	["m1a2_sep_armor"] = 1.8,
	["challenger2_n"] = 1.4,
	["challenger2_tes"] = 1.6,
	["strv_122_n"] = 1.5,
	["leclerc_sxxi"] = 1.5,

	["t-55a_mangal1"] = 0.2,
	["t55_exp"] = 0.1,
	["t-62_rus"] = 0.2,
	["t62m1_rus1"] = 0.2,
	["t72a"] = 0.4,
	["t72b_rus"] = 1.0,
	["t72b1989"] = 1.0,
	["t72b3"] = 1.4,
	["t72b3_ubh"] = 1.3,
	["t90a_rus"] = 1.5,
	["t90m"] = 1.7,
	["t90m_ha"] = 1.6,
	["t90m_2024"] = 1.7,
	["t90m_2024_ubh"] = 1.7,
	["t80bvm"] = 1.3,
	["t80bv_rus"] = 1.1,

	["t62m1_ukr"] = 0.2,
	["t72a_ukr"] = 0.5,
	["t72b_ukr"] = 0.7,
	["leopard_1a5_ukr"] = 0.4,
	["t-64bv_ukr"] = 1.2,
	["t64bv2017"] = 1.4,
	["t64bv2024"] = 1.6,
	["leopord_2a4"] = 1.4,
	["leopard_2a4_era"] = 1.5,
	["leopord_2a6"] = 1.6,
	["leopord_2a6_era"] = 1.6,
	["m1a1"] = 1.5,
	["m1a1_sa"] = 1.6,

	["ztz59"] = 0.2,
	["ztz79"] = 0.3,
	["ztz88a"] = 0.4,
	["ztz96_a"] = 1.1,
	["ztz96_a_gy"] = 1.2,
	["ztz99a"] = 1.7,
	["wz1001"] = 1.6,
}

function ApplyPurchaseOverlay(purchases)
	if type(purchases) ~= "table" then return end
	for _, pack in ipairs(purchases) do
		local units = pack and pack.Units
		if type(units) == "table" then
			for _, entry in ipairs(units) do
				local weight = entry and PriorityOverlay[entry.unit]
				if weight then entry.priority = weight end
			end
		end
	end
end
