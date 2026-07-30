param(
    [string]$RepoRoot = "",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799"
)

$ErrorActionPreference = "Stop"
$ExpectedBranch = "experiment/attack-mate-slot-proof"

$ScriptDirectory = Split-Path -Parent $MyInvocation.MyCommand.Path
if ([string]::IsNullOrWhiteSpace($RepoRoot)) {
    $RepoRoot = (Resolve-Path -LiteralPath (Join-Path $ScriptDirectory "..")).Path
} else {
    $RepoRoot = (Resolve-Path -LiteralPath $RepoRoot).Path
}
$WorkshopRoot = [System.IO.Path]::GetFullPath($WorkshopRoot)

$branch = (& git -C $RepoRoot branch --show-current 2>$null).Trim()
if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($branch)) {
    throw "Could not determine the active Git branch for: $RepoRoot"
}
if ($branch -ne $ExpectedBranch) {
    throw "Wrong source branch '$branch'. Switch GitHub Desktop to '$ExpectedBranch', pull origin, then run this script again."
}

$files = @(
    "resource\set\multiplayer\games\campaign_capture_the_flag.set",
    "resource\script\multiplayer\bot.main.lua",
    "resource\script\multiplayer\modes\attack_support.lua",
    "resource\map\multi\dcg_vars.inc",
    "resource\map\multi\attack_support_waves.inc",
    "resource\map\multi\attack_support_templates.inc",
    # Enemy-defender half of the system: garrison at the live flags, patrols, and
    # reinforcements off the defender's own map edge. Same delivery pipeline, aimed
    # the other way, gated on user_is_defender$ == 0 AND id_1st_enemy$ > 0.
    "resource\map\multi\enemy_defense_support.inc",
    "resource\map\multi\enemy_defense_templates.inc",
    "resource\script\multiplayer\modes\conquest.lua",
    # utility.lua carries the spawnPoint nil-guard. string.sub on a nil spawn
    # point faulted natively, so this file has to ship with conquest.lua or the
    # fix simply is not present in the game.
    "resource\script\multiplayer\modes\utility.lua",
    # The two human-DEFENCE mission engines. Appended rather than inserted so the
    # index-based lookups above keep pointing at the same files. Both gate every
    # trigger on user_is_defender$ == 1 and both wait for prep_inform$ == 1, and
    # neither parks prototypes of its own: defence support claims from the
    # attack-support NATO pool and enemy attack claims from the enemy-defence
    # faction pools, which is safe because the attack-mission engines that own
    # those pools are inert on exactly the missions these two run on.
    "resource\map\multi\defense_support_waves.inc",
    "resource\map\multi\enemy_attack_support.inc",
    # Player-nation prototype pools, drawn by BOTH the attack-support and the
    # defence-support engine. Safe to share because those two never run on the same
    # mission: attack support gates every trigger on user_is_defender$ == 0 and
    # defence support on == 1. APPENDED, never inserted - the $files[n] lookups
    # below are positional, so inserting this anywhere earlier silently repoints
    # every later index at the wrong file.
    "resource\map\multi\faction_support_templates.inc",
    # Player-facing support announcement strings (Phase 1). APPENDED only.
    "localizations\default\interface\text\mission\multi\support_events.pot",
    "localizations\default\interface\text\mission\multi\ce_mission_messages.pot",
    # Phase-4 flag prop prototypes (shared by Q2 + Q4 garrison steps).
    "resource\map\multi\flag_props_templates.inc",
    # Shadow of the base-game flag ammo supply entity. Identical to the vanilla def
    # except that it pulls Code:X's modern resupply tables instead of the WW2 ones,
    # and it is what the per-flag {Link ... "ammo"} lines the map patcher writes
    # actually resolve to. Same virtual path as the pak entry, so the .mdl and the
    # supply_zone decal still come from the pak and are not shipped here.
    # APPENDED only - the $files[n] lookups above are positional.
    "resource\entity\service\-multiplayer\flag_point\flagpoint_ammo\flagpoint_ammo.def",
    # E2 CE routing mirrors: source/deployed copies must remain byte-identical.
    "resource\map\multi\ce\ai_logic\ce_ai_logic_triggers.inc",
    "resource\map_scripts\ai_logic\ce_ai_logic_triggers.inc"
)

$gameSetSource = Join-Path $RepoRoot $files[0]
$botMainSource = Join-Path $RepoRoot $files[1]
$supportSource = Join-Path $RepoRoot $files[2]
$varsSource = Join-Path $RepoRoot $files[3]
$wavesSource = Join-Path $RepoRoot $files[4]
$tplSource = Join-Path $RepoRoot $files[5]
$defSource = Join-Path $RepoRoot $files[6]
$defTplSource = Join-Path $RepoRoot $files[7]
$conquestSource = Join-Path $RepoRoot $files[8]
$utilitySource = Join-Path $RepoRoot $files[9]
$dsSource = Join-Path $RepoRoot $files[10]
$eaSource = Join-Path $RepoRoot $files[11]
$factionTplSource = Join-Path $RepoRoot $files[12]
$flagPropsTplSource = Join-Path $RepoRoot $files[15]
$flagAmmoDefSource = Join-Path $RepoRoot $files[16]
$ceMapSource = Join-Path $RepoRoot $files[17]
$ceScriptSource = Join-Path $RepoRoot $files[18]

