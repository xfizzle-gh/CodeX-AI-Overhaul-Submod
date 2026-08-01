param(
    [string]$RepoRoot = "",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799"
)

$ErrorActionPreference = "Stop"
$ExpectedBranch = "feature/motor-friendly-defender-one-shot"
$ValidatedBaseline = "b182a28c7675bd70b084970c4a685fac628d975f"

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
    throw "Wrong source branch '$branch'. Switch to '$ExpectedBranch', pull origin, and run again."
}

& git -C $RepoRoot merge-base --is-ancestor $ValidatedBaseline HEAD
if ($LASTEXITCODE -ne 0) {
    throw "This branch is not descended from runtime-validated baseline $ValidatedBaseline."
}

$baselineWrapper = Join-Path $RepoRoot "tools\deploy_runtime_proven_motor_60s.ps1"
$overlay = Join-Path $RepoRoot "tools\apply_friendly_defender_motor_one_shot.py"
if (-not (Test-Path -LiteralPath $baselineWrapper)) {
    throw "Missing runtime-proven baseline deployer: $baselineWrapper"
}
if (-not (Test-Path -LiteralPath $overlay)) {
    throw "Missing friendly defender motor overlay: $overlay"
}

# Run the exact validated baseline deployer first. The only textual change in this
# ephemeral copy is the expected branch name, including the nested PR #67 wrapper
# assertion it generates internally.
$wrapperText = [System.IO.File]::ReadAllText($baselineWrapper)
$oldBranch = "recovery/runtime-proven-motor-60s"
if (-not $wrapperText.Contains($oldBranch)) {
    throw "Validated baseline branch assertion was not found; refusing to approximate it."
}
$wrapperText = $wrapperText.Replace($oldBranch, $ExpectedBranch)
$tempWrapper = Join-Path $ScriptDirectory "deploy_friendly_defender_base.generated.ps1"
[System.IO.File]::WriteAllText(
    $tempWrapper,
    $wrapperText,
    [System.Text.UTF8Encoding]::new($true)
)

try {
    & powershell -ExecutionPolicy Bypass -File $tempWrapper `
        -RepoRoot $RepoRoot `
        -WorkshopRoot $WorkshopRoot
    if ($LASTEXITCODE -ne 0) {
        throw "Runtime-proven baseline deployment failed with exit code $LASTEXITCODE."
    }
} finally {
    Remove-Item -LiteralPath $tempWrapper -Force -ErrorAction SilentlyContinue
}

$attackPath = Join-Path $WorkshopRoot "resource\map\multi\attack_support_waves.inc"
$enemyPath = Join-Path $WorkshopRoot "resource\map\multi\enemy_attack_support.inc"
$attackBefore = (Get-FileHash -LiteralPath $attackPath -Algorithm SHA256).Hash
$enemyBefore = (Get-FileHash -LiteralPath $enemyPath -Algorithm SHA256).Hash

& python $overlay --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) {
    throw "Friendly defender motor overlay failed with exit code $LASTEXITCODE."
}
& python $overlay --root $WorkshopRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "Friendly defender motor deployment did not validate."
}

$attackAfter = (Get-FileHash -LiteralPath $attackPath -Algorithm SHA256).Hash
$enemyAfter = (Get-FileHash -LiteralPath $enemyPath -Algorithm SHA256).Hash
if ($attackAfter -ne $attackBefore) {
    throw "Stage 2 changed the runtime-proven friendly-attacker engine."
}
if ($enemyAfter -ne $enemyBefore) {
    throw "Stage 2 changed the runtime-proven enemy-attacker engine."
}

$manifest = Join-Path $WorkshopRoot "friendly_defender_motor_one_shot.txt"
@(
    "branch=$ExpectedBranch"
    "validated_baseline=$ValidatedBaseline"
    "test_mission=player_defense"
    "new_path=friendly_defender"
    "first_dispatch_seconds=30_after_defense_engine_arms"
    "trucks_on_new_path=1"
    "ride_before_dismount_seconds=60"
    "post_dismount_cleanup_seconds=90"
    "lifecycle_source=runtime_proven_friendly_attacker_blocks"
    "ownership=defenderbot"
    "existing_friendly_attacker_changed=false"
    "existing_enemy_attacker_changed=false"
    "recurring_scheduler=disabled"
    "enemy_defender_path=absent"
) | Set-Content -LiteralPath $manifest -Encoding UTF8

Write-Host ""
Write-Host "Friendly defender motor one-shot deployed."
Write-Host "  Branch:     $ExpectedBranch"
Write-Host "  Test mode:  PLAYER DEFENSE"
Write-Host "  New path:   one friendly defender truck at +30s"
Write-Host "  Lifecycle:  60s drive; passenger-only emit; 90s cleanup"
Write-Host "  Preserved:  both previously validated motor engines are byte-identical"
