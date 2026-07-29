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
    "resource\script\multiplayer\modes\attacker_mate.lua",
    "resource\map\multi\dcg_vars.inc",
    "resource\map\multi\attack_mate_retask_probe.inc",
    "resource\map\multi\attack_mate_probe_templates.inc",
    "resource\script\multiplayer\modes\conquest.lua",
    # utility.lua carries the spawnPoint nil-guard. string.sub on a nil spawn
    # point faulted natively, so this file has to ship with conquest.lua or the
    # fix simply is not present in the game.
    "resource\script\multiplayer\modes\utility.lua"
)

$gameSetSource = Join-Path $RepoRoot $files[0]
$botMainSource = Join-Path $RepoRoot $files[1]
$mateSource = Join-Path $RepoRoot $files[2]
$varsSource = Join-Path $RepoRoot $files[3]
$retaskSource = Join-Path $RepoRoot $files[4]
$tplSource = Join-Path $RepoRoot $files[5]
$conquestSource = Join-Path $RepoRoot $files[6]
$utilitySource = Join-Path $RepoRoot $files[7]

foreach ($source in @($gameSetSource, $botMainSource, $mateSource, $varsSource, $retaskSource, $tplSource, $conquestSource, $utilitySource)) {
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
    throw "Source game set does not contain the attack-mate AI slot marker"
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
    'sc:SetVar("id_attacker_mate", id.playerId)',
    'sc:SetVar("attacker_mate_ready", 1)',
    # The mate arms MI delivery explicitly. Lua Spawn on this slot never reports
    # an available unit, so MI is the only path that puts bodies on the map.
    'sc:SetVar("attack_mate_use_mi_probe", 1)'
)) {
    if (-not (Select-String -Quiet -LiteralPath $mateSource -SimpleMatch $marker)) {
        throw "Source attacker_mate.lua is missing marker: $marker"
    }
}
# These reads AV on a slot with no spawn deck, and pulling in utility.lua from
# here crashed in lua.event.notify2 the moment the module loaded. Checked against
# a comment-stripped view: the file's own header names these forms as the rule.
function Get-LuaCode([string]$path) {
    return ((Get-Content -LiteralPath $path) | ForEach-Object { ($_ -split '--', 2)[0] }) -join "`n"
}
$SlotUnsafe = @('spawnPointName', 'PlayerSpawnPoint', 'require(')
$mateCode = Get-LuaCode $mateSource
foreach ($banned in $SlotUnsafe) {
    if ($mateCode.Contains($banned)) {
        throw "Source attacker_mate.lua touches the slot-unsafe surface: $banned"
    }
}
foreach ($marker in @(
    '{"attack_mate_probe_started"}',
    '{"attack_mate_probe_transferred"}',
    '{"attack_mate_wave_cmd"}',
    '{"attack_mate_use_mi_probe"}'
)) {
    if (-not (Select-String -Quiet -LiteralPath $varsSource -SimpleMatch $marker)) {
        throw "Source dcg_vars.inc is missing marker: $marker"
    }
}
foreach ($marker in @(
    '{var "user_is_defender$"}',
    '{var "attacker_mate_ready$"}',
    '{var "attack_mate_use_mi_probe$"}',
    '{target_waypoint "attack_mate_entry_a"}',
    '{target_waypoint "attack_mate_entry_b"}',
    '{var "enemy_spawnside$"}',
    '{player "3"}',
    # Command-gated wave clock. Waves keyed on entity presence alone all fired at
    # once; each wave now needs its own command value and clears it on entry.
    '{var "attack_mate_wave_cmd$"}',
    '{"attack_mate/schedule"',
    '{"attack_mate/wave1"',
    '{"attack_mate/wave2"',
    '{"attack_mate/wave3"',
    '("am_place_at_entry")',
    '("am_own_to_mate")',
    '("am_finish_deploy")',
    # Capture points are addressed as {tag flag}. The fpc1..fpc5 tags are absent
    # from one of the fourteen maps entirely, which left units standing still.
    '{select {tag {tag flag}}}',
    '{tag_add attack_mate_flag1}',
    '{tag_add attack_mate_flag2}',
    '{tag_add attack_mate_flag3}',
    # No-clone design: the pool originals are MOVED to the entry waypoint and
    # promoted in place, so they still carry the tag we put on them.
    '{tag_remove attack_mate_tpl}',
    # Decorating the advanced selector that addresses pool units zeroes the
    # match, so the deploy set is selected bare and nothing else.
    '{group {select {tag {tag attack_mate_deploy}}}}'
)) {
    if (-not (Select-String -Quiet -LiteralPath $retaskSource -SimpleMatch $marker)) {
        throw "Source retask probe is missing marker: $marker"
    }
}
foreach ($n in 1..16) {
    if (-not (Select-String -Quiet -LiteralPath $retaskSource -SimpleMatch ('{player "' + $n + '"}'))) {
        throw "Source retask probe is missing literal ownership case for player $n. The engine will not accept a var in the {player} node, so all sixteen slots must be spelled out"
    }
}
if (Select-String -Quiet -LiteralPath $retaskSource -SimpleMatch '{tag attack_mate_probe}') {
    throw "Source retask probe still gates on attack_mate_probe. That tag is a best-effort marker only; key on attack_mate_src, which is proven queryable on these units"
}
if (Select-String -Quiet -LiteralPath $retaskSource -SimpleMatch '{tag_remove attack_mate_src}') {
    throw "Source retask probe removes attack_mate_src, but the entire downstream chain selects on it"
}
if ((Select-String -LiteralPath $retaskSource -SimpleMatch '{state {state inactive}}').Count -ne 3) {
    throw "Source retask probe must exclude inactive flag points on ALL THREE shuffled flag picks - a mission activates only ~2 of a map's capture points, and a squad sent to a dead objective just sprints and stands there"
}
# Decorating the advanced selector that addresses pool units zeroes the match.
# Live proof in one run: a bare select moved all four; the same select plus a
# prop/state decoration matched nothing in the very next action.
foreach ($banned in @('{include {prop human}}', '{prop {prop human}}', '{state {state operatable}}')) {
    if (Select-String -Quiet -LiteralPath $retaskSource -SimpleMatch $banned) {
        throw "Source retask probe decorates a pool selector with $banned, which zeroes the match on these units"
    }
}
if (Select-String -Quiet -LiteralPath $retaskSource -Pattern '^[^;]*\bfpc') {
    throw "Source retask probe still targets fpc* capture points. Those tags are absent from outback entirely; address capture points as {tag flag}"
}
if (Select-String -Quiet -LiteralPath $retaskSource -SimpleMatch '{clone}') {
    throw "Source retask probe still clones. Three promote designs failed to match a cloned entity; a new entity's provenance is invisible to selectors on this engine. Move the originals instead"
}
if (Select-String -Quiet -LiteralPath $retaskSource -SimpleMatch '{zone {zone "gamezone"}}') {
    throw "Source retask probe separates entities by zone. allied_support_entry is a waypoint, not a zone, and is NOT inside gamezone"
}
if (Select-String -Quiet -LiteralPath $retaskSource -SimpleMatch '{"delay" {time 8}}') {
    throw "Source retask probe still contains the superseded blind startup delay"
}
foreach ($marker in @(
    '{Human "mp/nato/2022s/usmc_rifleman" 0xaf25',
    '{Human "mp/nato/2022s/1ad_rifleman" 0xaf34',
    '{Human "mp/nato/2022s/pzgd_rifleman" 0xaf42',
    '{Tags "attack_mate_tpl" "attack_mate_w1" "hidden" 0xaf25}',
    '{Tags "attack_mate_tpl" "attack_mate_w2" "hidden" 0xaf34}',
    '{Tags "attack_mate_tpl" "attack_mate_w3" "hidden" 0xaf42}',
    # Both humvees are crewed by explicit links so they arrive drivable with the
    # M2HB manned, and each carries its own tag because they deploy one at a time
    # (placed together they clip and flip).
    '{Entity "humvee_m2hb_usa" 0xaf50',
    '{Entity "humvee_m2hb_usa" 0xaf54',
    '{Link 0xaf51 {0xaf50 "driver"}}',
    '{Link 0xaf52 {0xaf50 "gunner2"}}',
    '"attack_mate_hmmwv1"',
    '"attack_mate_hmmwv2"'
)) {
    if (-not (Select-String -Quiet -LiteralPath $tplSource -SimpleMatch $marker)) {
        throw "Source probe template pool is missing marker: $marker"
    }
}
$tplAbleCount = (Select-String -LiteralPath $tplSource -SimpleMatch '{Able "-select"}').Count
if ($tplAbleCount -ne 27) {
    throw "Source probe template pool must park 27 prototypes with selection stripped (3 fireteams of 7 plus 2 crewed humvees); found $tplAbleCount"
}
# Line-anchored so the header's prose about {Inventory} does not trip this.
if (Select-String -Quiet -LiteralPath $tplSource -Pattern '^\s*\{Inventory') {
    throw "Probe templates must not bake an Inventory block - the breed supplies the loadout"
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
    throw 'Probe templates must use a real breed, not the breed-less empty-name Human form'
}
# The pool has no breeds of its own; every prototype resolves against the base
# mod's breed tree, so a missing install means 27 silently absent entities.
$breedRoot = Join-Path (Split-Path -Parent $WorkshopRoot) "3261086933\resource\set\breed\mp\nato\2022s"
foreach ($breed in @("usmc_rifleman", "1ad_rifleman", "pzgd_rifleman", "usarmy_crew")) {
    $breedSet = Join-Path $breedRoot ($breed + ".set")
    if (-not (Test-Path -LiteralPath $breedSet)) {
        throw "Breed mp/nato/2022s/$breed is not installed at: $breedSet"
    }
}