foreach ($source in @($gameSetSource, $botMainSource, $supportSource, $varsSource, $wavesSource, $tplSource, $defSource, $defTplSource, $conquestSource, $utilitySource, $dsSource, $eaSource, $factionTplSource, $flagPropsTplSource, $flagAmmoDefSource, $ceMapSource, $ceScriptSource)) {
    if (-not (Test-Path -LiteralPath $source)) {
        throw "Missing source file: $source"
    }
}
$ceSourceHashes = @($ceMapSource, $ceScriptSource) | ForEach-Object { (Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash }
if ($ceSourceHashes[0] -ne $ceSourceHashes[1]) { throw "Source CE ai_logic mirrors are not byte-identical" }
foreach ($marker in @('{"support_e2_test"}', '{"support_e2_stage"}', '{"support_e2_fail"}', '{"support_e2_lz"}', '{"support_e2_flag"}')) {
    if (-not (Select-String -Quiet -LiteralPath $varsSource -SimpleMatch $marker)) { throw "Source dcg_vars.inc is missing E2 state: $marker" }
}
foreach ($marker in @('{Entity "mi17_b8_rus"', '{Entity "mi17_b8_ukr"', '{Entity "uh-60m_blackhawk_mg"', '{Entity "il-76td_para"', '{Entity "c130_para"', 'support_e2_para_pax', '{Chassis "helicopter"')) {
    if (-not (Select-String -Quiet -LiteralPath $factionTplSource -SimpleMatch $marker)) { throw "Source faction pool is missing E2 marker: $marker" }
}
foreach ($key in @('mission/multi/support/e2_helo_inbound', 'mission/multi/support/e2_para_inbound', 'mission/multi/support/e2_insert_failed')) {
    if (-not (Select-String -Quiet -LiteralPath (Join-Path $RepoRoot $files[13]) -SimpleMatch "msgctxt `"$key`"")) { throw "Source support_events.pot is missing msgctxt $key" }
}
$E2HeloWaveMarkers = @(
    '; ===== E2 REAL AIR INSERT PROBES =====',
    '{"attack_support/e2_dispatch"',
    '{"attack_support/e2_helo_rusa"',
    '{"attack_support/e2_helo_ukr"',
    '{"attack_support/e2_helo_nato"',
    '{"air_state"',
    'support_e2_lz',
    '{"delete"'
)
$E2HeloTemplateMarkers = @('{Altitude 22}')
$E2HeloForbiddenMarkers = @('attack_support/e2_helo_prc', '{clone}', 'support_e2_lz_fpc')
foreach ($marker in $E2HeloWaveMarkers) {
    if (-not (Select-String -Quiet -LiteralPath $wavesSource -SimpleMatch $marker)) { throw "Source wave engine is missing E2 helicopter marker: $marker" }
}
foreach ($marker in $E2HeloTemplateMarkers) {
    if (-not (Select-String -Quiet -LiteralPath $factionTplSource -SimpleMatch $marker)) { throw "Source E2 helicopter template is missing marker: $marker" }
}
foreach ($marker in $E2HeloForbiddenMarkers) {
    if (Select-String -Quiet -LiteralPath $wavesSource -SimpleMatch $marker) { throw "Source wave engine contains forbidden E2 helicopter marker: $marker" }
}
# string.sub on a nil spawnPointName faulted natively on slots the engine gives no
# spawn point, so the read must stay type-guarded before the substring.
if (-not (Select-String -Quiet -LiteralPath $utilitySource -SimpleMatch 'if type(spawnPoint) ~= "string" or spawnPoint == "" then')) {
    throw "Source utility.lua is missing the spawnPoint nil-guard, which crashes natively on a slot with no spawn point"
}
# The whole point of shadowing this def is the modern ammo table. Shipping it with
# the base include would silently give every flag a WW2 crate whose regeneration is
# switched off by gameclass - the exact defect this replaces - so the swap is pinned
# here, and so is the call that consumes it.
if (-not (Select-String -Quiet -LiteralPath $flagAmmoDefSource -SimpleMatch '(include "/properties/resupply_hotmod.inc")')) {
    throw "Source flagpoint_ammo.def does not pull the modern resupply tables"
}
if (Select-String -Quiet -LiteralPath $flagAmmoDefSource -SimpleMatch '(include "/properties/resupply.inc")') {
    throw "Source flagpoint_ammo.def still pulls the base WW2 resupply tables"
}
if (-not (Select-String -Quiet -LiteralPath $flagAmmoDefSource -SimpleMatch '("flag_ammo_heavy")')) {
    throw "Source flagpoint_ammo.def is missing the flag_ammo_heavy supply-zone call"
}

if (-not (Select-String -Quiet -LiteralPath $gameSetSource -SimpleMatch "{aiTeamPlayers 1}")) {
    throw "Source game set does not contain the attack support AI slot marker"
}
if (Select-String -Quiet -LiteralPath $botMainSource -SimpleMatch "first_player_slot") {
    throw "Source bot.main.lua still contains the regressed FirstPlayerId skip gate"
}
if (-not (Select-String -Quiet -LiteralPath $botMainSource -SimpleMatch "Never use FirstPlayerId to exclude a")) {
    throw "Source bot.main.lua is missing the FirstPlayerId lesson guard comment"
}
if (-not (Select-String -Quiet -LiteralPath $botMainSource -SimpleMatch "local function safeRequire(path)")) {
    throw "Source bot.main.lua is missing guarded module loading"
}
if (Select-String -Quiet -LiteralPath $botMainSource -SimpleMatch "team_a_attack_safe_route") {
    throw "Source bot.main.lua contains the superseded crashing Team A route"
}
foreach ($marker in @(
    'sc:SetVar("id_attack_support", id.playerId)',
    'sc:SetVar("attack_support_ready", 1)',
    # Attack support arms MI delivery explicitly. Lua Spawn on this slot never reports
    # an available unit, so MI is the only path that puts bodies on the map.
    'sc:SetVar("attack_support_use_mi", 1)'
)) {
    if (-not (Select-String -Quiet -LiteralPath $supportSource -SimpleMatch $marker)) {
        throw "Source attack_support.lua is missing marker: $marker"
    }
}
# THE ENGINE-STATE MIRROR. The on-screen diagnostics are gated behind support_debug$ and
# ship off, so game.log is the only place a run can be read back. This slot loads on every
# campaign_capture_the_flag mission - attack or defence - so it reports all four wave
# engines from one place, always on, every MIRROR_QUANTS quants. Reads go through readVar,
# which pcall-guards GetVar: that getter is not proven on this BotApi surface and this is
# the slot where a mishandled native getter takes the whole process with it.
$MirrorMarkers = @(
    'local function readVar(name)',
    'local ok, v = pcall(function() return sc:GetVar(name) end)',
    'local MIRROR_QUANTS = 200',
    'local function mirrorEngineState()',
    '"faction_support_army", readVar("faction_support_army")',
    'emit("mirror", "attack_support",',
    'emit("mirror", "enemy_defense",',
    '"garrison_place", readVar("enemy_defense_place")',
    'emit("mirror", "defense_support",',
    'emit("mirror", "enemy_attack",',
    'if state.quant % MIRROR_QUANTS == 0 then',
    # The heartbeat predates the mirror and stays.
    'log("heartbeat", "q", state.quant)',
    'readVar("support_e2_test")',
    'readVar("support_e2_stage")',
    'readVar("support_e2_fail")',
    'readVar("support_e2_lz")',
    'readVar("support_e2_flag")'
)
foreach ($marker in $MirrorMarkers) {
    if (-not (Select-String -Quiet -LiteralPath $supportSource -SimpleMatch $marker)) {
        throw "Source attack_support.lua is missing the engine-state mirror: $marker"
    }
}
# These reads AV on a slot with no spawn deck, and pulling in utility.lua from
# here crashed in lua.event.notify2 the moment the module loaded. Checked against
# a comment-stripped view: the file's own header names these forms as the rule.
function Get-LuaCode([string]$path) {
    return ((Get-Content -LiteralPath $path) | ForEach-Object { ($_ -split '--', 2)[0] }) -join "`n"
}
# Comment-stripped view of an MI include. The engine headers name every neighbouring
# system in prose - which files they mirror, which pools they share, which var they
# deliberately do NOT read - so a cross-system substring check has to run on code only.
function Get-MiCode([string]$path) {
    return ((Get-Content -LiteralPath $path) | ForEach-Object { ($_ -split ';', 2)[0] }) -join "`n"
}
# THE SUPPORT TIMER GATE (ship requirement). Every {"timer"} in the four wave engines
# is either a developer diagnostic (support_debug$ == 1, default OFF) or a player-facing
# announcement (support_announce$ == 1, default ON via engine init). Both shapes are:
#   {"switch"
#     {"case" {condition {type cmp_i} {var "support_debug|announce$"} {op "=="} {value 1}} <timer>}
#     {"default"}
#   }
# An ungated timer fails the deploy. Comment-stripped view only (headers quote the gate).
$SupportTimerGate = '\{condition \{type cmp_i\} \{var "support_(?:debug|announce)\$"\} \{op "=="\} \{value 1\}\}\s*\{"timer"'
function Test-SupportTimerGate([string]$path, [string]$label) {
    $code = Get-MiCode $path
    $timers = ([regex]::Matches($code, '\{"timer"')).Count
    $gated = ([regex]::Matches($code, $SupportTimerGate)).Count
    if ($timers -eq 0) {
        throw "$label has no timers left at all, which means the gate rewrite lost them: $path"
    }
    if ($gated -ne $timers) {
        throw "$label ships $($timers - $gated) UNGATED on-screen timer(s) of $timers - must gate on support_debug`$ or support_announce`$: $path"
    }
    # Diagnostics stay default-OFF: engines must never write support_debug$.
    # support_announce$ IS written (init sets it to 1) and that is intentional.
    if ($code.Contains('{var "support_debug$"} {op "="}')) {
        throw "$label writes support_debug`$, so the shipped default is not OFF: $path"
    }
    Write-Host "OK gate $label $gated/$timers timers behind support_debug|announce`$"
}

# ---------------------------------------------------------------------------
# FLAG AMMO SUPPLY POINTS
#
# Every flag in this map family ships with an EMPTY built-in placer socket -
# {Placer {State "ammo" {Unlinked}}} - and nothing on the map ever fills it, so
# holding a flag buys the holder no resupply whatsoever. Vanilla's own CTF maps
# fill that socket the other way round and never carry the Placer block at all:
# a childless {Entity "flagpoint_ammo"} carrying the supply_zone extender, plus a
# {Link <child> {<flag> "ammo"}} line binding it into the flag's "ammo" slot.
# Reference shape, base game 2v2_countryside/battle_zones.mi lines 353-357 and
# 401. This step reproduces that exactly, once per flag, on every managed map.
#
# The ammo TABLE comes from the shadow def this repo ships at
# resource/entity/service/-multiplayer/flag_point/flagpoint_ammo/flagpoint_ammo.def
# - the vanilla def with one line changed, /properties/resupply.inc ->
# /properties/resupply_hotmod.inc, so its ("flag_ammo_heavy") call resolves to
# Code:X's modern table (24m radius, 5s regeneration, limit 750, modern items)
# instead of the base one, whose items are WW2 and whose regeneration is disabled
# by gameclass. Only the .def is shadowed: the .mdl and supply_zone.ebm resolve
# from the pak through the same virtual path (precedent: barbwire_on_wall.def).
#
# The flag never carries both forms. Vanilla has the Link and no Placer; a
# pristine map here has the Placer and no Link; a patched map must look like
# vanilla, so the empty socket is removed as part of linking.
#
# Self-healing and idempotent: every block this step has ever written is stripped
# first and then rebuilt from the flags actually present in the file, so a rerun
# is a byte-for-byte no-op and an interrupted run repairs itself on the next pass.
#
# Child entity ids come from 0xfd00 upward. That band was collision-swept across
# every .mi and .inc in resource/map/multi (highest id anywhere in the family is
# 0xf801; the parked pools live at 0xb0xx), and the sweep is re-asserted per file
# below rather than trusted - ids only have to be unique within one mission file,
# but a silent collision would rebind somebody else's link.
$FlagAmmoIdBase = 0xfd00
# Insertion anchor. The generated blocks belong in the entities section, after the
# map's own {Link} lines and ahead of the {Tags} block, which is exactly where the
# templates include already sits - and unlike the {Link} lines themselves (factory
# and winds_valley have none at all) it is present exactly once in every managed
# map, repo copy included, and is verified so by the include checks below.
$FlagAmmoAnchor = '(include "../attack_support_templates.inc")'
function Set-FlagAmmoSupply([string]$path, [string]$label) {
    $text = [System.IO.File]::ReadAllText($path)

    # 1. Strip everything a previous run of this step wrote, so what follows is
    #    generated from scratch rather than reconciled.
    $text = [regex]::Replace(
        $text,
        '[ \t]*\{Entity "flagpoint_ammo" 0x[0-9a-fA-F]+\r?\n[ \t]*\{Extender "supply_zone"\r?\n[ \t]*\{enabled\}\r?\n[ \t]*\{current 0\}\r?\n[ \t]*\}\r?\n[ \t]*\}\r?\n',
        ''
    )
    $text = [regex]::Replace(
        $text,
        '[ \t]*\{Link 0x[0-9a-fA-F]+ \{0x[0-9a-fA-F]+ "ammo"\}\}\r?\n',
        ''
    )
    if ($text.Contains('flagpoint_ammo')) {
        throw "Could not strip a previously written flagpoint_ammo block from: $path"
    }

    # 2. Retire the empty socket. A flag on this family carries at most two placer
    #    states, "sandbags" and "ammo" (factory has both, everything else only
    #    ammo), so drop the ammo state and then drop the Placer block if that left
    #    it with nothing in it. A Placer that still holds sandbags is untouched.
    $text = [regex]::Replace($text, '[ \t]*\{State "ammo" \{Unlinked\}\}\r?\n', '')
    $text = [regex]::Replace($text, '[ \t]*\{Placer\r?\n[ \t]*\}\r?\n', '')
    if ([regex]::IsMatch($text, '\{State "ammo" \{Unlinked\}\}')) {
        throw "Map still carries an unlinked ammo placer socket after the strip: $path"
    }

    # 3. One supply point per campaign flag, in file order.
    $flagIds = @()
    foreach ($m in [regex]::Matches($text, '\{Entity "flag_point_campaign_\d+" (0x[0-9a-fA-F]+)')) {
        $flagIds += $m.Groups[1].Value
    }
    if ($flagIds.Count -lt 1) {
        throw "No flag_point_campaign entities to link ammo supply into: $path"
    }
    if (($flagIds | Sort-Object -Unique).Count -ne $flagIds.Count) {
        throw "Duplicate flag entity ids in: $path"
    }
    if (-not $text.Contains($FlagAmmoAnchor)) {
        throw "Map is missing the flag-ammo insertion anchor: $path"
    }

    $entities = ""
    $links = ""
    for ($i = 0; $i -lt $flagIds.Count; $i++) {
        $childId = "0x{0:x}" -f ($FlagAmmoIdBase + $i)
        if ([regex]::IsMatch($text, '\b' + [regex]::Escape($childId) + '\b')) {
            throw "Flag-ammo child id $childId already in use in: $path"
        }
        # Entity block first, Link line after - the engine resolves a Link against
        # entities already declared, and vanilla emits them in that order too.
        $entities += "{Entity `"flagpoint_ammo`" $childId`r`n`t`t{Extender `"supply_zone`"`r`n`t`t`t{enabled}`r`n`t`t`t{current 0}`r`n`t`t}`r`n`t}`r`n`t"
        $links += "{Link $childId {$($flagIds[$i]) `"ammo`"}}`r`n`t"
    }
    # The anchor already carries its own leading tab, which becomes the indent of
    # the first emitted line; every block trails one so the anchor lands indented.
    $text = $text.Replace($FlagAmmoAnchor, $entities + $links + $FlagAmmoAnchor)
    [System.IO.File]::WriteAllText($path, $text, [System.Text.UTF8Encoding]::new($false))

    # 4. Re-read and verify against the file on disk, not against the buffer.
    $text = [System.IO.File]::ReadAllText($path)
    $entityCount = ([regex]::Matches($text, '\{Entity "flagpoint_ammo" 0x[0-9a-fA-F]+')).Count
    if ($entityCount -ne $flagIds.Count) {
        throw "Expected $($flagIds.Count) flagpoint_ammo entities in $path (found $entityCount)"
    }
    $linkMatches = [regex]::Matches($text, '\{Link (0x[0-9a-fA-F]+) \{(0x[0-9a-fA-F]+) "ammo"\}\}')
    if ($linkMatches.Count -ne $flagIds.Count) {
        throw "Expected $($flagIds.Count) ammo links in $path (found $($linkMatches.Count))"
    }
    $sources = @(); $targets = @()
    foreach ($m in $linkMatches) {
        $sources += $m.Groups[1].Value
        $targets += $m.Groups[2].Value
    }
    if (($sources | Sort-Object -Unique).Count -ne $sources.Count) {
        throw "Duplicate flag-ammo child ids in: $path"
    }
    if (($targets | Sort-Object -Unique).Count -ne $targets.Count) {
        throw "Two ammo supply points linked into the same flag in: $path"
    }
    foreach ($flagId in $flagIds) {
        if ($targets -notcontains $flagId) {
            throw "Flag $flagId has no ammo supply point in: $path"
        }
    }
    if ([regex]::IsMatch($text, '\{State "ammo" \{Unlinked\}\}')) {
        throw "Map carries both a linked supply point and an unlinked socket: $path"
    }
    Write-Host "FLAG-AMMO $label $($flagIds.Count) flag(s) linked in $path"
    return $flagIds.Count
}

$SlotUnsafe = @('spawnPointName', 'PlayerSpawnPoint', 'require(')
$supportCode = Get-LuaCode $supportSource
foreach ($banned in $SlotUnsafe) {
    if ($supportCode.Contains($banned)) {
        throw "Source attack_support.lua touches the slot-unsafe surface: $banned"
    }
}
foreach ($marker in @(
    # THE SHIP TOGGLE. Every on-screen {"timer" ...} diagnostic in all four wave engines
    # is wrapped in {"switch" {"case" {condition {type cmp_i} {var "support_debug$"}
    # {op "=="} {value 1}} <timer>} {"default"}}. Nothing ever writes this var, and an
    # unwritten MI var reads 0, so the shipped default is OFF and a player sees zero
    # support timers. Flip this one var to 1 - a single set_i anywhere that runs, or
    # sc:SetVar("support_debug", 1) from Lua - and every diagnostic in all four engines
    # comes back at once. The Test-SupportTimerGate check below is what keeps that true:
    # an ungated timer fails the deploy rather than shipping HUD spam to players.
    '{"support_debug"}',
    # Player-facing support announcements (default ON). Engines set this to 1 at init.
    # Distinct from support_debug$: diagnostics stay OFF; announcements stay ON.
    '{"support_announce"}',
    # Round-robin cursor over the two attack-support flank pads (Phase 2).
    '{"attack_support_flank_rr"}',
    # Rare IFV wave budget (Phase 3). 1 per mission max.
    '{"attack_support_ifv_left"}',
    # Motorized truck insert budget (cmd 19). 1 per mission max.
    '{"attack_support_motor_left"}',
    # Airmobile insert budget (Phase 5 E1). Cap 2/mission. Narrative helo only.
    '{"attack_support_air_left"}',
    '{"defense_support_air_left"}',
    '{"defense_support_use_air"}',
    '{"defense_support_air_test_done"}',
    # 1 = force first airmobile ~30s for Day-2 testing; set 0 to restore production schedule.
    '{"attack_support_air_test"}',
    # 1 when current placement batch uses an airmobile LZ pad.
    '{"attack_support_use_air"}',
    # 1 when the current attack wave elected a flank pad (announce + place path).
    '{"attack_support_use_flank"}',
    # One-shot flag-prop placement per mission (Phase 4).
    '{"flag_props_done"}',
    # Round-robin cursor per engine over that side's three entry pads. Bumped once per
    # placement batch, so no two consecutive batches land on the same pad. Undeclared it
    # would read a silent zero forever and every batch would pile onto pad 1 again.
    '{"attack_support_entry_rr"}',
    '{"enemy_defense_entry_rr"}',
    '{"defense_support_entry_rr"}',
    '{"enemy_attack_entry_rr"}',
    '{"attack_support_armed"}',
    '{"attack_support_transferred"}',
    '{"attack_support_stage"}',
    '{"attack_support_wave_cmd"}',
    '{"attack_support_wave_num"}',
    '{"attack_support_waves_left"}',
    '{"attack_support_busy"}',
    '{"attack_support_next_ok"}',
    '{"attack_support_hmmwv_left"}',
    '{"attack_support_use_mi"}',
    # Enemy defence state. Declared here because dcg_vars.inc is the only var block
    # every one of the fourteen maps pulls in; an undeclared read is a silent zero.
    '{"enemy_defense_armed"}',
    '{"enemy_defense_army"}',
    '{"enemy_defense_stage"}',
    '{"enemy_defense_transferred"}',
    '{"enemy_defense_wave_cmd"}',
    '{"enemy_defense_wave_num"}',
    '{"enemy_defense_waves_left"}',
    '{"enemy_defense_place"}',
    '{"enemy_defense_group"}',
    '{"enemy_defense_trickle_ok"}',
    '{"enemy_defense_trickle_busy"}',
    '{"enemy_defense_surge_ok"}',
    '{"enemy_defense_surge_busy"}',
    # Human-DEFENCE mission state. Same reason: an undeclared read is a silent zero,
    # and a silent zero on defense_support_armed$ would re-arm the engine every tick.
    '{"defense_support_armed"}',
    '{"defense_support_transferred"}',
    '{"defense_support_stage"}',
    '{"defense_support_wave_cmd"}',
    '{"defense_support_wave_num"}',
    '{"defense_support_waves_left"}',
    '{"defense_support_busy"}',
    '{"defense_support_next_ok"}',
    '{"defense_support_group"}',
    '{"defense_support_owner_fail"}',
    '{"enemy_attack_armed"}',
    '{"enemy_attack_army"}',
    '{"enemy_attack_stage"}',
    '{"enemy_attack_transferred"}',
    '{"enemy_attack_wave_cmd"}',
    '{"enemy_attack_wave_num"}',
    '{"enemy_attack_waves_left"}',
    '{"enemy_attack_busy"}',
    '{"enemy_attack_next_ok"}',
    '{"enemy_attack_owner_fail"}',
    '{"enemy_attack_motor_left"}'
)) {
    if (-not (Select-String -Quiet -LiteralPath $varsSource -SimpleMatch $marker)) {
        throw "Source dcg_vars.inc is missing marker: $marker"
    }
}
# Source side of the diagnostic gate, checked before anything is copied anywhere.
foreach ($pair in @(
    @($wavesSource, 'source attack support engine'),
    @($defSource, 'source enemy defence engine'),
    @($dsSource, 'source defence support engine'),
    @($eaSource, 'source enemy attack engine')
)) {
    Test-SupportTimerGate $pair[0] $pair[1]
}
# The retired allied-support experiment owned these. Its files are gone; a stale
# declaration invites someone to re-gate a production engine behind one.
foreach ($banned in @('allied_support_initialized', 'allied_support_wave_size', 'allied_support_target')) {
    if (Select-String -Quiet -LiteralPath $varsSource -SimpleMatch $banned) {
        throw "Source dcg_vars.inc declares retired allied-support state: $banned"
    }
}
# The retired inert wave skeleton owned these. Nothing reads them any more, and a
# stale declaration invites someone to re-gate the production engine behind one.
foreach ($banned in @('allied_attack_enabled', 'allied_attack_started', 'allied_attack_wave_num')) {
    if (Select-String -Quiet -LiteralPath $varsSource -SimpleMatch $banned) {
        throw "Source dcg_vars.inc still declares retired skeleton state: $banned"
    }
}
foreach ($marker in @(
    '{var "user_is_defender$"}',
    '{var "attack_support_ready$"}',
    '{var "attack_support_use_mi$"}',
    # The numbered pads, not the bare legacy name: placements round-robin across the
    # triple and only the patrol / roam move orders still address the alias.
    '{target_waypoint "attack_support_entry_a1"}',
    '{target_waypoint "attack_support_entry_b1"}',
    '{var "enemy_spawnside$"}',
    '{player "3"}',
    # Command-gated compositions. Waves keyed on entity presence alone all fired at
    # once; each composition now needs its own command value and clears it on entry.
    '{var "attack_support_wave_cmd$"}',
    '{"attack_support/init"',
    '{"attack_support/clock"',
    '{"attack_support/comp_usmc"',
    '{"attack_support/comp_1ad"',
    '{"attack_support/comp_acav"',
    '{"attack_support/comp_pzgren"',
    '{"attack_support/comp_arf"',
    # Faction-aware pools: the player's own nation supplies the wave. One trigger per
    # faction per comp, cmds 10-16, folded out of user_nation$ by as_resolve_army.
    '{var "faction_support_army$"}',
    '("as_resolve_army")',
    '("as_pick_hybrid_non_nato")',
    '{"attack_support/ally_rusa_line"',
    '{"attack_support/ally_ukr_line"',
    '{"attack_support/ally_prc_line"',
    '{"attack_support/ally_nato_line"',
    '{"attack_support/ally_rusa_manpad"',
    '{"attack_support/ally_ukr_veh"',
    '{"attack_support/ally_nato_veh"',
    # Self-re-arming randomized cadence and the level-scaled wave budget. The ladder
    # is 120-240s, about 20% tighter than the retired 150-300s one.
    '{"trigger" {name "attack_support/clock"}}',
    '{condition {type rand} {value 0.2}}',
    '{"delay" {time 120}}',
    '{"delay" {time 240}}',
    '{var "defense_level$"}',
    '{var "attack_support_waves_left$"}',
    '{var "attack_support_next_ok$"}',
    '{var "attack_support_busy$"}',
    # Live-unit cap, counted with the simple selector form the mission scripts use
    # for live units. Deferring must not consume a wave.
    '{tag attack_support_src}',
    '{state "not dead"}',
    'ATTACK SUPPORT NEAR CAP DEFER',
    'ATTACK SUPPORT WAVES EXHAUSTED',
    # Pool tags double as availability: a deploy strips the tag it took from.
    '{tag_remove attack_support_inf_usmc}',
    '{tag_remove attack_support_inf_1ad}',
    '{tag_remove attack_support_inf_pzgd}',
    '{tag_remove attack_support_inf_arf}',
    '{tag_remove ally_sup_rusa_line}',
    '{tag_remove ally_sup_nato_manpad}',
    # Crewed humvees are {Link}ed to one hull, so instances move whole and in order.
    '{var "attack_support_hmmwv_left$"}',
    '("am_deploy_next_hmmwv")',
    '("am_pick_composition")',
    '("am_place_at_entry")',
    '("am_own_to_support")',
    '("am_finish_deploy")',
    # Capture points are addressed as {tag flag}. The fpc1..fpc5 tags are absent
    # from one of the fourteen maps entirely, which left units standing still.
    '{select {tag {tag flag}}}',
    '{tag_add attack_support_flag1}',
    '{tag_add attack_support_flag2}',
    '{tag_add attack_support_flag3}',
    # No-clone design: the pool originals are MOVED to the entry waypoint and
    # promoted in place, so they still carry the tag we put on them.
    '{tag_remove attack_support_tpl}',
    # Decorating the advanced selector that addresses pool units zeroes the
    # match, so the deploy set is selected bare and nothing else.
    '{group {select {tag {tag attack_support_deploy}}}}'
)) {
    if (-not (Select-String -Quiet -LiteralPath $wavesSource -SimpleMatch $marker)) {
        throw "Source wave engine is missing marker: $marker"
    }
}
foreach ($n in 1..16) {
    if (-not (Select-String -Quiet -LiteralPath $wavesSource -SimpleMatch ('{player "' + $n + '"}'))) {
        throw "Source wave engine is missing literal ownership case for player $n. The engine will not accept a var in the {player} node, so all sixteen slots must be spelled out"
    }
}
if (Select-String -Quiet -LiteralPath $wavesSource -SimpleMatch '{tag attack_support_probe}') {
    throw "Source wave engine still gates on attack_support_probe. That tag is a best-effort marker only; key on attack_support_src, which is proven queryable on these units"
}
if (Select-String -Quiet -LiteralPath $wavesSource -SimpleMatch '{tag_remove attack_support_src}') {
    throw "Source wave engine removes attack_support_src, but the entire downstream chain selects on it"
}
if ((Select-String -LiteralPath $wavesSource -SimpleMatch '{state {state inactive}}').Count -lt 3) {
    throw "Source wave engine must exclude inactive flag points on ALL THREE shuffled flag picks - a mission activates only ~2 of a map's capture points, and a squad sent to a dead objective just sprints and stands there"
}
# Decorating the advanced selector that addresses pool units zeroes the match.
# Live proof in one run: a bare select moved all four; the same select plus a
# prop/state decoration matched nothing in the very next action.
foreach ($banned in @('{include {prop human}}', '{prop {prop human}}', '{state {state operatable}}')) {
    if (Select-String -Quiet -LiteralPath $wavesSource -SimpleMatch $banned) {
        throw "Source wave engine decorates a pool selector with $banned, which zeroes the match on these units"
    }
}
if (Select-String -Quiet -LiteralPath $wavesSource -Pattern '^[^;]*\bfpc') {
    throw "Source wave engine still targets fpc* capture points. Those tags are absent from outback entirely; address capture points as {tag flag}"
}
if (Select-String -Quiet -LiteralPath $wavesSource -SimpleMatch '{clone}') {
    throw "Source wave engine still clones. Three promote designs failed to match a cloned entity; a new entity's provenance is invisible to selectors on this engine. Move the originals instead"
}
if (Select-String -Quiet -LiteralPath $wavesSource -SimpleMatch '{zone {zone "gamezone"}}') {
    throw "Source wave engine separates entities by zone. allied_support_entry is a waypoint, not a zone, and is NOT inside gamezone"
}
if (Select-String -Quiet -LiteralPath $wavesSource -SimpleMatch '{"delay" {time 8}}') {
    throw "Source wave engine still contains the superseded blind startup delay"
}
# ===== ENEMY DEFENCE ENGINE (source side) =====
# The mirror of the attack-support engine. Same hard-won pipeline: no clone, bare
# pool selectors, literal {player} switch, {tag flag} capture points.
foreach ($marker in @(
    '{"enemy_defense/init"',
    '{"enemy_defense/trickle"',
    '{"enemy_defense/surge"',
    '{"enemy_defense/patrol_1"',
    '{"enemy_defense/patrol_4"',
    '{var "user_is_defender$"}',
    '{var "id_1st_enemy$"}',
    '{var "enemy_spawnside$"}',
    '{var "bot_army$"}',
    '{var "defense_level$"}',
    '{var "enemy_defense_army$"}',
    '{var "enemy_defense_wave_cmd$"}',
    '{var "enemy_defense_waves_left$"}',
    '{var "enemy_defense_place$"}',
    '{var "enemy_defense_group$"}',
    # Two independent self-re-arming spawners on different random ladders, so
    # arrivals never synchronise. Trickle 45-90s, surge 180-300s.
    '{"trigger" {name "enemy_defense/trickle"}}',
    '{"trigger" {name "enemy_defense/surge"}}',
    '{"delay" {time 45}}',
    '{"delay" {time 90}}',
    '{"delay" {time 180}}',
    '{"delay" {time 300}}',
    # Patrol re-order cadence, 60-120s per group, self-re-arming.
    '{"trigger" {name "enemy_defense/patrol_1"}}',
    '{"delay" {time 60}}',
    '{"delay" {time 120}}',
    # Garrison lands ON the active flag points; reinforcements at the defender's
    # OWN map edge - for this system enemy_spawnside$ 1 means side a, not side b.
    '{target {ignore_captured_by_user 0} {tag enemy_def_af1}}',
    '{tag_add enemy_def_af1}',
    '{tag_add enemy_def_r1}',
    # The numbered pads, not the bare legacy name: placements round-robin across the
    # triple and only the patrol / roam move orders still address the alias.
    '{target_waypoint "attack_support_entry_a1"}',
    '{target_waypoint "attack_support_entry_b1"}',
    '{waypoint "0"}',
    # Live cap defers without consuming a wave, exactly as attack support does.
    '{tag enemy_def_src}',
    '{state "not dead"}',
    '{count {op ">"} {value 16}}',
    'ENEMY DEFENSE NEAR CAP DEFER',
    'ENEMY DEFENSE WAVES EXHAUSTED',
    'ENEMY DEFENSE POOL SHORT - LINE TEAM INSTEAD',
    'ENEMY DEFENSE POOL EXHAUSTED',
    'ENEMY DEFENSE GARRISON AT FLAG 1',
    # Patrols may fall back; this is not a suicide push.
    '{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}',
    # Pool tags double as availability: a claim strips the tag it took from.
    '{tag_remove enemy_def_rusa_line}',
    '{tag_remove enemy_def_ukr_line}',
    '{tag_remove enemy_def_prc_line}',
    '{tag_remove enemy_def_nato_line}',
    '{tag_remove enemy_def_rusa_wpn}',
    '{tag_remove enemy_def_nato_wpn}',
    '{tag_remove enemy_def_tpl}',
    # Capture points are addressed as {tag flag}; fpc* is absent from outback.
    '{select {tag {tag flag}}}',
    # Bare deploy selector - decorating it zeroes the match on these units.
    '{group {select {tag {tag enemy_def_deploy}}}}',
    '("ed_place")',
    '("ed_finish")',
    '("ed_own_to_enemy")',
    '("ed_assign_group")',
    '("ed_resolve_army")',
    '("ed_claim_anchors")',
    '("ed_pick_garrison")',
    '("ed_pick_light")',
    '("ed_pick_squad")'
)) {
    if (-not (Select-String -Quiet -LiteralPath $defSource -SimpleMatch $marker)) {
        throw "Source enemy defence engine is missing marker: $marker"
    }
}
foreach ($n in 1..16) {
    if (-not (Select-String -Quiet -LiteralPath $defSource -SimpleMatch ('{player "' + $n + '"}'))) {
        throw "Source enemy defence engine is missing literal ownership case for player $n. The engine will not accept a var in the {player} node, so all sixteen slots must be spelled out"
    }
}
# The same pipeline constraints that cost the attack-support engine a live run each.
foreach ($banned in @('{clone}', '{include {prop human}}', '{prop {prop human}}', '{state {state operatable}}', '{zone {zone "gamezone"}}')) {
    if (Select-String -Quiet -LiteralPath $defSource -SimpleMatch $banned) {
        throw "Source enemy defence engine uses the forbidden idiom $banned"
    }
}
if (Select-String -Quiet -LiteralPath $defSource -Pattern '^[^;]*\bfpc') {
    throw "Source enemy defence engine still targets fpc* capture points. Those tags are absent from outback entirely; address capture points as {tag flag}"
}
if (Select-String -Quiet -LiteralPath $defSource -SimpleMatch '{tag_remove enemy_def_src}') {
    throw "Source enemy defence engine removes enemy_def_src, but the live-unit cap counts it"
}
if (Select-String -Quiet -LiteralPath $defSource -SimpleMatch '{var "id_attack_support$"}') {
    throw "Source enemy defence engine reaches into the attack-support owner var; it must own to id_1st_enemy$"
}
# Every trigger must carry the attack-mission gate, or the system fires on a
# human-DEFENCE mission and reinforces the wrong side.
$defText = [System.IO.File]::ReadAllText($defSource)
$defTriggers = [regex]::Matches($defText, '\{"enemy_defense/[a-z0-9_]+"')
if ($defTriggers.Count -ne 19) {
    throw "Expected 19 enemy_defense triggers, found $($defTriggers.Count)"
}
$defGates = [regex]::Matches($defText, [regex]::Escape('{var "user_is_defender$"} {op "=="} {value 0}'))
if ($defGates.Count -lt 19) {
    throw "Only $($defGates.Count) of the 19 enemy_defense triggers carry the user_is_defender$ == 0 gate"
}
# 160 prototypes: 4 faction pools x (24 line + 16 weapons). A claim MOVES bodies
# out and never returns them, so each pool carries the whole L3 budget alone.
$defTplAble = (Select-String -LiteralPath $defTplSource -SimpleMatch '{Able "-select"}').Count
if ($defTplAble -ne 160) {
    throw "Source enemy defence pool must park 160 prototypes with selection stripped (4 factions x 40); found $defTplAble"
}
foreach ($pair in @(
    @('enemy_def_rusa_line', 24), @('enemy_def_rusa_wpn', 16),
    @('enemy_def_ukr_line', 24), @('enemy_def_ukr_wpn', 16),
    @('enemy_def_prc_line', 24), @('enemy_def_prc_wpn', 16),
    @('enemy_def_nato_line', 24), @('enemy_def_nato_wpn', 16)
)) {
    $n = (Select-String -LiteralPath $defTplSource -SimpleMatch ('"' + $pair[0] + '"')).Count
    if ($n -ne $pair[1]) {
        throw "Source enemy defence pool must tag $($pair[1]) prototypes as $($pair[0]); found $n"
    }
}
if (Select-String -Quiet -LiteralPath $defTplSource -Pattern '^\s*\{Human ""') {
    throw 'Enemy defence templates must use a real breed, not the breed-less empty-name Human form'
}
if (Select-String -Quiet -LiteralPath $defTplSource -Pattern '^\s*\{Inventory') {
    throw "Enemy defence templates must not bake an Inventory block - the breed supplies the loadout"
}
# No vehicles here: the defender bot already buys its own armour.
if (Select-String -Quiet -LiteralPath $defTplSource -Pattern '^\s*\{(Entity|Vehicle) ') {
    throw "Enemy defence templates must be infantry only - enemy armour comes from the purchase economy"
}
# Paths that do not exist in Code:X and would silently park 160 absent entities.
foreach ($banned in @('era1960', "$([char]0x65B0)$([char]0x5EFA)$([char]0x6587)$([char]0x4EF6)$([char]0x5939)")) {
    if (Select-String -Quiet -LiteralPath $defTplSource -SimpleMatch $banned) {
        throw "Enemy defence templates reference a non-existent breed path: $banned"
    }
}
# Parked in its own off-map band so it cannot collide with the attack-support pool.
if (Select-String -Quiet -LiteralPath $defTplSource -SimpleMatch '-35100}') {
    throw "Enemy defence templates park on the attack-support pool's y band (-35100)"
}
if (-not (Select-String -Quiet -LiteralPath $defTplSource -SimpleMatch '{Position -9000 -35400}')) {
    throw "Enemy defence templates are not parked in their own off-map band (y -35400, x from -9000)"
}
$defBreedRoot = Join-Path (Split-Path -Parent $WorkshopRoot) "3261086933\resource\set\breed\mp"
foreach ($breed in @(
    "rusa\2022s\rus90_squadlead", "rusa\2022s\rus90_rifleman", "rusa\2022s\rus90_mg",
    "rusa\2022s\rus90_seniorrifleman", "rusa\2022s\rus90_antitank", "rusa\2022s\rus90_marksman",
    "ukr\2022s\ter_squadlead", "ukr\2022s\ter_rifleman", "ukr\2022s\ter_mg",
    "ukr\2022s\ter_antitank", "ukr\2022s\ter_marksman",
    "prc\2022s\pla_squadlead", "prc\2022s\pla_senior", "prc\2022s\pla_rifleman",
    "prc\2022s\pla_mg", "prc\2022s\pla_antitank_pf98", "prc\2022s\pla_marksman",
    "nato\2022s\nato_squadlead", "nato\2022s\nato_teamlead", "nato\2022s\nato_rifleman",
    "nato\2022s\nato_mg", "nato\2022s\nato_antitank", "nato\2022s\nato_sniper"
)) {
    $breedSet = Join-Path $defBreedRoot ($breed + ".set")
    if (-not (Test-Path -LiteralPath $breedSet)) {
        throw "Enemy defence breed is not installed at: $breedSet"
    }
}

# ===== HUMAN-DEFENCE MISSION ENGINES (source side) =====
# Quadrants 2 and 3. Same hard-won pipeline as the attack-mission pair: no clone,
# bare pool selectors, literal {player} switch, {tag flag} capture points, and every
# trigger gated on user_is_defender$ == 1 so both are provably inert on an attack
# mission. Both also gate on prep_inform$ == 1, because a defence mission runs a real
# 480s preparation phase and nothing may deploy into it.
foreach ($marker in @(
    '{"defense_support/init"',
    '{"defense_support/clock"',
    '{"defense_support/hold_1"',
    '{"defense_support/hold_3"',
    '{"defense_support/comp_usmc"',
    '{"defense_support/comp_1ad"',
    '{"defense_support/comp_pzgd"',
    '{var "user_is_defender$"}',
    '{var "prep_inform$"}',
    '{var "id_defenderbot$"}',
    '{var "enemy_spawnside$"}',
    '{var "defense_level$"}',
    '{var "defense_support_wave_cmd$"}',
    '{var "defense_support_waves_left$"}',
    '{var "defense_support_group$"}',
    # Self-re-arming cadence and hold-group re-order ladders, both randomized. The
    # 115-230s clock shares no value with enemy_attack_support.inc's 125-280s clock,
    # which is the only other engine live on the same mission, and none with its own
    # 90-150s hold ladder either - a shared value lets wave arrivals drift into phase
    # with the hold redistribution. The first bucket is 115, not 110, for that reason.
    '{"trigger" {name "defense_support/clock"}}',
    '{"trigger" {name "defense_support/hold_1"}}',
    '{"delay" {time 115}}',
    '{"delay" {time 230}}',
    '{"delay" {time 90}}',
    '{"delay" {time 150}}',
    # Reinforcements enter at the DEFENDER's own edge, which is the side the enemy is
    # NOT on: for this engine enemy_spawnside$ 1 means side b.
    # The numbered pads, not the bare legacy name: placements round-robin across the
    # triple and only the patrol / roam move orders still address the alias.
    '{target_waypoint "attack_support_rear_a1"}',
    '{target_waypoint "attack_support_rear_b1"}',
    # They advance on the claimed ACTIVE flags and then dig in. This is a defence.
    '{tag_add def_sup_af1}',
    '{target {ignore_captured_by_user 0} {tag def_sup_af1}}',
    '{"actor_to_cover"',
    '{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}',
    # Live cap defers without consuming a wave, exactly as attack support does.
    '{tag def_sup_src}',
    '{state "not dead"}',
    '{count {op ">"} {value 14}}',
    'DEFENSE SUPPORT NEAR CAP DEFER',
    'DEFENSE SUPPORT WAVES EXHAUSTED',
    'DEFENSE SUPPORT POOL SHORT - FACTION LINE',
    'DEFENSE SUPPORT POOL EXHAUSTED',
    # Shared NATO pool: a claim strips the pool tag it took from.
    '{tag_remove attack_support_inf_usmc}',
    '{tag_remove attack_support_inf_1ad}',
    '{tag_remove attack_support_inf_pzgd}',
    '{tag_remove attack_support_inf_arf}',
    '{tag_remove attack_support_tpl}',
    # Shared player-nation pools, claimed the same way. The garrison is line-or-recon
    # only and the whole engine is vehicle-free: vehicles are attack-only.
    '{var "faction_support_army$"}',
    '("ds_resolve_army")',
    '("ds_pick_garrison")',
    '("ds_pick_hybrid_non_nato")',
    '{"defense_support/garrison_init"',
    '{"defense_support/ally_rusa_line"',
    '{"defense_support/ally_nato_manpad"',
    '{tag_remove ally_sup_rusa_line}',
    '{tag_remove ally_sup_prc_recon}',
    # Capture points are addressed as {tag flag}; fpc* is absent from outback.
    '{select {tag {tag flag}}}',
    # Bare deploy selector - decorating it zeroes the match on these units.
    '{group {select {tag {tag def_sup_deploy}}}}',
    # The owner is NOT guessed. An unresolved defender bot transfers nothing.
    'DEFENSE SUPPORT OWNER UNRESOLVED - NO TRANSFER',
    '{var "defense_support_owner_fail$"}',
    'DEFENSE SUPPORT OWNER - SLOT 1',
    '("ds_place_at_entry")',
    '("ds_own_to_defenderbot")',
    '("ds_report_owner")',
    '("ds_claim_anchors")',
    '("ds_assign_group")',
    '("ds_finish")',
    '("ds_pick_composition")'
)) {
    if (-not (Select-String -Quiet -LiteralPath $dsSource -SimpleMatch $marker)) {
        throw "Source defence support engine is missing marker: $marker"
    }
}
foreach ($marker in @(
    '{"enemy_attack/init"',
    '{"enemy_attack/clock"',
    '{"enemy_attack/rusa_line"',
    '{"enemy_attack/nato_wpn"',
    '{var "user_is_defender$"}',
    '{var "prep_inform$"}',
    '{var "id_1st_enemy$"}',
    '{var "enemy_spawnside$"}',
    '{var "bot_army$"}',
    '{var "defense_level$"}',
    '{var "enemy_attack_army$"}',
    '{var "enemy_attack_wave_cmd$"}',
    '{var "enemy_attack_waves_left$"}',
    '{"trigger" {name "enemy_attack/clock"}}',
    '{"delay" {time 125}}',
    '{"delay" {time 280}}',
    # Attacker pressure enters at the ATTACKER's own edge: enemy_spawnside$ 1 is
    # side a here, the same reading the enemy-defence engine uses.
    # The numbered pads, not the bare legacy name: placements round-robin across the
    # triple and only the patrol / roam move orders still address the alias.
    '{target_waypoint "attack_support_entry_a1"}',
    '{target_waypoint "attack_support_entry_b1"}',
    '{tag_add ea_flag1}',
    '{target {ignore_captured_by_user 0} {tag ea_flag1}}',
    '{ai {no_retreat on} {advance_ratio 1} {retreat_ratio 0}}',
    '{tag ea_src}',
    '{state "not dead"}',
    '{count {op ">"} {value 16}}',
    'ENEMY ATTACK NEAR CAP DEFER',
    'ENEMY ATTACK WAVES EXHAUSTED',
    'ENEMY ATTACK POOL SHORT - LINE TEAM INSTEAD',
    'ENEMY ATTACK POOL EXHAUSTED',
    'ENEMY ATTACK OWNER UNRESOLVED - NO TRANSFER',
    '{var "enemy_attack_owner_fail$"}',
    # Shared enemy-defence faction pools: a claim strips the pool tag it took from.
    '{tag_remove enemy_def_rusa_line}',
    '{tag_remove enemy_def_ukr_line}',
    '{tag_remove enemy_def_prc_line}',
    '{tag_remove enemy_def_nato_line}',
    '{tag_remove enemy_def_rusa_wpn}',
    '{tag_remove enemy_def_nato_wpn}',
    '{tag_remove enemy_def_tpl}',
    '{select {tag {tag flag}}}',
    '{group {select {tag {tag ea_deploy}}}}',
    '("ea_place_at_entry")',
    '("ea_own_to_enemy")',
    '("ea_resolve_army")',
    '("ea_finish")',
    '("ea_poke_line")',
    '("ea_poke_wpn")',
    '("ea_pick_wave")'
)) {
    if (-not (Select-String -Quiet -LiteralPath $eaSource -SimpleMatch $marker)) {
        throw "Source enemy attack engine is missing marker: $marker"
    }
}
foreach ($pair in @(@($dsSource, 'defence support'), @($eaSource, 'enemy attack'))) {
    $path = $pair[0]
    $label = $pair[1]
    foreach ($n in 1..16) {
        if (-not (Select-String -Quiet -LiteralPath $path -SimpleMatch ('{player "' + $n + '"}'))) {
            throw "Source $label engine is missing literal ownership case for player $n. The engine will not accept a var in the {player} node, so all sixteen slots must be spelled out"
        }
    }
    # The same pipeline constraints that cost the attack-support engine a live run each.
    foreach ($banned in @('{clone}', '{include {prop human}}', '{prop {prop human}}', '{state {state operatable}}', '{zone {zone "gamezone"}}', '{player "0"}')) {
        if (Select-String -Quiet -LiteralPath $path -SimpleMatch $banned) {
            throw "Source $label engine uses the forbidden idiom $banned"
        }
    }
    if (Select-String -Quiet -LiteralPath $path -Pattern '^[^;]*\bfpc') {
        throw "Source $label engine still targets fpc* capture points. Those tags are absent from outback entirely; address capture points as {tag flag}"
    }
    # Nothing from the retired allied-support experiment may be resurrected here.
    if (Select-String -Quiet -LiteralPath $path -SimpleMatch 'allied_support') {
        throw "Source $label engine references the retired allied-support experiment"
    }
    # Every trigger must carry the defence-mission gate, or the system fires on a
    # human-ATTACK mission and reinforces the wrong side.
    $text = [System.IO.File]::ReadAllText($path)
    $triggers = [regex]::Matches($text, '\{"(?:defense_support|enemy_attack)/[a-z0-9_]+"')
    $gates = [regex]::Matches($text, [regex]::Escape('{var "user_is_defender$"} {op "=="} {value 1}'))
    if ($triggers.Count -lt 8) {
        throw "Source $label engine declares only $($triggers.Count) triggers"
    }
    if ($gates.Count -lt $triggers.Count) {
        throw "Only $($gates.Count) of the $($triggers.Count) $label triggers carry the user_is_defender$ == 1 gate"
    }
    # A defence mission has a real 480s prep phase. Both init triggers must wait for
    # it, or waves land on top of the player's own placement.
    if (-not (Select-String -Quiet -LiteralPath $path -SimpleMatch '{var "prep_inform$"} {op "=="} {value 1}')) {
        throw "Source $label engine does not gate on prep_inform$ == 1, so it deploys during the defence preparation phase"
    }
    # These two engines park no prototypes of their own; they claim from the pools the
    # attack-mission engines own. A template include here would double-park them.
    # Case-sensitive: Phase-4 flag props use lowercase {entity "…"} inside {"spawn"},
    # which is runtime spawn — not parked {Human}/{Entity} template blocks.
    if (Select-String -Quiet -LiteralPath $path -CaseSensitive -Pattern '^\s*\{(Human|Entity|Vehicle) ') {
        throw "Source $label engine declares entities. It must claim from the existing parked pools, not park its own"
    }
}
# Neither engine may read the other's state, even though both are live on the same
# mission and each shares prototypes with an attack-mission engine. Checked on the
# comment-stripped view: the headers name every neighbouring system on purpose.
$dsCode = Get-MiCode $dsSource
$eaCode = Get-MiCode $eaSource
foreach ($banned in @('enemy_attack_', 'enemy_def_', 'id_1st_enemy', 'attack_support_src', 'attack_support_deploy')) {
    if ($dsCode.Contains($banned)) {
        throw "Source defence support engine reaches into other-system state: $banned"
    }
}
foreach ($banned in @('defense_support_', 'def_sup_', 'enemy_defense_', 'id_defenderbot', 'id_attack_support', 'enemy_def_src', 'enemy_def_deploy')) {
    if ($eaCode.Contains($banned)) {
        throw "Source enemy attack engine reaches into other-system state: $banned"
    }
}
# The shared pools are read as claims, and nothing else from those systems is touched.
foreach ($pool in @('attack_support_inf_usmc', 'attack_support_inf_1ad', 'attack_support_inf_pzgd')) {
    if (-not $dsCode.Contains($pool)) {
        throw "Source defence support engine does not claim the shared NATO pool: $pool"
    }
}
foreach ($faction in @('rusa', 'ukr', 'prc', 'nato')) {
    foreach ($role in @('line', 'wpn')) {
        $pool = "enemy_def_" + $faction + "_" + $role
        if (-not $eaCode.Contains($pool)) {
            throw "Source enemy attack engine does not claim the shared faction pool: $pool"
        }
    }
}
# The two live-on-the-same-mission cadences must not share a delay value, or the
# friendly and hostile arrivals drift into phase with each other.
$dsDelays = [regex]::Matches([System.IO.File]::ReadAllText($dsSource), '\{"delay" \{time (\d+)\}\}') | ForEach-Object { [int]$_.Groups[1].Value } | Where-Object { $_ -ge 60 }
$eaDelays = [regex]::Matches([System.IO.File]::ReadAllText($eaSource), '\{"delay" \{time (\d+)\}\}') | ForEach-Object { [int]$_.Groups[1].Value } | Where-Object { $_ -ge 60 }
$shared = @($dsDelays | Where-Object { $eaDelays -contains $_ })
if ($shared.Count -ne 0) {
    throw "The defence-mission engines share cadence values ($($shared -join ', ')), so friendly and hostile waves would synchronise"
}

foreach ($marker in @(
    '{Human "mp/nato/2022s/usmc_rifleman" 0xaf23',
    '{Human "mp/nato/2022s/1ad_rifleman" 0xaf37',
    '{Human "mp/nato/2022s/pzgd_rifleman" 0xaf4c',
    '{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf23}',
    '{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf37}',
    '{Tags "attack_support_tpl" "attack_support_inf_pzgd" "hidden" 0xaf4c}',
    # Every humvee is crewed by explicit links so it arrives drivable with the M2HB
    # manned, and each instance carries its own tag: they deploy one at a time
    # (placed together they clip and flip) and a crew is bound to one hull, so an
    # instance can only ever move as a whole.
    '{Entity "humvee_m2hb_usa" 0xaf54',
    '{Entity "humvee_m2hb_usa" 0xaf5d',
    '{Link 0xaf55 {0xaf54 "driver"}}',
    '{Link 0xaf56 {0xaf54 "gunner2"}}',
    '"attack_support_hmmwv1"',
    '"attack_support_hmmwv2"',
    '"attack_support_hmmwv3"',
    '"attack_support_hmmwv4"'
)) {
    if (-not (Select-String -Quiet -LiteralPath $tplSource -SimpleMatch $marker)) {
        throw "Source template pool is missing marker: $marker"
    }
}
# 64 prototypes. A wave MOVES pool originals out and never returns them, so the pool
# has to carry the whole level budget (L3 = 8 waves) across every composition it can
# draw, or a late wave silently deploys nothing.
$tplAbleCount = (Select-String -LiteralPath $tplSource -SimpleMatch '{Able "-select"}').Count
if ($tplAbleCount -ne 84) {
    throw "Source template pool must park 84 prototypes with selection stripped (20 USMC + 20 1AD + 20 ARF + 12 pzgren infantry plus 4 crewed humvees); found $tplAbleCount"
}
foreach ($pair in @(
    @('attack_support_inf_usmc', 20),
    @('attack_support_inf_1ad', 20),
    @('attack_support_inf_pzgd', 12),
    @('attack_support_inf_arf', 20)
)) {
    $n = (Select-String -LiteralPath $tplSource -SimpleMatch ('"' + $pair[0] + '"')).Count
    if ($n -ne $pair[1]) {
        throw "Source template pool must tag $($pair[1]) prototypes as $($pair[0]); found $n"
    }
}
# This pool shares every resolved map with enemy_defense_templates.inc, whose MID band
# opens at 9100. The ARF block first shipped as 9084..9103 and duplicated four MIDs in
# all fourteen maps, so the whole file has to stay strictly below that band.
$tplMids = [regex]::Matches((Get-Content -Raw -LiteralPath $tplSource), '\{MID (\d+)\}') |
    ForEach-Object { [int]$_.Groups[1].Value }
if ($tplMids.Count -ne 84) {
    throw "Source template pool must carry 84 MIDs; found $($tplMids.Count)"
}
if (($tplMids | Sort-Object -Unique).Count -ne 84) {
    throw "Source template pool has duplicate MIDs"
}
$tplMidMax = ($tplMids | Measure-Object -Maximum).Maximum
if ($tplMidMax -ge 9100) {
    throw "Source template pool MID $tplMidMax runs into the enemy-defence band at 9100"
}

# Player-nation pools: 502 prototypes across four factions (incl. rare IFV packages). Depths are per faction and
# each is shared by the attack and defence engines, which never run on the same mission,
# so each only has to cover ONE engine's L3 budget of 8 waves.
$factionAble = (Select-String -LiteralPath $factionTplSource -SimpleMatch '{Able "-select"}').Count
if ($factionAble -ne 502) {
    throw "Faction pool must park 502 prototypes with selection stripped; found $factionAble"
}
foreach ($faction in @('rusa', 'ukr', 'prc', 'nato')) {
    foreach ($pair in @(
        @('line', 24), @('wpn', 16), @('recon', 15),
        @('assault', 16), @('eng', 12), @('manpad', 8)
    )) {
        $tag = 'ally_sup_' + $faction + '_' + $pair[0]
        $n = (Select-String -LiteralPath $factionTplSource -SimpleMatch ('"' + $tag + '"')).Count
        if ($n -ne $pair[1]) {
            throw "Faction pool must tag $($pair[1]) prototypes as $tag; found $n"
        }
    }
}
# Light vehicles exist for Ukraine and NATO only, and are attack-only at the wave layer.
foreach ($pair in @(@('ally_sup_ukr_veh', 9), @('ally_sup_nato_veh', 6))) {
    $n = (Select-String -LiteralPath $factionTplSource -SimpleMatch ('"' + $pair[0] + '"')).Count
    if ($n -ne $pair[1]) {
        throw "Faction pool must tag $($pair[1]) prototypes as $($pair[0]); found $n"
    }
}
foreach ($faction in @('rusa', 'prc')) {
    if (Select-String -Quiet -LiteralPath $factionTplSource -SimpleMatch ('"ally_sup_' + $faction + '_veh"')) {
        throw "Faction $faction must not park a vehicle pool"
    }
}
# Same idiom bans as every other pool, plus the band check against both neighbours.
foreach ($banned in @('{clone}', '{include {prop human}}', '{state {state operatable}}', 'allied_support')) {
    if (Select-String -Quiet -LiteralPath $factionTplSource -SimpleMatch $banned) {
        throw "Faction pool uses the forbidden idiom $banned"
    }
}
if (Select-String -Quiet -LiteralPath $factionTplSource -Pattern '^\s*\{Human ""') {
    throw "Faction pool must use real breeds, never breed-less {Human ''}"
}
$factionMids = [regex]::Matches((Get-Content -Raw -LiteralPath $factionTplSource), '\{MID (\d+)\}') |
    ForEach-Object { [int]$_.Groups[1].Value }
if (($factionMids | Sort-Object -Unique).Count -ne 502) {
    throw "Faction pool must carry 502 unique MIDs; found $(($factionMids | Sort-Object -Unique).Count)"
}
if (($factionMids | Measure-Object -Minimum).Minimum -lt 9300) {
    throw "Faction pool MIDs must start at 9300, clear of the other two pools"
}
# Line-anchored so the header's prose about {Inventory} does not trip this.
if (Select-String -Quiet -LiteralPath $tplSource -Pattern '^\s*\{Inventory') {
    throw "Templates must not bake an Inventory block - the breed supplies the loadout"
}
# conquest.lua is tracked by this toolchain now. ensureAttackPrepInform must stay
# defined above OnGameQuant (a call above the definition resolves to a nil global
# and crashes the bot on its first quant), and IssueScatterOrder marks the adopted
# external scatter-order work.
foreach ($marker in @(
    'local function ensureAttackPrepInform',
    'local function IssueScatterOrder',
    'local function ScheduleSpawnOrderNudge',
    'local function publishEnemySpawnSide',
    'BotApi.Scene:SetVar("enemy_spawnside"',
    'Context.SpawnSeekTimer = Context.SpawnSeekTimer or {}',
    # ensureAttackPrepInform's early return. botDefender is THIS BOT's role - the line
    # right above it writes user_is_defender as `botDefender and 0 or 1`, and
    # OnPrepTimeOver's "when player was defending, bot is attacker" branch keys on
    # `not botDefender`. So the human-ATTACK case, which is what this function exists
    # for because those missions never raise PrepTimeOver, is botDefender == true.
    'if not botDefender then return end'
)) {
    if (-not (Select-String -Quiet -LiteralPath $conquestSource -SimpleMatch $marker)) {
        throw "Source conquest.lua is missing marker: $marker"
    }
}
# The inverted form published prep_inform on the first quant of every human-DEFENCE
# mission. That made prep read as already over at t=0: it fired dcg_script's
# dcg2/userdefend/prep_end during the player's own placement, and it would let both
# defence-mission wave engines deploy into the preparation phase they gate on.
if (Select-String -Quiet -LiteralPath $conquestSource -SimpleMatch 'if botDefender then return end') {
    throw "Source conquest.lua still carries the inverted ensureAttackPrepInform gate, which publishes prep_inform at t=0 on a defence mission"
}
$conquestText = [System.IO.File]::ReadAllText($conquestSource)
foreach ($pair in @(
    @('local function ensureAttackPrepInform', 'function OnGameQuant'),
    @('local function IssueScatterOrder', 'IssueScatterOrder(squad, flags'),
    @('local function ScheduleSpawnOrderNudge', 'ScheduleSpawnOrderNudge(squad)'),
    @('local function publishEnemySpawnSide', 'publishEnemySpawnSide()')
)) {
    $defAt = $conquestText.IndexOf($pair[0])
    $useAt = $conquestText.IndexOf($pair[1])
    if ($defAt -lt 0 -or $useAt -lt 0 -or $defAt -ge $useAt) {
        throw "Source conquest.lua defines '$($pair[0])' after its use '$($pair[1])' - Lua resolves that to a nil global and crashes the bot"
    }
}
if (Select-String -Quiet -LiteralPath $tplSource -Pattern '^\s*\{Human ""') {
    throw 'Templates must use a real breed, not the breed-less empty-name Human form'
}
# The pool has no breeds of its own; every prototype resolves against the base
# mod's breed tree, so a missing install means 64 silently absent entities.
$breedRoot = Join-Path (Split-Path -Parent $WorkshopRoot) "3261086933\resource\set\breed\mp\nato\2022s"
foreach ($breed in @("usmc_rifleman", "1ad_rifleman", "pzgd_rifleman", "usarmy_crew")) {
    $breedSet = Join-Path $breedRoot ($breed + ".set")
    if (-not (Test-Path -LiteralPath $breedSet)) {
        throw "Breed mp/nato/2022s/$breed is not installed at: $breedSet"
    }
}

Write-Host "Deploying attack support wave engine (opening wave 30-45s, then randomized 120-240s cadence, level-scaled composition pools)"
Write-Host "  plus the enemy defence engine (garrison on the live flags, four patrol groups, 45-90s trickle + 180-300s surge off the defender's own edge)"
Write-Host "  plus the two human-DEFENCE mission engines, both behind prep_inform: defensive support for the defender bot (25-40s opening, 115-230s cadence, hold groups on the active flags) and enemy attacker pressure (65-95s opening, 125-280s cadence, assault on the player's flags)"
Write-Host "  plus the player-nation pools in faction_support_templates.inc: the friendly waves follow the PLAYER's faction (user_nation$ folded to rusa/ukr/nato/prc), with recon, assault, engineer and MANPAD teams gated by campaign level and light vehicles on attack missions only"
Write-Host "  and retiring the allied-support experiment: both .inc files removed, includes and entry waypoint stripped from all fourteen maps"
Write-Host "Repository: $RepoRoot"
Write-Host "Branch:     $branch"
Write-Host "Workshop:   $WorkshopRoot"

foreach ($relative in $files) {
    $source = Join-Path $RepoRoot $relative
    $target = Join-Path $WorkshopRoot $relative

    New-Item -ItemType Directory -Force -Path (Split-Path $target) | Out-Null
    Copy-Item -LiteralPath $source -Destination $target -Force

    $sourceHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $source).Hash
    $targetHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $target).Hash
    if ($sourceHash -ne $targetHash) {
        throw "Hash mismatch after copying: $relative"
    }

    Write-Host "OK $relative $sourceHash"
}

