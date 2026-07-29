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
    "resource\script\multiplayer\modes\attacker_mate.lua"
)

$gameSetSource = Join-Path $RepoRoot $files[0]
$botMainSource = Join-Path $RepoRoot $files[1]
$mateSource = Join-Path $RepoRoot $files[2]

foreach ($source in @($gameSetSource, $botMainSource, $mateSource)) {
    if (-not (Test-Path -LiteralPath $source)) {
        throw "Missing source file: $source"
    }
}

# Refuse to deploy anything except the live-proven manual-transfer checkpoint.
# The transferable Team A process reports the same ID as FirstPlayerId. Loading
# a Lua controller on that process caused the native crash; skipping it leaves
# the ownership slot alive and allows engine-level manual transfer.
if (-not (Select-String -Quiet -LiteralPath $gameSetSource -SimpleMatch "{aiTeamPlayers 1}")) {
    throw "Source game set does not contain the attack-mate AI slot marker"
}
if (-not (Select-String -Quiet -LiteralPath $botMainSource -SimpleMatch 'route_skip", "first_player_slot')) {
    throw "Source bot.main.lua is not the proven first-player-slot checkpoint"
}
if (-not (Select-String -Quiet -LiteralPath $botMainSource -SimpleMatch "identity.firstPlayerId > 0 and identity.playerId == identity.firstPlayerId")) {
    throw "Source bot.main.lua is missing the proven FirstPlayerId isolation gate"
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

Write-Host "Deploying proven attack-mate manual-transfer checkpoint"
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

$gameSet = Join-Path $WorkshopRoot $files[0]
$botMain = Join-Path $WorkshopRoot $files[1]
$mate = Join-Path $WorkshopRoot $files[2]

if (-not (Select-String -Quiet -LiteralPath $gameSet -SimpleMatch "{aiTeamPlayers 1}")) {
    throw "Workshop game set does not contain the attack-mate AI slot marker"
}
if (-not (Select-String -Quiet -LiteralPath $botMain -SimpleMatch 'route_skip", "first_player_slot')) {
    throw "Workshop bot.main.lua does not contain the proven first-player-slot skip"
}
if (-not (Select-String -Quiet -LiteralPath $botMain -SimpleMatch "identity.firstPlayerId > 0 and identity.playerId == identity.firstPlayerId")) {
    throw "Workshop bot.main.lua is missing the proven FirstPlayerId isolation gate"
}
if (Select-String -Quiet -LiteralPath $botMain -SimpleMatch "team_a_attack_safe_route") {
    throw "Workshop bot.main.lua still contains the superseded crashing Team A route"
}
if (-not (Select-String -Quiet -LiteralPath $mate -SimpleMatch '"diagnostics_only"')) {
    throw "Workshop attacker_mate.lua is not the read-only checkpoint"
}

Write-Host "`nVerification markers:"
Select-String -LiteralPath $gameSet -Pattern "aiTeamPlayers 1"
Select-String -LiteralPath $botMain -Pattern "CODEX_ATTACK_MATE_ROUTER|route_skip|first_player_slot|safeRequire"
Select-String -LiteralPath $mate -Pattern "CODEX_ATTACK_MATE_PROBE|diagnostics_only"

Write-Host "`nDeployment complete. Fully restart Gates of Hell before testing."
