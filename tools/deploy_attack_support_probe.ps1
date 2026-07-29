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
    "resource\script\multiplayer\modes\conquest.lua",
    # utility.lua carries the spawnPoint nil-guard. string.sub on a nil spawn
    # point faulted natively, so this file has to ship with conquest.lua or the
    # fix simply is not present in the game.
    "resource\script\multiplayer\modes\utility.lua"
)

$gameSetSource = Join-Path $RepoRoot $files[0]
$botMainSource = Join-Path $RepoRoot $files[1]
$supportSource = Join-Path $RepoRoot $files[2]
$varsSource = Join-Path $RepoRoot $files[3]
$wavesSource = Join-Path $RepoRoot $files[4]
$tplSource = Join-Path $RepoRoot $files[5]
$conquestSource = Join-Path $RepoRoot $files[6]
$utilitySource = Join-Path $RepoRoot $files[7]

foreach ($source in @($gameSetSource, $botMainSource, $supportSource, $varsSource, $wavesSource, $tplSource, $conquestSource, $utilitySource)) {
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
    '{"attack_support_use_mi"}'
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
    'Context.SpawnSeekTimer = Context.SpawnSeekTimer or {}'
)) {
    if (-not (Select-String -Quiet -LiteralPath $conquestSource -SimpleMatch $marker)) {
        throw "Source conquest.lua is missing marker: $marker"
    }
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
$conquest = Join-Path $WorkshopRoot $files[6]

if (-not (Select-String -Quiet -LiteralPath $gameSet -SimpleMatch "{aiTeamPlayers 1}")) {
    throw "Workshop game set does not contain the attack support AI slot marker"
}
foreach ($marker in @(
    'local function ensureAttackPrepInform',
    'local function IssueScatterOrder'
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

Write-Host "`nVerification markers:"
Select-String -LiteralPath $gameSet -Pattern "aiTeamPlayers 1"
Select-String -LiteralPath $botMain -Pattern "CODEX_ATTACK_SUPPORT_ROUTER|route_skip|first_player_slot|safeRequire"
Select-String -LiteralPath $support -Pattern "CODEX_ATTACK_SUPPORT|attack_support_use_mi"
Select-String -LiteralPath $vars -Pattern "attack_support_armed|attack_support_wave|attack_support_next_ok|attack_support_use_mi"
Select-String -LiteralPath $waves -Pattern '\{"attack_support/|ATTACK SUPPORT (ARMED|WAVE|NEAR|POOL)'
Write-Host "Patched maps: $($mapFiles.Count)"

Write-Host "`nDeployment complete. Fully restart Gates of Hell before testing."
