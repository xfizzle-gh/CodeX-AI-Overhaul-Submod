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
    "resource\map\multi\enemy_attack_support.inc"
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

foreach ($source in @($gameSetSource, $botMainSource, $supportSource, $varsSource, $wavesSource, $tplSource, $defSource, $defTplSource, $conquestSource, $utilitySource, $dsSource, $eaSource)) {
    if (-not (Test-Path -LiteralPath $source)) {
        throw "Missing source file: $source"
    }
}
# string.sub on a nil spawnPointName faulted natively on slots the engine gives no
# spawn point, so the read must stay type-guarded before the substring.
if (-not (Select-String -Quiet -LiteralPath $utilitySource -SimpleMatch 'if type(spawnPoint) ~= "string" or spawnPoint == "" then')) {
    throw "Source utility.lua is missing the spawnPoint nil-guard, which crashes natively on a slot with no spawn point"
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
$SlotUnsafe = @('spawnPointName', 'PlayerSpawnPoint', 'require(')
$supportCode = Get-LuaCode $supportSource
foreach ($banned in $SlotUnsafe) {
    if ($supportCode.Contains($banned)) {
        throw "Source attack_support.lua touches the slot-unsafe surface: $banned"
    }
}
foreach ($marker in @(
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
    '{"enemy_attack_owner_fail"}'
)) {
    if (-not (Select-String -Quiet -LiteralPath $varsSource -SimpleMatch $marker)) {
        throw "Source dcg_vars.inc is missing marker: $marker"
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
    '{target_waypoint "attack_support_entry_a"}',
    '{target_waypoint "attack_support_entry_b"}',
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
    # Self-re-arming randomized cadence and the level-scaled wave budget.
    '{"trigger" {name "attack_support/clock"}}',
    '{condition {type rand} {value 0.2}}',
    '{"delay" {time 150}}',
    '{"delay" {time 300}}',
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
if ((Select-String -LiteralPath $wavesSource -SimpleMatch '{state {state inactive}}').Count -ne 3) {
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
    '{target_waypoint "attack_support_entry_a"}',
    '{target_waypoint "attack_support_entry_b"}',
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
    # 140-290s clock shares no value with enemy_attack_support.inc's 125-280s clock,
    # which is the only other engine live on the same mission.
    '{"trigger" {name "defense_support/clock"}}',
    '{"trigger" {name "defense_support/hold_1"}}',
    '{"delay" {time 140}}',
    '{"delay" {time 290}}',
    '{"delay" {time 90}}',
    '{"delay" {time 150}}',
    # Reinforcements enter at the DEFENDER's own edge, which is the side the enemy is
    # NOT on: for this engine enemy_spawnside$ 1 means side b.
    '{target_waypoint "attack_support_entry_a"}',
    '{target_waypoint "attack_support_entry_b"}',
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
    'DEFENSE SUPPORT POOL SHORT - RIFLE TEAM INSTEAD',
    'DEFENSE SUPPORT POOL EXHAUSTED',
    # Shared NATO pool: a claim strips the pool tag it took from.
    '{tag_remove attack_support_inf_usmc}',
    '{tag_remove attack_support_inf_1ad}',
    '{tag_remove attack_support_inf_pzgd}',
    '{tag_remove attack_support_tpl}',
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
    '{target_waypoint "attack_support_entry_a"}',
    '{target_waypoint "attack_support_entry_b"}',
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
    if (Select-String -Quiet -LiteralPath $path -Pattern '^\s*\{(Human|Entity|Vehicle) ') {
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
if ($tplAbleCount -ne 64) {
    throw "Source template pool must park 64 prototypes with selection stripped (20 USMC + 20 1AD + 12 pzgren infantry plus 4 crewed humvees); found $tplAbleCount"
}
foreach ($pair in @(
    @('attack_support_inf_usmc', 20),
    @('attack_support_inf_1ad', 20),
    @('attack_support_inf_pzgd', 12)
)) {
    $n = (Select-String -LiteralPath $tplSource -SimpleMatch ('"' + $pair[0] + '"')).Count
    if ($n -ne $pair[1]) {
        throw "Source template pool must tag $($pair[1]) prototypes as $($pair[0]); found $n"
    }
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

Write-Host "Deploying attack support wave engine (opening wave 30-45s, then randomized 150-300s cadence, level-scaled composition pools)"
Write-Host "  plus the enemy defence engine (garrison on the live flags, four patrol groups, 45-90s trickle + 180-300s surge off the defender's own edge)"
Write-Host "  plus the two human-DEFENCE mission engines, both behind prep_inform: defensive support for the defender bot (25-40s opening, 140-290s cadence, hold groups on the active flags) and enemy attacker pressure (65-95s opening, 125-280s cadence, assault on the player's flags)"
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
    "resource\script\multiplayer\modes\attack_support_brain.lua"
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

$anchor = '(include "../allied_support_waves.inc")'
$wavesInclude = '(include "../attack_support_waves.inc")'
$tplAnchor = '(include "../allied_support_templates.inc")'
$tplInclude = '(include "../attack_support_templates.inc")'
# Enemy-defence half. Each sits immediately after its attack-support counterpart:
# the engine in the triggers section, the prototype pool in the entities section.
$defInclude = '(include "../enemy_defense_support.inc")'
$defTplInclude = '(include "../enemy_defense_templates.inc")'
# The two human-DEFENCE mission engines, in the triggers section behind the two
# attack-mission ones. Neither has a templates include: defence support claims from the
# attack-support pool and enemy attack from the enemy-defence pools, and the engines
# that own those pools are inert on a defence mission, so there is no contention and
# nothing extra to park.
$dsInclude = '(include "../defense_support_waves.inc")'
$eaInclude = '(include "../enemy_attack_support.inc")'
$waypointsAnchor = "`t`t{waypoints"
$entryName = '{"attack_support_entry_'
# Name kept from the probe era on purpose: it holds the genuinely pristine
# pre-patch maps, and the "already backed up" check below is what stops a rerun
# from overwriting them with maps this script has already patched.
$backupRoot = Join-Path $WorkshopRoot "_attack_support_probe_backups"

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

    $wavesCount = ([regex]::Matches($text, [regex]::Escape($wavesInclude))).Count

    if ($wavesCount -eq 0) {
        if (-not $text.Contains($anchor)) {
            throw "Map is missing allied-support include anchor: $mapFile"
        }

        $relativeMap = $mapFile.Substring($WorkshopRoot.Length).TrimStart('\')
        $backup = Join-Path $backupRoot $relativeMap
        if (-not (Test-Path -LiteralPath $backup)) {
            New-Item -ItemType Directory -Force -Path (Split-Path $backup) | Out-Null
            Copy-Item -LiteralPath $mapFile -Destination $backup -Force
        }

        $replacement = $anchor + "`r`n`t`t`t" + $wavesInclude
        $text = $text.Replace($anchor, $replacement)
        [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))
        $wavesCount = 1
    }

    if ($wavesCount -ne 1) {
        throw "Expected exactly one wave-engine include in: $mapFile"
    }

    # Entities-section include for the wave engine's real-breed prototype pool,
    # placed immediately after the existing templates include. Idempotent.
    $text = [System.IO.File]::ReadAllText($mapFile)
    $tplCount = ([regex]::Matches($text, [regex]::Escape($tplInclude))).Count
    if ($tplCount -eq 0) {
        if (-not $text.Contains($tplAnchor)) {
            throw "Map is missing the templates include anchor: $mapFile"
        }
        $text = $text.Replace($tplAnchor, $tplAnchor + "`r`n`t" + $tplInclude)
        [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))
        $tplCount = 1
    }
    if ($tplCount -ne 1) {
        throw "Expected exactly one wave-templates include in: $mapFile"
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

    # Enemy-defence prototype pool, in the entities section right after the
    # attack-support pool. Idempotent.
    $text = [System.IO.File]::ReadAllText($mapFile)
    $defTplCount = ([regex]::Matches($text, [regex]::Escape($defTplInclude))).Count
    if ($defTplCount -eq 0) {
        if (-not $text.Contains($tplInclude)) {
            throw "Map is missing the enemy-defence templates include anchor: $mapFile"
        }
        $text = $text.Replace($tplInclude, $tplInclude + "`r`n`t" + $defTplInclude)
        [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))
        $defTplCount = 1
    }
    if ($defTplCount -ne 1) {
        throw "Expected exactly one enemy-defence templates include in: $mapFile"
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

    # Attack-side entry waypoints, one per spawn side. The dynamic campaign swaps
    # attacker/defender spawns per mission instance, so the engine picks between
    # them at runtime from enemy_spawnside$ - a single static entry is never right.
    # The per-map coordinates live in the repo copy of the map; copy those exact
    # blocks across rather than recomputing anything here.
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
    # side-agnostic one written by earlier deploys, and the pre-rename
    # attack_mate_entry* blocks - then rebuild both sides from the repo. Rewriting
    # beats trying to reconcile whatever an interrupted earlier run left behind.
    $text = [System.IO.File]::ReadAllText($mapFile)
    $text = [regex]::Replace(
        $text,
        '\s*\{"attack_(?:support|mate)_entry[a-z_]*"\s*\r?\n\s*\{position [^}]*\}\s*\r?\n\s*\{radius \d+\}\s*\r?\n\s*\}',
        ''
    )
    foreach ($side in @("b", "a")) {
        $wpMatch = [regex]::Match($repoText, '\{"attack_support_entry_' + $side + '"\s*\r?\n\s*\{position [^}]*\}\s*\r?\n\s*\{radius \d+\}\s*\r?\n\s*\}')
        if (-not $wpMatch.Success) {
            throw "Repo map is missing the attack_support_entry_$side waypoint block: $repoMap"
        }
        if (-not $text.Contains($waypointsAnchor)) {
            throw "Map is missing the waypoints anchor: $mapFile"
        }
        # The repo block already carries the right indentation; only normalise its
        # line endings to CRLF to match the workshop map.
        $block = "`r`n`t`t`t" + ($wpMatch.Value -replace '\r?\n', "`r`n")
        $text = $text.Replace($waypointsAnchor, $waypointsAnchor + $block)
    }
    [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))

    $text = [System.IO.File]::ReadAllText($mapFile)
    foreach ($side in @("a", "b")) {
        $n = ([regex]::Matches($text, [regex]::Escape('{"attack_support_entry_' + $side + '"'))).Count
        if ($n -ne 1) {
            throw "Expected exactly one attack_support_entry_$side waypoint in: $mapFile (found $n)"
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
    # Every include the four quadrants need, exactly once each, and nothing from the
    # retired allied-support experiment.
    foreach ($include in @($tplInclude, $defTplInclude, $wavesInclude, $defInclude, $dsInclude, $eaInclude)) {
        $n = ([regex]::Matches($text, [regex]::Escape($include))).Count
        if ($n -ne 1) {
            throw "Expected exactly one $include in: $mapFile (found $n)"
        }
    }
    if (([regex]::Matches($text, [regex]::Escape('{"allied_support_entry"'))).Count -ne 1) {
        throw "Map lost its allied_support_entry waypoint: $mapFile"
    }

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
    '{"attack_support_use_mi"}'
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
if ((Select-String -LiteralPath $tplTarget -SimpleMatch '{Able "-select"}').Count -ne 64) {
    throw "Workshop template pool is not the 64-prototype wave pool"
}
$poolTarget = Join-Path $WorkshopRoot "resource\map\multi\allied_support_templates.inc"
if (-not (Select-String -Quiet -LiteralPath $poolTarget -SimpleMatch '{Tags "allied_support_template" "hidden" "cmp_def" 0xaf01}')) {
    throw "Workshop off-map template pool is missing or untagged: allied_support_templates.inc"
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
    foreach ($banned in @('{clone}', '{include {prop human}}', '{state {state operatable}}', '{zone {zone "gamezone"}}')) {
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

Write-Host "`nDeployment complete. Fully restart Gates of Hell before testing."
