param(
    [string]$RepoRoot = "",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799"
)

$ErrorActionPreference = "Stop"
$ExpectedBranch = "experiment/defense-transport-control-comparison"
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

$stage2Wrapper = Join-Path $RepoRoot "tools\deploy_friendly_defender_motor_one_shot.ps1"
$normalOnly = Join-Path $RepoRoot "tools\apply_normal_transport_only.py"
if (-not (Test-Path -LiteralPath $stage2Wrapper)) {
    throw "Missing defender transport helper deployment: $stage2Wrapper"
}
if (-not (Test-Path -LiteralPath $normalOnly)) {
    throw "Missing normal transport-only overlay: $normalOnly"
}

# Build the validated linked-package base and defender-side placement/ownership
# helpers. The normal-only overlay then removes both timer-driven dispatches.
$wrapperText = [System.IO.File]::ReadAllText($stage2Wrapper)
$oldBranch = "feature/motor-friendly-defender-one-shot"
if (-not $wrapperText.Contains($oldBranch)) {
    throw "Stage-2 branch assertion was not found; refusing to approximate deployment."
}
$wrapperText = $wrapperText.Replace($oldBranch, $ExpectedBranch)
$tempWrapper = Join-Path $ScriptDirectory "deploy_normal_transport_base.generated.ps1"
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
        throw "Linked transport base deployment failed with exit code $LASTEXITCODE."
    }
} finally {
    Remove-Item -LiteralPath $tempWrapper -Force -ErrorAction SilentlyContinue
}

# Replace the experiment with exactly two ordinary AI combat transports:
# one friendly NATO FMTV and one enemy Russian Ural. No scripted dismount,
# turnaround, return-to-edge, deletion, or cleanup remains active.
& python $normalOnly --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) {
    throw "Normal transport-only overlay failed with exit code $LASTEXITCODE."
}
& python $normalOnly --root $WorkshopRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "Normal transport-only deployment did not validate."
}

$manifest = Join-Path $WorkshopRoot "normal_transport_only.txt"
@(
    "branch=$ExpectedBranch"
    "validated_baseline=$ValidatedBaseline"
    "test_mission=player_defense"
    "required_matchup=friendly_nato_vs_enemy_rusa"
    "active_transport_count=2"
    "friendly_transport=fmtv"
    "enemy_transport=ural"
    "spawn_seconds=45"
    "order=single_advance_to_active_flag"
    "passengers=linked_and_seated"
    "dismount=engine_controlled"
    "scripted_emit=false"
    "scripted_turnaround=false"
    "scripted_return=false"
    "scripted_delete=false"
    "scripted_cleanup=false"
    "enemy_scripted_motor_budget=0"
) | Set-Content -LiteralPath $manifest -Encoding UTF8

Write-Host ""
Write-Host "Normal transport-only test deployed."
Write-Host "  Mission:     PLAYER DEFENSE - NATO versus RUSSIA"
Write-Host "  Friendly:    one loaded NATO FMTV"
Write-Host "  Enemy:       one loaded Russian Ural"
Write-Host "  Behavior:    ordinary AI advance; engine-controlled dismount under fire"
Write-Host "  Removed:     timed emit, forced turnaround, return-to-edge, deletion, cleanup"
Write-Host "  Total:       exactly two transport trucks"