# Files this toolchain used to ship and no longer does. Left behind they are dead
# weight at best; attack_support_probe.inc in particular is the pre-rename wave
# engine, and a map that still included it would load two wave engines at once.
foreach ($orphan in @(
    "resource\map\multi\attack_support_probe.inc",
    "resource\map\multi\allied_attack_waves.inc",
    "resource\script\multiplayer\modes\attack_support_brain.lua",
    # The retired allied-support experiment. It predates the no-clone discovery: it
    # cloned breed-less {Human ""} prototypes with cleared inventories, sized waves off
    # a defence_level switch and drove them with an FSM, and none of it was ever proven
    # to put a body on the map. defense_support_waves.inc replaces it entirely, so both
    # files go and their includes are stripped from every map below.
    "resource\map\multi\allied_support_waves.inc",
    "resource\map\multi\allied_support_templates.inc"
)) {
    $orphanPath = Join-Path $WorkshopRoot $orphan
    if (Test-Path -LiteralPath $orphanPath) {
        Remove-Item -LiteralPath $orphanPath -Force
        Write-Host "REMOVED ORPHAN $orphan"
    }
}

$mapRoot = Join-Path $WorkshopRoot "resource\map\multi"
$mapFiles = @(
    Get-ChildItem -LiteralPath $mapRoot -Directory |
        Where-Object { $_.Name -match '^dcg_\[cwa71\]_' } |
        ForEach-Object { Join-Path $_.FullName "campaign_capture_the_flag.mi" } |
        Where-Object { Test-Path -LiteralPath $_ }
)
if ($mapFiles.Count -ne 14) {
    throw "Expected 14 CWA campaign_capture_the_flag.mi files, found $($mapFiles.Count)"
}

