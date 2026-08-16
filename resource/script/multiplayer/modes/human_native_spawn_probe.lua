-- #106 research only: read-only probe of the real human Dynamic Conquest slot.
--
-- Goal: determine whether the human slot, unlike the extra Team-A Mate, owns the
-- native Conquest unit catalog and spawn metadata required for a genuine native
-- support birth. This module intentionally performs NO spawn or state mutation.
--
-- Safety contract:
--   * no Spawn / SpawnAt
--   * no Events or timers
--   * no Scene / QueryScene
--   * no mission variables
--   * no unitset/preset changes
--   * no ownership/control changes
--   * one module-load snapshot only

local PREFIX = "CODEX_HUMAN_NATIVE_CONTEXT"

local CANDIDATES = {
    "rus90_inf_rifle(rusa)",
    "lud_22_1(rusa)",
}

local function emit(...)
    local out = { PREFIX .. ":" }
    for n = 1, select("#", ...) do
        out[#out + 1] = tostring(select(n, ...))
    end
    print(table.concat(out, " "))
end

local function safeRead(label, fn)
    local ok, value = pcall(fn)
    if not ok then
        emit("read_error", label, tostring(value))
        return nil
    end
    return value
end

local instance = (BotApi and BotApi.Instance) or {}
local conquest = (BotApi and BotApi.Conquest) or {}
local commands = (BotApi and BotApi.Commands) or nil

local playerId = tonumber(instance.playerId or 0) or 0
local isHuman = instance.isHuman == true or tostring(instance.isHuman or "") == "true"
local gameMode = tostring(instance.gameMode or "")
local team = tostring(instance.team or "")
local army = tostring(instance.army or "")
local attacking = conquest.Attacking

emit(
    "module_loaded",
    "playerId", playerId,
    "isHuman", tostring(isHuman),
    "gameMode", gameMode,
    "team", team,
    "army", army,
    "attacking", tostring(attacking),
    "native_spawn_calls", "disabled"
)

if not isHuman
    or gameMode ~= "campaign_capture_the_flag"
    or team ~= "a"
    or army ~= "rusa"
    or attacking ~= true then
    emit("gate_skip", "not_rusa_human_attack")
    return
end

local spawnPointName = safeRead("instance.spawnPointName", function()
    return instance.spawnPointName
end)
local playerSpawnPoint = safeRead("conquest.PlayerSpawnPoint", function()
    return conquest.PlayerSpawnPoint
end)
local enemySpawnPoint = safeRead("conquest.EnemySpawnPoint", function()
    return conquest.EnemySpawnPoint
end)

emit(
    "spawn_context",
    "instance_spawnPointName", tostring(spawnPointName),
    "playerSpawnPoint", tostring(playerSpawnPoint),
    "enemySpawnPoint", tostring(enemySpawnPoint)
)

if not commands or not commands.IsUnitAvailable then
    emit("result", "commands_or_IsUnitAvailable_missing", "native_spawn_calls", "disabled")
    return
end

local availableCount = 0
local checkedCount = 0
for _, unit in ipairs(CANDIDATES) do
    checkedCount = checkedCount + 1
    local ok, available = pcall(function()
        return commands:IsUnitAvailable(unit)
    end)
    if not ok then
        emit("unit_check", unit, "lua_error", tostring(available))
    else
        if available == true then availableCount = availableCount + 1 end
        emit("unit_check", unit, "IsUnitAvailable", tostring(available))
    end
end

emit(
    "result",
    "checked", checkedCount,
    "available", availableCount,
    "native_spawn_calls", "disabled"
)

if availableCount > 0 then
    emit("conclusion", "HUMAN_NATIVE_CATALOG_PRESENT", "next", "guarded_human_birth_probe")
else
    emit("conclusion", "HUMAN_NATIVE_CATALOG_NOT_OBSERVED", "next", "inspect_human_deployment_authority")
end