Write-Host "Deploying command-gated attack-mate wave engine (W1 30s / W2 90s / W3 150s)"
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
$probeInclude = '(include "../attack_mate_retask_probe.inc")'
$tplAnchor = '(include "../allied_support_templates.inc")'
$tplInclude = '(include "../attack_mate_probe_templates.inc")'
$waypointsAnchor = "`t`t{waypoints"
$entryName = '{"attack_mate_entry_'
$backupRoot = Join-Path $WorkshopRoot "_attack_mate_probe_backups"

foreach ($mapFile in $mapFiles) {
    $text = [System.IO.File]::ReadAllText($mapFile)
    $probeCount = ([regex]::Matches($text, [regex]::Escape($probeInclude))).Count

    if ($probeCount -eq 0) {
        if (-not $text.Contains($anchor)) {
            throw "Map is missing allied-support include anchor: $mapFile"
        }

        $relativeMap = $mapFile.Substring($WorkshopRoot.Length).TrimStart('\')
        $backup = Join-Path $backupRoot $relativeMap
        if (-not (Test-Path -LiteralPath $backup)) {
            New-Item -ItemType Directory -Force -Path (Split-Path $backup) | Out-Null
            Copy-Item -LiteralPath $mapFile -Destination $backup -Force
        }

        $replacement = $anchor + "`r`n`t`t`t" + $probeInclude
        $text = $text.Replace($anchor, $replacement)
        [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))
        $probeCount = 1
    }

    if ($probeCount -ne 1) {
        throw "Expected exactly one retask-probe include in: $mapFile"
    }

    # Entities-section include for the probe's own real-breed prototype pool,
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
        throw "Expected exactly one probe-templates include in: $mapFile"
    }

    # Attack-side entry waypoints, one per spawn side. The dynamic campaign swaps
    # attacker/defender spawns per mission instance, so the probe picks between
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

    # Self-healing and idempotent: strip every attack_mate_entry* block first -
    # including the superseded side-agnostic one written by earlier deploys - then
    # rebuild both sides from the repo. Rewriting beats trying to reconcile
    # whatever an interrupted earlier run happened to leave behind.
    $text = [System.IO.File]::ReadAllText($mapFile)
    $text = [regex]::Replace(
        $text,
        '\s*\{"attack_mate_entry[a-z_]*"\s*\r?\n\s*\{position [^}]*\}\s*\r?\n\s*\{radius \d+\}\s*\r?\n\s*\}',
        ''
    )
    foreach ($side in @("b", "a")) {
        $wpMatch = [regex]::Match($repoText, '\{"attack_mate_entry_' + $side + '"\s*\r?\n\s*\{position [^}]*\}\s*\r?\n\s*\{radius \d+\}\s*\r?\n\s*\}')
        if (-not $wpMatch.Success) {
            throw "Repo map is missing the attack_mate_entry_$side waypoint block: $repoMap"
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
        $n = ([regex]::Matches($text, [regex]::Escape('{"attack_mate_entry_' + $side + '"'))).Count
        if ($n -ne 1) {
            throw "Expected exactly one attack_mate_entry_$side waypoint in: $mapFile (found $n)"
        }
    }
    if ([regex]::IsMatch($text, '\{"attack_mate_entry"')) {
        throw "Map still carries the superseded single-sided attack_mate_entry: $mapFile"
    }
    if (([regex]::Matches($text, [regex]::Escape('{"allied_support_entry"'))).Count -ne 1) {
        throw "Map lost its allied_support_entry waypoint: $mapFile"
    }

    Write-Host "PATCHED $mapFile"
}