# The retired allied-support includes were the insertion anchors for the two
# attack-support includes, and a pristine base map has nothing else in either place to
# anchor on. So they are not deleted, they are CONVERTED: a map that still carries a
# legacy include has it rewritten into its live replacement in situ, which both retires
# the experiment and preserves the exact position everything downstream anchors on. A
# map that already carries the live include just loses the legacy line.
$legacyConversions = @(
    @('(include "../allied_support_waves.inc")', '(include "../attack_support_waves.inc")'),
    @('(include "../allied_support_templates.inc")', '(include "../attack_support_templates.inc")')
)
$wavesInclude = '(include "../attack_support_waves.inc")'
$tplInclude = '(include "../attack_support_templates.inc")'
# Player-nation prototype pools, shared by the attack-support and defence-support
# engines. Sits between the two NATO pools in the entities section. The name matters:
# the retired experiment was called allied_support_*, and that substring is banned
# outright by the guards below, so this file must never be renamed back into it.
$factionTplInclude = '(include "../faction_support_templates.inc")'
# Enemy-defence half. Each sits immediately after its attack-support counterpart:
# the engine in the triggers section, the prototype pool in the entities section.
$defInclude = '(include "../enemy_defense_support.inc")'
$defTplInclude = '(include "../enemy_defense_templates.inc")'
$flagPropsTplInclude = '(include "../flag_props_templates.inc")'
# The two human-DEFENCE mission engines, in the triggers section behind the two
# attack-mission ones. Neither has a templates include: defence support claims from the
# attack-support pool and enemy attack from the enemy-defence pools, and the engines
# that own those pools are inert on a defence mission, so there is no contention and
# nothing extra to park.
$dsInclude = '(include "../defense_support_waves.inc")'
$eaInclude = '(include "../enemy_attack_support.inc")'
# The shared engine-state declaration. Thirteen maps ship it from the base pack;
# border ships an inline eleven-var block instead, which leaves every engine gate
# reading a silent zero, so the conversion step below rewrites it into this include.
$varsInclude = '(include "../dcg_vars.inc")'
$waypointsAnchor = "`t`t{waypoints"
$entryName = '{"attack_support_entry_'
# Side of the entry triangle, in map units. Map coordinates are decimetres on this
# family: a pair of sandbag heaps ~5m apart sits ~52 units apart, the entry pads carry
# {radius 150} (15m), and the maps span ~19000 units (~1.9km). So 270 is ~27m - wider
# than a pad radius, so two consecutive batches cannot overlap, and a rounding error
# against a 1.9km map. Change it here and every deployed map is regenerated on the next
# run; the repo maps keep only the single centroid the triple is derived from.
$EntrySpacing = 270.0
# Flank pads (Phase 2): depth toward map centre from side centroid, lateral spread
# as a fraction of the perpendicular spawn-line extent (approx via centroid length).
# Defence reinforcements form up BEHIND the defender's own spawn line rather than
# on it: rear pads sit RearFactor x the spawn centroid, further from the objectives
# than the troops already holding them. 1.00 would collapse them onto the entry pads.
$RearFactor = 1.15
$FlankDepth = 0.50
$FlankSpread = 0.35
# Airmobile LZ pads (Phase 5 E1): deeper toward centre than flanks.
$AirDepth = 0.65
# Name kept from the probe era on purpose: it holds the genuinely pristine
# pre-patch maps, and the "already backed up" check below is what stops a rerun
# from overwriting them with maps this script has already patched.
$backupRoot = Join-Path $WorkshopRoot "_attack_support_probe_backups"
# Running total for the summary line: one ammo supply point per flag across the family.
$flagAmmoTotal = 0

