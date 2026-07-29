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
    "resource\map\multi\attack_mate_probe_templates.inc"
)

$gameSetSource = Join-Path $RepoRoot $files[0]
$botMainSource = Join-Path $RepoRoot $files[1]
$mateSource = Join-Path $RepoRoot $files[2]
$varsSource = Join-Path $RepoRoot $files[3]
$retaskSource = Join-Path $RepoRoot $files[4]
$tplSource = Join-Path $RepoRoot $files[5]

foreach ($source in @($gameSetSource, $botMainSource, $mateSource, $varsSource, $retaskSource, $tplSource)) {
    if (-not (Test-Path -LiteralPath $source)) {
        throw "Missing source file: $source"
    }
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
if (-not (Select-String -Quiet -LiteralPath $mateSource -SimpleMatch '"diagnostics_only"')) {
    throw "Source attacker_mate.lua is not the read-only checkpoint"
}
foreach ($marker in @(
    '{"attack_mate_probe_started"}',
    '{"attack_mate_probe_transferred"}',
    '{"attack_mate_probe_retasked"}'
)) {
    if (-not (Select-String -Quiet -LiteralPath $varsSource -SimpleMatch $marker)) {
        throw "Source dcg_vars.inc is missing marker: $marker"
    }
}
foreach ($marker in @(
    '{var "user_is_defender$"}',
    'ATTACK MATE PROBE ARMED CLONE TEST',
    '{target_waypoint "attack_mate_entry"}',
    '{player "3"}',
    'ATTACK MATE PROBE 3 LEG1 ORDERED',
    'ATTACK MATE PROBE 4 RETASKED TO FPC2',
    '{select {tag {tag attack_mate_tpl}}}',
    '{tag_add attack_mate_pool}',
    'ATTACK MATE PROBE FAIL NO POOL',
    # No-clone design: the pool originals are MOVED to the entry waypoint and
    # promoted in place, so they still carry the tag we put on them.
    '{tag_remove attack_mate_tpl}',
    '{group {select {tag {tag attack_mate_tpl}}}}',
    '{amount 4}',
    # Decorating an advanced selector zeroes the match on these breed-less
    # templates, so promote reuses the placement's proven bare form and the whole
    # downstream chain keys on attack_mate_src - the one tag proven queryable.
    '{group {select {tag {tag attack_mate_src}}}}',
    '{tag attack_mate_src} {type human}} {operation set}',
    '{selector {source standart} {tag attack_mate_src}}'
)) {
    if (-not (Select-String -Quiet -LiteralPath $retaskSource -SimpleMatch $marker)) {
        throw "Source retask probe is missing marker: $marker"
    }
}
if (Select-String -Quiet -LiteralPath $retaskSource -SimpleMatch '{tag attack_mate_probe}') {
    throw "Source retask probe still gates on attack_mate_probe. That tag is a best-effort marker only; key on attack_mate_src, which is proven queryable on these units"
}
if (Select-String -Quiet -LiteralPath $retaskSource -SimpleMatch '{tag_remove attack_mate_src}') {
    throw "Source retask probe removes attack_mate_src, but the entire downstream chain selects on it"
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
    '{Human "mp/nato/2022s/1ad_rifleman" 0xaf11',
    '{Human "mp/nato/2022s/1ad_rifleman" 0xaf14',
    '{Tags "attack_mate_tpl" "hidden" 0xaf11}',
    '{Tags "attack_mate_tpl" "hidden" 0xaf14}'
)) {
    if (-not (Select-String -Quiet -LiteralPath $tplSource -SimpleMatch $marker)) {
        throw "Source probe template pool is missing marker: $marker"
    }
}
# Line-anchored so the header's prose about {Inventory} does not trip this.
if (Select-String -Quiet -LiteralPath $tplSource -Pattern '^\s*\{Inventory') {
    throw "Probe templates must not bake an Inventory block - the breed supplies the loadout"
}
if (Select-String -Quiet -LiteralPath $tplSource -Pattern '^\s*\{Human ""') {
    throw 'Probe templates must use a real breed, not the breed-less empty-name Human form'
}
$breedSet = Join-Path (Split-Path -Parent $WorkshopRoot) "3261086933\resource\set\breed\mp\nato\2022s\1ad_rifleman.set"
if (-not (Test-Path -LiteralPath $breedSet)) {
    throw "Breed mp/nato/2022s/1ad_rifleman is not installed at: $breedSet"
}