$gameSet = Join-Path $WorkshopRoot $files[0]
$botMain = Join-Path $WorkshopRoot $files[1]
$mate = Join-Path $WorkshopRoot $files[2]
$vars = Join-Path $WorkshopRoot $files[3]
$retask = Join-Path $WorkshopRoot $files[4]
$conquest = Join-Path $WorkshopRoot $files[6]

if (-not (Select-String -Quiet -LiteralPath $gameSet -SimpleMatch "{aiTeamPlayers 1}")) {
    throw "Workshop game set does not contain the attack-mate AI slot marker"
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
if (-not (Select-String -Quiet -LiteralPath $mate -SimpleMatch 'sc:SetVar("attack_mate_use_mi_probe", 1)')) {
    throw "Workshop attacker_mate.lua does not arm MI delivery, so no mate units will ever reach the map"
}
$mateTargetCode = Get-LuaCode $mate
foreach ($banned in $SlotUnsafe) {
    if ($mateTargetCode.Contains($banned)) {
        throw "Workshop attacker_mate.lua touches the slot-unsafe surface: $banned"
    }
}
foreach ($marker in @('{"attack_mate_probe_started"}', '{"attack_mate_wave_cmd"}', '{"attack_mate_use_mi_probe"}')) {
    if (-not (Select-String -Quiet -LiteralPath $vars -SimpleMatch $marker)) {
        throw "Workshop dcg_vars.inc is missing the attack-mate probe state: $marker"
    }
}
if (-not (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{var "user_is_defender$"}')) {
    throw "Workshop retask probe is missing the attack-only gate"
}
if (-not (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{var "attack_mate_wave_cmd$"}')) {
    throw "Workshop retask probe is missing the wave command gate - without it the waves all fire at once on entity presence"
}
foreach ($wave in @('{"attack_mate/wave1"', '{"attack_mate/wave2"', '{"attack_mate/wave3"')) {
    if (-not (Select-String -Quiet -LiteralPath $retask -SimpleMatch $wave)) {
        throw "Workshop retask probe is missing wave trigger: $wave"
    }
}
if (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{"delay" {time 8}}') {
    throw "Workshop retask probe still contains the superseded blind startup delay"
}
foreach ($n in 1..3) {
    if (-not (Select-String -Quiet -LiteralPath $retask -SimpleMatch ('{selector {tag attack_mate_w' + $n + '}}'))) {
        throw "Workshop retask probe does not gate wave $n on its own pool being present"
    }
}
if (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{clone}') {
    throw "Workshop retask probe still clones instead of moving the pool originals"
}
if (Select-String -Quiet -LiteralPath $retask -Pattern '^[^;]*\bfpc') {
    throw "Workshop retask probe still targets fpc* capture points"
}
if (-not (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{tag_add attack_mate_flag1}')) {
    throw "Workshop retask probe is not claiming a real flag point"
}
if (-not (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{group {select {tag {tag attack_mate_deploy}}}}')) {
    throw "Workshop retask probe is not using the proven bare select form"
}
if (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{tag attack_mate_probe}') {
    throw "Workshop retask probe still gates on the unproven attack_mate_probe marker"
}
if (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{zone {zone "gamezone"}}') {
    throw "Workshop retask probe still separates entities by zone"
}
$tplTarget = Join-Path $WorkshopRoot "resource\map\multi\attack_mate_probe_templates.inc"
if (-not (Select-String -Quiet -LiteralPath $tplTarget -SimpleMatch '{Human "mp/nato/2022s/usmc_rifleman" 0xaf25')) {
    throw "Workshop probe template pool is missing or not real-breed"
}
if (Select-String -Quiet -LiteralPath $tplTarget -Pattern '^\s*\{Human ""') {
    throw "Workshop probe template pool reverted to the breed-less empty-name Human form, which spawns unarmed bodies"
}
if ((Select-String -LiteralPath $tplTarget -SimpleMatch '{Able "-select"}').Count -ne 27) {
    throw "Workshop probe template pool is not the 27-prototype wave pool"
}
$poolTarget = Join-Path $WorkshopRoot "resource\map\multi\allied_support_templates.inc"
if (-not (Select-String -Quiet -LiteralPath $poolTarget -SimpleMatch '{Tags "allied_support_template" "hidden" "cmp_def" 0xaf01}')) {
    throw "Workshop off-map template pool is missing or untagged: allied_support_templates.inc"
}

Write-Host "`nVerification markers:"
Select-String -LiteralPath $gameSet -Pattern "aiTeamPlayers 1"
Select-String -LiteralPath $botMain -Pattern "CODEX_ATTACK_MATE_ROUTER|route_skip|first_player_slot|safeRequire"
Select-String -LiteralPath $mate -Pattern "CODEX_ATTACK_MATE|attack_mate_use_mi_probe"
Select-String -LiteralPath $vars -Pattern "attack_mate_probe_|attack_mate_wave_cmd|attack_mate_use_mi_probe"
Select-String -LiteralPath $retask -Pattern 'user_is_defender|attack_mate_wave_cmd|attack_mate/wave|ATTACK MATE WAVE'
Write-Host "Patched maps: $($mapFiles.Count)"

Write-Host "`nDeployment complete. Fully restart Gates of Hell before testing."