foreach ($mapFile in $mapFiles) {
    # Legacy cleanup for maps written by an earlier deploy. Those files no longer
    # exist, so an inherited include line is a dangling reference the engine cannot
    # resolve - and attack_support_probe.inc is the pre-rename wave engine, which
    # would run a second, conflicting schedule if it survived. Strip the old include
    # lines outright; the current includes are then (re)added by the blocks below.
    $text = [System.IO.File]::ReadAllText($mapFile)
    $legacyBefore = $text
    foreach ($legacyInclude in @(
        '(include "../attack_mate_retask_probe.inc")',
        '(include "../attack_mate_probe_templates.inc")',
        '(include "../attack_support_probe.inc")'
    )) {
        $text = [regex]::Replace($text, '\s*' + [regex]::Escape($legacyInclude), '')
    }
    if ($text -ne $legacyBefore) {
        [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))
        Write-Host "LEGACY-STRIPPED superseded includes from $mapFile"
    }

    # Retire the allied-support experiment. Back the map up FIRST if this is the first
    # time this toolchain has touched it - a pristine map is the only thing that can
    # still be restored to base behaviour.
    $legacyBefore = $text
    $carriesLegacy = $false
    foreach ($pair in $legacyConversions) {
        if ($text.Contains($pair[0])) { $carriesLegacy = $true }
    }
    if ($carriesLegacy) {
        $relativeMap = $mapFile.Substring($WorkshopRoot.Length).TrimStart('\')
        $backup = Join-Path $backupRoot $relativeMap
        if (-not (Test-Path -LiteralPath $backup)) {
            New-Item -ItemType Directory -Force -Path (Split-Path $backup) | Out-Null
            Copy-Item -LiteralPath $mapFile -Destination $backup -Force
        }
        foreach ($pair in $legacyConversions) {
            $legacy = $pair[0]
            $live = $pair[1]
            if (-not $text.Contains($legacy)) { continue }
            if ($text.Contains($live)) {
                # Already patched by an earlier deploy: just drop the legacy line.
                $text = [regex]::Replace($text, '[ \t]*' + [regex]::Escape($legacy) + '\r?\n', '')
            } else {
                # Pristine map: convert the legacy include into its replacement in place.
                $text = $text.Replace($legacy, $live)
            }
        }
        # The waypoint the retired engine cloned into. Nothing reads it any more.
        $text = [regex]::Replace(
            $text,
            '[ \t]*\{"allied_support_entry"\r?\n[ \t]*\{position [^}]*\}\r?\n[ \t]*\{radius \d+\}\r?\n[ \t]*\}\r?\n',
            ''
        )
        [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))
        Write-Host "LEGACY-STRIPPED retired allied support from $mapFile"
    }
    if ($text -match 'allied_support') {
        throw "Map still references the retired allied-support experiment: $mapFile"
    }

    $wavesCount = ([regex]::Matches($text, [regex]::Escape($wavesInclude))).Count
    if ($wavesCount -ne 1) {
        throw "Expected exactly one wave-engine include in: $mapFile (found $wavesCount)"
    }
    $tplCount = ([regex]::Matches($text, [regex]::Escape($tplInclude))).Count
    if ($tplCount -ne 1) {
        throw "Expected exactly one wave-templates include in: $mapFile (found $tplCount)"
    }

    # Enemy-defence engine, in the triggers section right after the attack-support
    # engine. Idempotent, and anchored on the attack-support include rather than a
    # base-game line so the two halves always sit together.
    $text = [System.IO.File]::ReadAllText($mapFile)
    $defCount = ([regex]::Matches($text, [regex]::Escape($defInclude))).Count
    if ($defCount -eq 0) {
        if (-not $text.Contains($wavesInclude)) {
            throw "Map is missing the enemy-defence include anchor: $mapFile"
        }
        $text = $text.Replace($wavesInclude, $wavesInclude + "`r`n`t`t`t" + $defInclude)
        [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))
        $defCount = 1
    }
    if ($defCount -ne 1) {
        throw "Expected exactly one enemy-defence include in: $mapFile"
    }

    # Player-nation prototype pools, in the entities section right after the
    # attack-support pool and ahead of the enemy-defence pool. Injected before the
    # enemy-defence pool below so the three always land in a fixed order even on a
    # map that has none of them yet. Idempotent.
    $text = [System.IO.File]::ReadAllText($mapFile)
    $factionTplCount = ([regex]::Matches($text, [regex]::Escape($factionTplInclude))).Count
    if ($factionTplCount -eq 0) {
        if (-not $text.Contains($tplInclude)) {
            throw "Map is missing the faction-pool templates include anchor: $mapFile"
        }
        $text = $text.Replace($tplInclude, $tplInclude + "`r`n`t" + $factionTplInclude)
        [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))
        $factionTplCount = 1
    }
    if ($factionTplCount -ne 1) {
        throw "Expected exactly one faction-pool templates include in: $mapFile"
    }

    # Enemy-defence prototype pool, in the entities section right after the
    # faction pools. Idempotent.
    $text = [System.IO.File]::ReadAllText($mapFile)
    $defTplCount = ([regex]::Matches($text, [regex]::Escape($defTplInclude))).Count
    if ($defTplCount -eq 0) {
        if (-not $text.Contains($factionTplInclude)) {
            throw "Map is missing the enemy-defence templates include anchor: $mapFile"
        }
        $text = $text.Replace($factionTplInclude, $factionTplInclude + "`r`n`t" + $defTplInclude)
        [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))
        $defTplCount = 1
    }
    if ($defTplCount -ne 1) {
        throw "Expected exactly one enemy-defence templates include in: $mapFile"
    }


    # Flag-prop prototypes (Phase 4), entities section after enemy-defence pool.
    $text = [System.IO.File]::ReadAllText($mapFile)
    $flagPropsTplCount = ([regex]::Matches($text, [regex]::Escape($flagPropsTplInclude))).Count
    if ($flagPropsTplCount -eq 0) {
        if (-not $text.Contains($defTplInclude)) {
            throw "Map is missing the flag-props templates include anchor: $mapFile"
        }
        $text = $text.Replace($defTplInclude, $defTplInclude + "`r`n`t" + $flagPropsTplInclude)
        [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))
        $flagPropsTplCount = 1
    }
    if ($flagPropsTplCount -ne 1) {
        throw "Expected exactly one flag-props templates include in: $mapFile"
    }

    # The two human-DEFENCE mission engines, in the triggers section behind the
    # enemy-defence engine so all four quadrants sit together in a fixed order.
    # Idempotent, and each anchored on the include immediately ahead of it.
    foreach ($pair in @(@($defInclude, $dsInclude), @($dsInclude, $eaInclude))) {
        $ahead = $pair[0]
        $add = $pair[1]
        $text = [System.IO.File]::ReadAllText($mapFile)
        $n = ([regex]::Matches($text, [regex]::Escape($add))).Count
        if ($n -eq 0) {
            if (-not $text.Contains($ahead)) {
                throw "Map is missing the include anchor $ahead for $add : $mapFile"
            }
            $text = $text.Replace($ahead, $ahead + "`r`n`t`t`t" + $add)
            [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))
            $n = 1
        }
        if ($n -ne 1) {
            throw "Expected exactly one $add in: $mapFile (found $n)"
        }
    }

    # Attack-side entry waypoints, THREE per spawn side. The dynamic campaign swaps
    # attacker/defender spawns per mission instance, so the engine picks the side at
    # runtime from enemy_spawnside$ - a single static entry is never right - and then
    # round-robins across that side's three pads, because consecutive batches landing on
    # one pad were dropping bodies on top of each other and killing them on arrival.
    # The per-map centroid lives in the repo copy of the map; the triple is derived from
    # it here, and nothing else about the geometry is recomputed.
    #
    # Each entry sits on its own side's spawn centroid - the map-edge spawn area -
    # derived from that map's spawn_a / spawn_b markers. The centroid multiplier is
    # the balance knob (0,0 is the map centre, so a factor below 1.0 pulls the
    # arrival point forward into open ground). It is currently 1.00; changing it
    # means regenerating all 28 waypoints in the repo maps from the spawn markers,
    # not editing anything in this script.
    $repoMap = Join-Path $RepoRoot ("resource\map\multi\" + (Split-Path -Leaf (Split-Path -Parent $mapFile)) + "\campaign_capture_the_flag.mi")
    if (-not (Test-Path -LiteralPath $repoMap)) {
        throw "No repo map to source the entry waypoints from: $repoMap"
    }
    $repoText = [System.IO.File]::ReadAllText($repoMap)

    # Self-healing and idempotent: strip every entry block first - the superseded
    # side-agnostic one written by earlier deploys, the pre-rename attack_mate_entry*
    # blocks, and the numbered triple this run is about to rebuild - then rebuild both
    # sides from the repo. Rewriting beats trying to reconcile whatever an interrupted
    # earlier run left behind. The name class has to admit digits or the triple written
    # by the previous run survives the strip and the rebuild doubles it.
    $text = [System.IO.File]::ReadAllText($mapFile)
    $text = [regex]::Replace(
        $text,
        '\s*\{"attack_(?:support|mate)_(?:entry|rear|flank|air)[a-z0-9_]*"\s*\r?\n\s*\{position [^}]*\}\s*\r?\n\s*\{radius \d+\}\s*\r?\n\s*\}',
        ''
    )
    if (-not $text.Contains($waypointsAnchor)) {
        throw "Map is missing the waypoints anchor: $mapFile"
    }
    # Sides in reverse, and points in reverse within a side: every block is inserted
    # directly after the anchor, so emitting backwards leaves a/a1/a2/a3/b/b1/b2/b3 in
    # reading order in the file.
    foreach ($side in @("b", "a")) {
        $wpMatch = [regex]::Match($repoText, '\{"attack_support_entry_' + $side + '"\s*\r?\n\s*\{position ([^}]*)\}\s*\r?\n\s*\{radius (\d+)\}\s*\r?\n\s*\}')
        if (-not $wpMatch.Success) {
            throw "Repo map is missing the attack_support_entry_$side waypoint block: $repoMap"
        }
        $coords = @($wpMatch.Groups[1].Value.Trim() -split '\s+')
        if ($coords.Count -lt 2) {
            throw "Unreadable position on attack_support_entry_$side in: $repoMap"
        }
        $cx = [double]$coords[0]
        $cy = [double]$coords[1]
        $cz = if ($coords.Count -ge 3) { [double]$coords[2] } else { 0.0 }
        $radius = [int]$wpMatch.Groups[2].Value

        # Unit vector from this entry toward the map centre (0,0 IS the centre), and the
        # lateral one along the map edge. A degenerate centroid falls back to a fixed
        # frame so the triple is still three distinct points.
        $len = [math]::Sqrt(($cx * $cx) + ($cy * $cy))
        if ($len -lt 1.0) {
            $vx = 0.0; $vy = 1.0
        } else {
            $vx = -$cx / $len; $vy = -$cy / $len
        }
        $ux = -$vy
        $uy = $vx

        # Point 1 is the centroid itself - the exact coordinate the legacy
        # attack_support_entry_<side> keeps - and points 2 and 3 complete an equilateral
        # triangle of side $EntrySpacing: one step along the edge, one step inward. All
        # three pairwise gaps are therefore $EntrySpacing, so no two consecutive wave
        # batches can be dropped close enough to crush each other on arrival.
        $tri = @(
            @(0.0, 0.0),
            @(($EntrySpacing * $ux), ($EntrySpacing * $uy)),
            @(($EntrySpacing * ((0.5 * $ux) + (0.8660254 * $vx))),
              ($EntrySpacing * ((0.5 * $uy) + (0.8660254 * $vy))))
        )
        foreach ($point in @(3, 2, 1)) {
            $offset = $tri[$point - 1]
            $px = $cx + $offset[0]
            $py = $cy + $offset[1]
            $name = 'attack_support_entry_' + $side + $point
            $block = "`r`n`t`t`t{`"$name`"`r`n`t`t`t`t{position " +
                ("{0:F2} {1:F2} {2:F2}" -f $px, $py, $cz) +
                "}`r`n`t`t`t`t{radius $radius}`r`n`t`t`t}"
            $text = $text.Replace($waypointsAnchor, $waypointsAnchor + $block)

            # Rear tier: the same triangle pushed out past the spawn line, used by the
            # defence engine so its reinforcements walk in from behind the line.
            $rx = ($cx * $RearFactor) + $offset[0]
            $ry = ($cy * $RearFactor) + $offset[1]
            $rname = 'attack_support_rear_' + $side + $point
            $rblock = "`r`n`t`t`t{`"$rname`"`r`n`t`t`t`t{position " +
                ("{0:F2} {1:F2} {2:F2}" -f $rx, $ry, $cz) +
                "}`r`n`t`t`t`t{radius $radius}`r`n`t`t`t}"
            $text = $text.Replace($waypointsAnchor, $waypointsAnchor + $rblock)
        }

        # Flank pads: midway toward map centre, offset left/right along the edge.
        # attack_support_flank_<side>1/2. Attack-support Q1 only.
        $fx = $cx * (1.0 - $FlankDepth)
        $fy = $cy * (1.0 - $FlankDepth)
        $spread = [math]::Max(400.0, $len * $FlankSpread)
        $flanks = @(
            @(($fx + $spread * $ux), ($fy + $spread * $uy)),
            @(($fx - $spread * $ux), ($fy - $spread * $uy))
        )
        foreach ($fp in @(2, 1)) {
            $off = $flanks[$fp - 1]
            $name = 'attack_support_flank_' + $side + $fp
            $block = "`r`n`t`t`t{`"$name`"`r`n`t`t`t`t{position " +
                ("{0:F2} {1:F2} {2:F2}" -f $off[0], $off[1], $cz) +
                "}`r`n`t`t`t`t{radius $radius}`r`n`t`t`t}"
            $text = $text.Replace($waypointsAnchor, $waypointsAnchor + $block)
        }


        # Airmobile LZ pads: deeper insert points (Phase 5 E1, narrative helo only).
        $ax = $cx * (1.0 - $AirDepth)
        $ay = $cy * (1.0 - $AirDepth)
        $aspread = [math]::Max(350.0, $len * 0.25)
        $airs = @(
            @(($ax + $aspread * $ux), ($ay + $aspread * $uy)),
            @(($ax - $aspread * $ux), ($ay - $aspread * $uy))
        )
        foreach ($ap in @(2, 1)) {
            $off = $airs[$ap - 1]
            $name = 'attack_support_air_' + $side + $ap
            $block = "`r`n`t`t`t{`"$name`"`r`n`t`t`t`t{position " +
                ("{0:F2} {1:F2} {2:F2}" -f $off[0], $off[1], $cz) +
                "}`r`n`t`t`t`t{radius $radius}`r`n`t`t`t}"
            $text = $text.Replace($waypointsAnchor, $waypointsAnchor + $block)
        }

        # The legacy single-pad name, kept as an alias of point 1 rather than migrated
        # away: the enemy-defence patrol and roam {action move} orders still address it,
        # and those are orders rather than placements, so they want one stable point.
        $block = "`r`n`t`t`t" + ($wpMatch.Value -replace '\r?\n', "`r`n")
        $text = $text.Replace($waypointsAnchor, $waypointsAnchor + $block)
    }
    [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))

    $text = [System.IO.File]::ReadAllText($mapFile)
    foreach ($side in @("a", "b")) {
        # The legacy alias, exactly once.
        $n = ([regex]::Matches($text, [regex]::Escape('{"attack_support_entry_' + $side + '"'))).Count
        if ($n -ne 1) {
            throw "Expected exactly one attack_support_entry_$side waypoint in: $mapFile (found $n)"
        }
        # And the triple, exactly three per side, one block each.
        foreach ($point in @(1, 2, 3)) {
            $n = ([regex]::Matches($text, [regex]::Escape('{"attack_support_entry_' + $side + $point + '"'))).Count
            if ($n -ne 1) {
                throw "Expected exactly one attack_support_entry_$side$point waypoint in: $mapFile (found $n)"
            }
        }
        $triple = ([regex]::Matches($text, '\{"attack_support_entry_' + $side + '[123]"')).Count
        if ($triple -ne 3) {
            throw "Expected exactly three entry pads on side $side in: $mapFile (found $triple)"
        }
    }
    
    foreach ($side in @("a", "b")) {
        foreach ($fp in @(1, 2)) {
            $n = ([regex]::Matches($text, [regex]::Escape('{"attack_support_flank_' + $side + $fp + '"'))).Count
            if ($n -ne 1) {
                throw "Expected exactly one attack_support_flank_$side$fp waypoint in: $mapFile (found $n)"
            }
        }
    }
    foreach ($side in @("a", "b")) {
        foreach ($ap in @(1, 2)) {
            $n = ([regex]::Matches($text, [regex]::Escape('{"attack_support_air_' + $side + $ap + '"'))).Count
            if ($n -ne 1) {
                throw "Expected exactly one attack_support_air_$side$ap waypoint in: $mapFile (found $n)"
            }
        }
    }

    if ([regex]::IsMatch($text, '\{"attack_support_entry"')) {
        throw "Map still carries the superseded single-sided attack_support_entry: $mapFile"
    }
    # Nothing from the pre-rename naming may survive in a deployed map.
    if ([regex]::IsMatch($text, 'attack_mate')) {
        throw "Map still carries pre-rename attack_mate naming: $mapFile"
    }
    # Waypoint "0" is the roam fallback the enemy-defence patrols use where a map has
    # no spare flag point, and it is base-game map geometry - losing it would silently
    # turn those branches into no-ops.
    if (-not [regex]::IsMatch($text, '\{"0"\s*\r?\n\s*\{position ')) {
        throw "Map lost its waypoint 0: $mapFile"
    }

    # Engine-state declaration. border is the one map in the family whose vars block
    # is inline rather than the shared dcg_vars.inc include; an undeclared MI var read
    # is a silent zero, so on that map user_is_defender$ failed Q2/Q3's == 1 gate and
    # the owner ids failed Q1/Q4's > 0 gate - all four wave engines inert. Convert the
    # inline block to the shared include, keeping "balance" - the one var dcg_vars.inc
    # does not declare - inline so nothing is ever declared twice. dcg_script.inc is
    # deliberately NOT added: border runs nikral's trigger set instead, and the
    # engines' gates are published from conquest.lua, which is map-agnostic. Idempotent:
    # a map that already carries the include skips straight to the count check.
    $varsCount = ([regex]::Matches($text, [regex]::Escape($varsInclude))).Count
    if ($varsCount -eq 0) {
        $inlineBlock = [regex]::Match(
            $text,
            '\{vars\r?\n(?:[ \t]*\{"[a-z0-9_]+"\}\r?\n)+[ \t]*\}'
        )
        if (-not $inlineBlock.Success) {
            throw "Map has neither the dcg_vars.inc include nor a recognisable inline vars block: $mapFile"
        }
        if (-not $inlineBlock.Value.Contains('{"balance"}')) {
            throw "Inline vars block is missing the map-local balance var: $mapFile"
        }
        $converted = "{vars`r`n`t`t`t$varsInclude`r`n`t`t`t{`"balance`"}`r`n`t`t}"
        $text = $text.Replace($inlineBlock.Value, $converted)
        [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))
        Write-Host "BORDER-VARS converted inline vars block in $mapFile"
        $varsCount = 1
    }
    if ($varsCount -ne 1) {
        throw "Expected exactly one dcg_vars.inc include in: $mapFile (found $varsCount)"
    }

    # Every include the four quadrants need, exactly once each, and nothing from the
    # retired allied-support experiment.
    $text = [System.IO.File]::ReadAllText($mapFile)
    foreach ($include in @($varsInclude, $tplInclude, $factionTplInclude, $defTplInclude, $flagPropsTplInclude, $wavesInclude, $defInclude, $dsInclude, $eaInclude)) {
        $n = ([regex]::Matches($text, [regex]::Escape($include))).Count
        if ($n -ne 1) {
            throw "Expected exactly one $include in: $mapFile (found $n)"
        }
    }
    if ([regex]::IsMatch($text, 'allied_support')) {
        throw "Map still references the retired allied-support experiment: $mapFile"
    }

    # Live ammo supply on every flag, deployed copy AND repo copy. The repo copy is
    # patched too because this is real map content rather than derived geometry: the
    # waypoint triples above are regenerated from a single repo centroid every run
    # and so deliberately live only in the workshop, but the supply points are the
    # same in both, and a repo map that lacked them would look like a regression to
    # anyone reading the tracked file. Anchor and flag set are identical in the two
    # copies, so both land byte-identically.
    $flagsPatched = Set-FlagAmmoSupply $mapFile "workshop"
    $null = Set-FlagAmmoSupply $repoMap "repo"
    $flagAmmoTotal += $flagsPatched

    Write-Host "PATCHED $mapFile"
}

$gameSet = Join-Path $WorkshopRoot $files[0]
$botMain = Join-Path $WorkshopRoot $files[1]
$support = Join-Path $WorkshopRoot $files[2]
$vars = Join-Path $WorkshopRoot $files[3]
$waves = Join-Path $WorkshopRoot $files[4]
$def = Join-Path $WorkshopRoot $files[6]
$defTpl = Join-Path $WorkshopRoot $files[7]
$conquest = Join-Path $WorkshopRoot $files[8]
$ds = Join-Path $WorkshopRoot $files[10]
$ea = Join-Path $WorkshopRoot $files[11]

if (-not (Select-String -Quiet -LiteralPath $gameSet -SimpleMatch "{aiTeamPlayers 1}")) {
    throw "Workshop game set does not contain the attack support AI slot marker"
}
foreach ($marker in @(
    'local function ensureAttackPrepInform',
    'local function IssueScatterOrder',
    # botDefender is THIS BOT's role, so the early return has to fire on the human
    # DEFENCE case (not botDefender). The inverted form published prep_inform on the
    # first quant of every defence mission, which made prep read as already over at
    # t=0 and would let the defence-mission wave engines deploy into the prep phase.
    'if not botDefender then return end'
)) {
    if (-not (Select-String -Quiet -LiteralPath $conquest -SimpleMatch $marker)) {
        throw "Workshop conquest.lua is missing marker: $marker"
    }
}
if (Select-String -Quiet -LiteralPath $botMain -SimpleMatch "first_player_slot") {
    throw "Workshop bot.main.lua still contains the regressed FirstPlayerId skip gate"
}
if (-not (Select-String -Quiet -LiteralPath $botMain -SimpleMatch "Never use FirstPlayerId to exclude a")) {
    throw "Workshop bot.main.lua is missing the FirstPlayerId lesson guard comment"
}
if (Select-String -Quiet -LiteralPath $botMain -SimpleMatch "team_a_attack_safe_route") {
    throw "Workshop bot.main.lua still contains the superseded crashing Team A route"
}
if (-not (Select-String -Quiet -LiteralPath $support -SimpleMatch 'sc:SetVar("attack_support_use_mi", 1)')) {
    throw "Workshop attack_support.lua does not arm MI delivery, so no attack support units will ever reach the map"
}
# The deployed copy is the one the game reads, so the mirror is verified there too.
foreach ($marker in $MirrorMarkers) {
    if (-not (Select-String -Quiet -LiteralPath $support -SimpleMatch $marker)) {
        throw "Workshop attack_support.lua is missing the engine-state mirror: $marker"
    }
}
$supportTargetCode = Get-LuaCode $support
foreach ($banned in $SlotUnsafe) {
    if ($supportTargetCode.Contains($banned)) {
        throw "Workshop attack_support.lua touches the slot-unsafe surface: $banned"
    }
}
foreach ($marker in @(
    '{"attack_support_armed"}',
    '{"attack_support_wave_cmd"}',
    '{"attack_support_wave_num"}',
    '{"attack_support_waves_left"}',
    '{"attack_support_next_ok"}',
    '{"attack_support_hmmwv_left"}',
    '{"attack_support_use_mi"}',
    '{"support_e2_test"}', '{"support_e2_stage"}', '{"support_e2_fail"}', '{"support_e2_lz"}', '{"support_e2_flag"}'
)) {
    if (-not (Select-String -Quiet -LiteralPath $vars -SimpleMatch $marker)) {
        throw "Workshop dcg_vars.inc is missing the attack support wave state: $marker"
    }
}
if (Select-String -Quiet -LiteralPath $vars -SimpleMatch 'allied_attack_enabled') {
    throw "Workshop dcg_vars.inc still declares the retired skeleton's enable var"
}
if (-not (Select-String -Quiet -LiteralPath $waves -SimpleMatch '{var "user_is_defender$"}')) {
    throw "Workshop wave engine is missing the attack-only gate"
}
if (-not (Select-String -Quiet -LiteralPath $waves -SimpleMatch '{var "attack_support_wave_cmd$"}')) {
    throw "Workshop wave engine is missing the composition command gate - without it every wave fires at once on entity presence"
}
foreach ($trigger in @(
    '{"attack_support/init"',
    '{"attack_support/clock"',
    '{"attack_support/comp_usmc"',
    '{"attack_support/comp_1ad"',
    '{"attack_support/comp_acav"',
    '{"attack_support/comp_pzgren"'
)) {
    if (-not (Select-String -Quiet -LiteralPath $waves -SimpleMatch $trigger)) {
        throw "Workshop wave engine is missing trigger: $trigger"
    }
}
if (Select-String -Quiet -LiteralPath $waves -SimpleMatch '{"delay" {time 8}}') {
    throw "Workshop wave engine still contains the superseded blind startup delay"
}
# Each composition gates on its own pool, so an exhausted pool is a no-op that falls
# back rather than an empty deploy.
foreach ($pool in @('attack_support_inf_usmc', 'attack_support_inf_1ad', 'attack_support_inf_pzgd')) {
    if (-not (Select-String -Quiet -LiteralPath $waves -SimpleMatch ('{selector {tag ' + $pool + '}}'))) {
        throw "Workshop wave engine does not gate a composition on its pool being stocked: $pool"
    }
}
if (-not (Select-String -Quiet -LiteralPath $waves -SimpleMatch '{"trigger" {name "attack_support/clock"}}')) {
    throw "Workshop wave engine does not re-arm its cadence clock, so it fires one wave and stops"
}
if (-not (Select-String -Quiet -LiteralPath $waves -SimpleMatch 'ATTACK SUPPORT NEAR CAP DEFER')) {
    throw "Workshop wave engine is missing the live-unit cap defer"
}
if (Select-String -Quiet -LiteralPath $waves -SimpleMatch '{clone}') {
    throw "Workshop wave engine still clones instead of moving the pool originals"
}
if (Select-String -Quiet -LiteralPath $waves -Pattern '^[^;]*\bfpc') {
    throw "Workshop wave engine still targets fpc* capture points"
}
foreach ($marker in $E2HeloWaveMarkers) {
    if (-not (Select-String -Quiet -LiteralPath $waves -SimpleMatch $marker)) { throw "Workshop wave engine is missing E2 helicopter marker: $marker" }
}
foreach ($marker in $E2HeloForbiddenMarkers) {
    if (Select-String -Quiet -LiteralPath $waves -SimpleMatch $marker) { throw "Workshop wave engine contains forbidden E2 helicopter marker: $marker" }
}
if (-not (Select-String -Quiet -LiteralPath $waves -SimpleMatch '{tag_add attack_support_flag1}')) {
    throw "Workshop wave engine is not claiming a real flag point"
}
if (-not (Select-String -Quiet -LiteralPath $waves -SimpleMatch '{group {select {tag {tag attack_support_deploy}}}}')) {
    throw "Workshop wave engine is not using the proven bare select form"
}
if (Select-String -Quiet -LiteralPath $waves -SimpleMatch '{tag attack_support_probe}') {
    throw "Workshop wave engine still gates on the unproven attack_support_probe marker"
}
if (Select-String -Quiet -LiteralPath $waves -SimpleMatch '{zone {zone "gamezone"}}') {
    throw "Workshop wave engine still separates entities by zone"
}
$tplTarget = Join-Path $WorkshopRoot "resource\map\multi\attack_support_templates.inc"
if (-not (Select-String -Quiet -LiteralPath $tplTarget -SimpleMatch '{Human "mp/nato/2022s/usmc_rifleman" 0xaf23')) {
    throw "Workshop template pool is missing or not real-breed"
}
if (Select-String -Quiet -LiteralPath $tplTarget -Pattern '^\s*\{Human ""') {
    throw "Workshop template pool reverted to the breed-less empty-name Human form, which spawns unarmed bodies"
}
if ((Select-String -LiteralPath $tplTarget -SimpleMatch '{Able "-select"}').Count -ne 84) {
    throw "Workshop template pool is not the 84-prototype wave pool"
}

# Player-nation pools, workshop side. Same depths and the same band separation the
# source check enforces, so a hand-edited workshop copy cannot drift.
$factionTarget = Join-Path $WorkshopRoot "resource\map\multi\faction_support_templates.inc"
if (-not (Test-Path -LiteralPath $factionTarget)) {
    throw "Workshop is missing the player-nation pool: faction_support_templates.inc"
}
foreach ($marker in $E2HeloTemplateMarkers) {
    if (-not (Select-String -Quiet -LiteralPath $factionTarget -SimpleMatch $marker)) { throw "Workshop E2 helicopter template is missing marker: $marker" }
}
if ((Select-String -LiteralPath $factionTarget -SimpleMatch '{Able "-select"}').Count -ne 502) {
    throw "Workshop faction pool is not the 502-prototype player-nation pool"
}
if (Select-String -Quiet -LiteralPath $factionTarget -Pattern '^\s*\{Human ""') {
    throw "Workshop faction pool reverted to the breed-less empty-name Human form"
}
foreach ($marker in @(
    '{Human "mp/rusa/2022s/rus90_squadlead" 0xb200',
    '"ally_sup_tpl"',
    '"ally_sup_rusa_line"',
    '"ally_sup_ukr_recon"',
    '"ally_sup_prc_manpad"',
    '"ally_sup_nato_assault"',
    '{Entity "fennek"',
    '{Entity "humvee_m2hb_ukr"',
    '{Entity "mi17_b8_rus"', '{Entity "mi17_b8_ukr"', '{Entity "uh-60m_blackhawk_mg"',
    '{Entity "il-76td_para"', '{Entity "c130_para"', 'support_e2_para_pax', '{Chassis "helicopter"'
)) {
    if (-not (Select-String -Quiet -LiteralPath $factionTarget -SimpleMatch $marker)) {
        throw "Workshop faction pool is missing marker: $marker"
    }
}
foreach ($banned in @('{clone}', '{include {prop human}}', '{state {state operatable}}', 'allied_support')) {
    if (Select-String -Quiet -LiteralPath $factionTarget -SimpleMatch $banned) {
        throw "Workshop faction pool uses the forbidden idiom $banned"
    }
}

# The retired allied-support experiment must be gone from the workshop, not merely
# unreferenced: a leftover .inc is dead weight and invites a map to include it again.
# NOTE: faction_support_templates.inc is the LIVE player-nation pool and is not one of
# these. It was deliberately renamed off the allied_support_* prefix precisely because
# that substring is banned outright by the guards in this script.
foreach ($retired in @(
    "resource\map\multi\allied_support_waves.inc",
    "resource\map\multi\allied_support_templates.inc",
    "resource\map\multi\allied_support_faction_templates.inc"
)) {
    if (Test-Path -LiteralPath (Join-Path $WorkshopRoot $retired)) {
        throw "Workshop still carries the retired allied-support file: $retired"
    }
}

# ===== ENEMY DEFENCE ENGINE (workshop side) =====
foreach ($marker in @(
    '{"enemy_defense/init"',
    '{"enemy_defense/trickle"',
    '{"enemy_defense/surge"',
    '{"enemy_defense/patrol_1"',
    '{"enemy_defense/patrol_4"',
    '{var "user_is_defender$"}',
    '{var "id_1st_enemy$"}',
    '{var "bot_army$"}',
    '{"trigger" {name "enemy_defense/trickle"}}',
    '{"trigger" {name "enemy_defense/surge"}}',
    '{"trigger" {name "enemy_defense/patrol_1"}}',
    '{target {ignore_captured_by_user 0} {tag enemy_def_af1}}',
    'ENEMY DEFENSE NEAR CAP DEFER',
    'ENEMY DEFENSE WAVES EXHAUSTED',
    '{group {select {tag {tag enemy_def_deploy}}}}'
)) {
    if (-not (Select-String -Quiet -LiteralPath $def -SimpleMatch $marker)) {
        throw "Workshop enemy defence engine is missing marker: $marker"
    }
}
foreach ($banned in @('{clone}', '{include {prop human}}', '{state {state operatable}}', '{zone {zone "gamezone"}}')) {
    if (Select-String -Quiet -LiteralPath $def -SimpleMatch $banned) {
        throw "Workshop enemy defence engine uses the forbidden idiom $banned"
    }
}
if (Select-String -Quiet -LiteralPath $def -Pattern '^[^;]*\bfpc') {
    throw "Workshop enemy defence engine still targets fpc* capture points"
}
if ((Select-String -LiteralPath $defTpl -SimpleMatch '{Able "-select"}').Count -ne 160) {
    throw "Workshop enemy defence pool is not the 160-prototype four-faction pool"
}
foreach ($breedRef in @(
    '{Human "mp/rusa/2022s/rus90_rifleman"',
    '{Human "mp/ukr/2022s/ter_rifleman"',
    '{Human "mp/prc/2022s/pla_rifleman"',
    '{Human "mp/nato/2022s/nato_rifleman"'
)) {
    if (-not (Select-String -Quiet -LiteralPath $defTpl -SimpleMatch $breedRef)) {
        throw "Workshop enemy defence pool is missing a faction pool: $breedRef"
    }
}
if (Select-String -Quiet -LiteralPath $defTpl -Pattern '^\s*\{Human ""') {
    throw "Workshop enemy defence pool reverted to the breed-less empty-name Human form"
}

# ===== HUMAN-DEFENCE MISSION ENGINES (workshop side) =====
foreach ($marker in @(
    '{"defense_support/init"',
    '{"defense_support/clock"',
    '{"defense_support/hold_1"',
    '{"defense_support/comp_usmc"',
    '{var "user_is_defender$"}',
    '{var "prep_inform$"}',
    '{var "id_defenderbot$"}',
    '{"trigger" {name "defense_support/clock"}}',
    '{"trigger" {name "defense_support/hold_1"}}',
    '{target {ignore_captured_by_user 0} {tag def_sup_af1}}',
    'DEFENSE SUPPORT NEAR CAP DEFER',
    'DEFENSE SUPPORT WAVES EXHAUSTED',
    'DEFENSE SUPPORT OWNER UNRESOLVED - NO TRANSFER',
    '{group {select {tag {tag def_sup_deploy}}}}'
)) {
    if (-not (Select-String -Quiet -LiteralPath $ds -SimpleMatch $marker)) {
        throw "Workshop defence support engine is missing marker: $marker"
    }
}
foreach ($marker in @(
    '{"enemy_attack/init"',
    '{"enemy_attack/clock"',
    '{"enemy_attack/rusa_line"',
    '{"enemy_attack/nato_wpn"',
    '{var "user_is_defender$"}',
    '{var "prep_inform$"}',
    '{var "id_1st_enemy$"}',
    '{var "bot_army$"}',
    '{"trigger" {name "enemy_attack/clock"}}',
    '{target {ignore_captured_by_user 0} {tag ea_flag1}}',
    'ENEMY ATTACK NEAR CAP DEFER',
    'ENEMY ATTACK WAVES EXHAUSTED',
    '{group {select {tag {tag ea_deploy}}}}'
)) {
    if (-not (Select-String -Quiet -LiteralPath $ea -SimpleMatch $marker)) {
        throw "Workshop enemy attack engine is missing marker: $marker"
    }
}
foreach ($pair in @(@($ds, 'defence support'), @($ea, 'enemy attack'))) {
    $path = $pair[0]
    $label = $pair[1]
    foreach ($banned in @('{clone}', '{include {prop human}}', '{state {state operatable}}', '{zone {zone "gamezone"}}', 'allied_support')) {
        if (Select-String -Quiet -LiteralPath $path -SimpleMatch $banned) {
            throw "Workshop $label engine uses the forbidden idiom $banned"
        }
    }
    if (Select-String -Quiet -LiteralPath $path -Pattern '^[^;]*\bfpc') {
        throw "Workshop $label engine still targets fpc* capture points"
    }
    if (-not (Select-String -Quiet -LiteralPath $path -SimpleMatch '{var "prep_inform$"} {op "=="} {value 1}')) {
        throw "Workshop $label engine does not gate on prep_inform$ == 1, so it deploys during the defence preparation phase"
    }
}

# Workshop side of the diagnostic gate. This is the copy the game actually reads, so it
# is the one that decides whether a player sees support timers. dcg_vars.inc must also
# declare the toggle: an undeclared var reads 0 too, but then the flip has nothing to set.
if (-not (Select-String -Quiet -LiteralPath $vars -SimpleMatch '{"support_debug"}')) {
    throw "Workshop dcg_vars.inc does not declare the support_debug toggle"
}
if (-not (Select-String -Quiet -LiteralPath $vars -SimpleMatch '{"support_announce"}')) {
    throw "Workshop dcg_vars.inc does not declare the support_announce toggle"
}
$pot = Join-Path $WorkshopRoot "localizations\default\interface\text\mission\multi\support_events.pot"
$cePot = Join-Path $WorkshopRoot "localizations\default\interface\text\mission\multi\ce_mission_messages.pot"
$ceMapTarget = Join-Path $WorkshopRoot $files[17]
$ceScriptTarget = Join-Path $WorkshopRoot $files[18]
$ceTargetHashes = @($ceMapTarget, $ceScriptTarget) | ForEach-Object { (Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash }
if ($ceTargetHashes[0] -ne $ceTargetHashes[1]) { throw "Workshop CE ai_logic mirrors are not byte-identical" }
if (-not (Test-Path -LiteralPath $pot)) {
    throw "Workshop is missing support_events.pot"
}
if (-not (Test-Path -LiteralPath $cePot)) {
    throw "Workshop is missing ce_mission_messages.pot"
}
foreach ($key in @(
    'mission/multi/support/wave_inbound',
    'mission/multi/support/vehicle_inbound',
    'mission/multi/support/flank_inbound',
    'mission/multi/support/waves_exhausted',
    'mission/multi/support/defense_reinforced',
    'mission/multi/support/enemy_activity',
    'mission/multi/support/airborne_inbound_nato',
    'mission/multi/support/e2_helo_inbound', 'mission/multi/support/e2_para_inbound',
    'mission/multi/support/e2_insert_failed'
)) {
    if (-not (Select-String -Quiet -LiteralPath $pot -SimpleMatch "msgctxt `"$key`"")) {
        throw "support_events.pot is missing msgctxt $key"
    }
}
if (-not (Select-String -Quiet -LiteralPath $cePot -SimpleMatch 'msgctxt "mission/multi/dcg_error01"')) {
    throw "ce_mission_messages.pot is missing dcg_error01"
}
foreach ($pair in @(
    @($waves, 'workshop attack support engine'),
    @($def, 'workshop enemy defence engine'),
    @($ds, 'workshop defence support engine'),
    @($ea, 'workshop enemy attack engine')
)) {
    Test-SupportTimerGate $pair[0] $pair[1]
}