Write-Host "Deploying readiness-gated attack-mate clone + retask proof"
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
$entryName = '{"attack_mate_entry"'
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

    # Attack-side entry waypoint. allied_support_entry is authored at the spawn_a
    # centroid (the DEFENDER's rear), so on an attack mission it drops the units in
    # enemy territory. The per-map replacement lives in the repo copy of the map;
    # copy that exact block across rather than recomputing coordinates here.
    $text = [System.IO.File]::ReadAllText($mapFile)
    $wpCount = ([regex]::Matches($text, [regex]::Escape($entryName))).Count
    if ($wpCount -eq 0) {
        $repoMap = Join-Path $RepoRoot ("resource\map\multi\" + (Split-Path -Leaf (Split-Path -Parent $mapFile)) + "\campaign_capture_the_flag.mi")
        if (-not (Test-Path -LiteralPath $repoMap)) {
            throw "No repo map to source the entry waypoint from: $repoMap"
        }
        $repoText = [System.IO.File]::ReadAllText($repoMap)
        $wpMatch = [regex]::Match($repoText, '\{"attack_mate_entry"\s*\r?\n\s*\{position [^}]*\}\s*\r?\n\s*\{radius \d+\}\s*\r?\n\s*\}')
        if (-not $wpMatch.Success) {
            throw "Repo map is missing the attack_mate_entry waypoint block: $repoMap"
        }
        if (-not $text.Contains($waypointsAnchor)) {
            throw "Map is missing the waypoints anchor: $mapFile"
        }
        # The repo block already carries the right indentation; only normalise its
        # line endings to CRLF to match the workshop map.
        $block = "`r`n`t`t`t" + ($wpMatch.Value -replace '\r?\n', "`r`n")
        $text = $text.Replace($waypointsAnchor, $waypointsAnchor + $block)
        [System.IO.File]::WriteAllText($mapFile, $text, [System.Text.UTF8Encoding]::new($false))
        $wpCount = 1
    }
    if ($wpCount -ne 1) {
        throw "Expected exactly one attack_mate_entry waypoint in: $mapFile"
    }

    Write-Host "PATCHED $mapFile"
}

$gameSet = Join-Path $WorkshopRoot $files[0]
$botMain = Join-Path $WorkshopRoot $files[1]
$mate = Join-Path $WorkshopRoot $files[2]
$vars = Join-Path $WorkshopRoot $files[3]
$retask = Join-Path $WorkshopRoot $files[4]

if (-not (Select-String -Quiet -LiteralPath $gameSet -SimpleMatch "{aiTeamPlayers 1}")) {
    throw "Workshop game set does not contain the attack-mate AI slot marker"
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
if (-not (Select-String -Quiet -LiteralPath $mate -SimpleMatch '"diagnostics_only"')) {
    throw "Workshop attacker_mate.lua is not the read-only checkpoint"
}
if (-not (Select-String -Quiet -LiteralPath $vars -SimpleMatch '{"attack_mate_probe_started"}')) {
    throw "Workshop dcg_vars.inc is missing the attack-mate probe state"
}
if (-not (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{var "user_is_defender$"}')) {
    throw "Workshop retask probe is missing the attack-only gate"
}
if (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{"delay" {time 8}}') {
    throw "Workshop retask probe still contains the superseded blind startup delay"
}
if (-not (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{tag_add attack_mate_pool}')) {
    throw "Workshop retask probe is not sourcing the off-map template pool"
}
if (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{clone}') {
    throw "Workshop retask probe still clones instead of moving the pool originals"
}
if (-not (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{group {select {tag {tag attack_mate_src}}}}')) {
    throw "Workshop retask probe is not using the proven bare select form"
}
if (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{tag attack_mate_probe}') {
    throw "Workshop retask probe still gates on the unproven attack_mate_probe marker"
}
if (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{zone {zone "gamezone"}}') {
    throw "Workshop retask probe still separates entities by zone"
}
$tplTarget = Join-Path $WorkshopRoot "resource\map\multi\attack_mate_probe_templates.inc"
if (-not (Select-String -Quiet -LiteralPath $tplTarget -SimpleMatch '{Human "mp/nato/2022s/1ad_rifleman" 0xaf11')) {
    throw "Workshop probe template pool is missing or not real-breed"
}
if (-not (Select-String -Quiet -LiteralPath $retask -SimpleMatch '{group {select {tag {tag attack_mate_tpl}}}}')) {
    throw "Workshop retask probe is not sourcing its own real-breed pool"
}
$poolTarget = Join-Path $WorkshopRoot "resource\map\multi\allied_support_templates.inc"
if (-not (Select-String -Quiet -LiteralPath $poolTarget -SimpleMatch '{Tags "allied_support_template" "hidden" "cmp_def" 0xaf01}')) {
    throw "Workshop off-map template pool is missing or untagged: allied_support_templates.inc"
}

Write-Host "`nVerification markers:"
Select-String -LiteralPath $gameSet -Pattern "aiTeamPlayers 1"
Select-String -LiteralPath $botMain -Pattern "CODEX_ATTACK_MATE_ROUTER|route_skip|first_player_slot|safeRequire"
Select-String -LiteralPath $mate -Pattern "CODEX_ATTACK_MATE_PROBE|diagnostics_only"
Select-String -LiteralPath $vars -Pattern "attack_mate_probe_"
Select-String -LiteralPath $retask -Pattern 'prep_inform|user_is_defender|ATTACK MATE PROBE|allied_support_entry|player "3"'
Write-Host "Patched maps: $($mapFiles.Count)"

Write-Host "`nDeployment complete. Fully restart Gates of Hell before testing."
