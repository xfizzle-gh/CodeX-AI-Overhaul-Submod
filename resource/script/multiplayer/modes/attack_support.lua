-- Roots cull (2026-08-29): friendly attack-support spawn path is disabled.
--
-- bot.main.lua MUST keep routing the extra Team A campaign slot here.
-- If that slot falls through to conquest.lua it purchase-spawns a second
-- friendly army. This module is an inert occupant: no Scene:SetVar of
-- id_attack_support / attack_support_ready / attack_support_use_mi, no
-- squad orders, no utility.lua / logic/main require (that path AVs here).

local PREFIX = "CODEX_ATTACK_SUPPORT"

local function emit(...)
	local out = { PREFIX .. ":" }
	for n = 1, select("#", ...) do
		out[#out + 1] = tostring(select(n, ...))
	end
	print(table.concat(out, " "))
end

local function events()
	return (BotApi and BotApi.Events) or nil
end

local function identity()
	local i = (BotApi and BotApi.Instance) or {}
	local c = (BotApi and BotApi.Conquest) or {}
	return {
		playerId = tonumber(i.playerId or 0) or 0,
		team = tostring(i.team or ""),
		attacking = c.Attacking,
	}
end

local function onGameStart()
	local id = identity()
	emit("disabled", "playerId", id.playerId, "team", id.team, "attacking", tostring(id.attacking))
end

local ev = events()
if ev and ev.Subscribe then
	ev:Subscribe(ev.GameStart, function()
		pcall(onGameStart)
	end)
end