# THE ENTRY ROUND ROBIN. Every deployed engine addresses the numbered pads and none of
# them still places on the bare legacy name: that one is kept as an alias only for the
# patrol / roam {action move} orders. Each engine bumps its own cursor once per batch -
# a shared cursor would let two engines running on the same mission cancel each other's
# rotation - and each placement site is a three-case cascade on it.
foreach ($quad in @(
    @($waves, 'workshop attack support engine', 'attack_support', 'am_entry_next', 'am_place_at_entry', 'attack_support_entry_'),
    @($def, 'workshop enemy defence engine', 'enemy_defense', 'ed_entry_next', 'ed_place', 'attack_support_entry_'),
    @($ds, 'workshop defence support engine', 'defense_support', 'ds_entry_next', 'ds_place_at_entry', 'attack_support_rear_'),
    @($ea, 'workshop enemy attack engine', 'enemy_attack', 'ea_entry_next', 'ea_place_at_entry', 'attack_support_entry_')
)) {
    $path = $quad[0]
    $label = $quad[1]
    $var = $quad[2]
    $rotate = $quad[3]
    $batch = $quad[4]
    # Pad family: the defence engine forms up on the rear tier, everyone else on the
    # entry tier. The rule is unchanged - three distinct pads per side, or arrivals stack.
    $family = $quad[5]
    $code = Get-MiCode $path
    foreach ($side in @('a', 'b')) {
        foreach ($point in @(1, 2, 3)) {
            if (-not $code.Contains('{target_waypoint "' + $family + $side + $point + '"}')) {
                throw "$label never places on pad $family$side$point, so arrivals still stack: $path"
            }
        }
    }
    foreach ($side in @('a', 'b')) {
        if ($code.Contains('{target_waypoint "attack_support_entry_' + $side + '"}')) {
            throw "$label still places on the legacy single pad attack_support_entry_$side : $path"
        }
    }
    if (-not $code.Contains('(define "' + $rotate + '"')) {
        throw "$label is missing its entry-pad rotation define $rotate : $path"
    }
    $cursor = "$($var)_entry_rr"
    if (-not $code.Contains('{"set_i" {var "' + $cursor + '$"} {op "+"} {value 1}}')) {
        throw "$label never advances $cursor, so every batch lands on pad 1: $path"
    }
    if (-not $code.Contains('{"set_i" {var "' + $cursor + '$"} {op "="} {value 1}}')) {
        throw "$label never wraps $cursor back to 1, so it runs off the end: $path"
    }
    # Bumped once per BATCH, at the top of the batch define - not per body, which would
    # scatter one fireteam across all three pads, and not per wave, which would leave
    # G1 and G2 of the same wave on the same pad.
    $batchAt = $code.IndexOf('(define "' + $batch + '"')
    if ($batchAt -lt 0) {
        throw "$label is missing its placement batch define $batch : $path"
    }
    $batchBody = $code.Substring($batchAt)
    $rotAt = $batchBody.IndexOf('("' + $rotate + '")')
    $placeAt = $batchBody.IndexOf('_place_one")')
    if ($rotAt -lt 0 -or $placeAt -lt 0 -or $rotAt -gt $placeAt) {
        throw "$label does not bump the entry cursor before the first body of $batch : $path"
    }
    # Two explicit cases per cascade (pads 2 and 3, pad 1 is the default) across the
    # three branches of the spawn-side switch: side a, side b, and the unpublished
    # fallback. Six is the whole placement surface of the engine.
    $cascades = ([regex]::Matches($code, '\{var "' + $cursor + '\$"\} \{op "=="\}')).Count
    if ($cascades -ne 6) {
        throw "$label has $cascades round-robin cascade cases, expected 6: $path"
    }
    Write-Host "OK rr $label 6 pads addressed, cursor bumped once per $batch"
}

Write-Host "`nVerification markers:"
Select-String -LiteralPath $gameSet -Pattern "aiTeamPlayers 1"
Select-String -LiteralPath $botMain -Pattern "CODEX_ATTACK_SUPPORT_ROUTER|route_skip|first_player_slot|safeRequire"
Select-String -LiteralPath $support -Pattern "CODEX_ATTACK_SUPPORT|attack_support_use_mi"
Select-String -LiteralPath $vars -Pattern "attack_support_armed|attack_support_wave|attack_support_next_ok|attack_support_use_mi"
Select-String -LiteralPath $waves -Pattern '\{"attack_support/|ATTACK SUPPORT (ARMED|WAVE|NEAR|POOL)'
Select-String -LiteralPath $vars -Pattern "enemy_defense_"
Select-String -LiteralPath $def -Pattern '\{"enemy_defense/'
Select-String -LiteralPath $vars -Pattern "defense_support_|enemy_attack_"
Select-String -LiteralPath $ds -Pattern '\{"defense_support/|DEFENSE SUPPORT (ARMED|WAVE|NEAR|POOL|OWNER)'
Select-String -LiteralPath $ea -Pattern '\{"enemy_attack/|ENEMY ATTACK (ARMED|WAVE|NEAR|POOL|ARMY)'
Write-Host "Patched maps: $($mapFiles.Count)"
Write-Host "Flag ammo supply points linked: $flagAmmoTotal"

# The deployed def is what the {Link ... "ammo"} lines actually resolve to, so the
# include swap is verified on the copy the game reads, not only on the source.
$flagAmmoDef = Join-Path $WorkshopRoot $files[16]
if (-not (Test-Path -LiteralPath $flagAmmoDef)) {
    throw "Workshop is missing the flagpoint_ammo shadow def"
}
if (-not (Select-String -Quiet -LiteralPath $flagAmmoDef -SimpleMatch '(include "/properties/resupply_hotmod.inc")')) {
    throw "Workshop flagpoint_ammo.def does not pull the modern resupply tables"
}
if (Select-String -Quiet -LiteralPath $flagAmmoDef -SimpleMatch '(include "/properties/resupply.inc")') {
    throw "Workshop flagpoint_ammo.def still pulls the base WW2 resupply tables"
}
if (-not (Select-String -Quiet -LiteralPath $flagAmmoDef -SimpleMatch '("flag_ammo_heavy")')) {
    throw "Workshop flagpoint_ammo.def is missing the flag_ammo_heavy supply-zone call"
}
# Shipping the .mdl alongside would shadow the pak model too and is not the intent -
# only the def is ours; the geometry and the decal stay base-game.
if (Test-Path -LiteralPath (Join-Path $WorkshopRoot "resource\entity\service\-multiplayer\flag_point\flagpoint_ammo\flagpoint_ammo.mdl")) {
    throw "Workshop carries a shadow flagpoint_ammo.mdl, which was never meant to ship"
}

Write-Host "`nDeployment complete. Fully restart Gates of Hell before testing."


# Phase 2: only the friendly attack engine may address flank pads.
foreach ($pair in @(
    @((Join-Path $WorkshopRoot "resource\map\multi\defense_support_waves.inc"), "defence support"),
    @((Join-Path $WorkshopRoot "resource\map\multi\enemy_defense_support.inc"), "enemy defence"),
    @((Join-Path $WorkshopRoot "resource\map\multi\enemy_attack_support.inc"), "enemy attack")
)) {
    if (Select-String -Quiet -LiteralPath $pair[0] -SimpleMatch "attack_support_flank_") {
        throw ("Workshop {0} engine must not reference flank pads" -f $pair[1])
    }
}



# Flag-prop pool (Phase 4 rewrite).
$flagPropsTarget = Join-Path $WorkshopRoot "resource\map\multi\flag_props_templates.inc"
if (-not (Test-Path -LiteralPath $flagPropsTarget)) {
    throw "Workshop is missing flag_props_templates.inc"
}
# Twelve, not the original fifteen: the three ammo crates are retired. Flags get
# their supply from a real flagpoint_ammo linked into the flag's own "ammo" placer
# slot (see Set-FlagAmmoSupply), which is the vanilla mechanism and follows the flag
# when it changes hands, so a crate prop on top of it is dead weight. Only the L2+
# crewless weapon half of Phase 4 still comes out of this pool.
if ((Select-String -LiteralPath $flagPropsTarget -SimpleMatch '{Able "-select"}').Count -ne 12) {
    throw "Workshop flag-prop pool is not the 12-prototype weapons-only pool"
}
$flagPropsCode = Get-MiCode $flagPropsTarget
if ($flagPropsCode -match 'para_ammo') {
    throw "Workshop flag-prop pool still parks the retired ammo crates"
}
if ($flagPropsCode -match 'flag_prop_ammo') {
    throw "Workshop flag-prop pool still carries the retired ammo-crate role tag"
}
foreach ($marker in @(
    '{Entity "mg_stand_nsvt_rus_ai"',
    '{Entity "mg_stand_nsvt_ukr_ai"',
    '{Entity "mg_stand_qjz171"',
    '{Entity "bgm71_tow_ai"',
    '"flag_prop_tpl"',
    '"flag_prop_wpn_rusa"',
    '"flag_prop_wpn_nato"'
)) {
    if (-not (Select-String -Quiet -LiteralPath $flagPropsTarget -SimpleMatch $marker)) {
        throw "Workshop flag-prop pool is missing marker: $marker"
    }
}
if (Select-String -Quiet -LiteralPath $flagPropsTarget -SimpleMatch '{clone}') {
    throw "Workshop flag-prop pool uses forbidden {clone}"
}
# Engines must not use entity-name selectors or runtime spawn for props.
foreach ($pair in @(
    @((Join-Path $WorkshopRoot "resource\map\multi\defense_support_waves.inc"), "defence support"),
    @((Join-Path $WorkshopRoot "resource\map\multi\enemy_defense_support.inc"), "enemy defence")
)) {
    $code = Get-Content -LiteralPath $pair[0] -Raw
    $codeOnly = ($code -split "`n" | ForEach-Object { ($_ -split ";", 2)[0] }) -join "`n"
    if ($codeOnly -match '\{select \{entity') {
        throw ("Workshop {0} still uses entity-name selectors for props" -f $pair[1])
    }
    if ($codeOnly -match '\{"spawn"') {
        throw ("Workshop {0} still uses runtime spawn" -f $pair[1])
    }
    # The weapon half of Phase 4 stays; the crate half is retired and must not
    # reappear in either garrison path.
    if ($codeOnly -notmatch 'flag_prop_wpn_nato') {
        throw ("Workshop {0} missing the L2+ crewless weapon claim" -f $pair[1])
    }
    if ($codeOnly -match 'flag_prop_ammo') {
        throw ("Workshop {0} still claims the retired ammo crate" -f $pair[1])
    }
}

# Phase 5: only friendly attack engine may address airmobile LZ pads.
# Friendly attack + defense both use airmobile LZ pads (parity).
# Enemy engines stay on edge delivery for now (no air pad refs).
foreach ($pair in @(
    @((Join-Path $WorkshopRoot "resource\map\multi\enemy_defense_support.inc"), "enemy defence"),
    @((Join-Path $WorkshopRoot "resource\map\multi\enemy_attack_support.inc"), "enemy attack")
)) {
    if (Select-String -Quiet -LiteralPath $pair[0] -SimpleMatch "attack_support_air_") {
        throw ("Workshop {0} engine must not reference air pads" -f $pair[1])
    }
}
