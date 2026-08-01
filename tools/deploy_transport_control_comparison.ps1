param(
    [string]$RepoRoot = "",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799"
)

$ErrorActionPreference = "Stop"
$ExpectedBranch = "experiment/defense-transport-control-comparison"
$ComparisonBase = "2e095596ada5ddc9d51b6fa2f28fe22ba1bf34cb"

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

& git -C $RepoRoot merge-base --is-ancestor $ComparisonBase HEAD
if ($LASTEXITCODE -ne 0) {
    throw "This branch is not descended from tested transport correction $ComparisonBase."
}

$baseWrapper = Join-Path $RepoRoot "tools\deploy_motor_drive_origin_exit_fixed.ps1"
$comparison = Join-Path $RepoRoot "tools\apply_transport_control_comparison_fixed.py"
if (-not (Test-Path -LiteralPath $baseWrapper)) {
    throw "Missing scripted transport deployer: $baseWrapper"
}
if (-not (Test-Path -LiteralPath $comparison)) {
    throw "Missing normal transport comparison overlay: $comparison"
}

# Rebuild the tested scripted stack first. The temporary copy changes only its
# branch assertion; all source paths resolve against this experiment branch, so
# the current turnaround-first and infantry-reassert overlay is deployed.
$wrapperText = [System.IO.File]::ReadAllText($baseWrapper)
$oldBranch = "fix/enemy-motor-drive-origin-exit"
if (-not $wrapperText.Contains($oldBranch)) {
    throw "Base wrapper branch assertion was not found; refusing to approximate deployment."
}
$wrapperText = $wrapperText.Replace($oldBranch, $ExpectedBranch)
$tempWrapper = Join-Path $ScriptDirectory "deploy_transport_comparison_base.generated.ps1"
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
        throw "Scripted transport stack failed with exit code $LASTEXITCODE."
    }
} finally {
    Remove-Item -LiteralPath $tempWrapper -Force -ErrorAction SilentlyContinue
}

# Add one independent normal-combat transport for each side. These controls have
# no scripted emit, turnaround, withdrawal, or cleanup. They receive one normal
# advance order toward a flag and remain under ordinary AI behavior.
& python $comparison --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) {
    throw "Normal transport comparison overlay failed with exit code $LASTEXITCODE."
}
& python $comparison --root $WorkshopRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "Normal transport comparison overlay did not validate."
}

$manifest = Join-Path $WorkshopRoot "transport_control_comparison.txt"
@(
    "branch=$ExpectedBranch"
    "stacked_on=$ComparisonBase"
    "test_mission=player_defense"
    "required_matchup=friendly_nato_vs_enemy_rusa"
    "scripted_friendly_spawn_seconds=30"
    "scripted_enemy_spawn_seconds=15"
    "scripted_turnaround_seconds=75"
    "scripted_turn_before_emit=true"
    "scripted_stop_dwell_seconds=1"
    "scripted_passenger_emit=passengers_only"
    "scripted_infantry_order_reasserted_after_hull_exit=true"
    "control_friendly_vehicle=fmtv"
    "control_enemy_vehicle=ural"
    "control_spawn_seconds=45"
    "control_order=single_advance_to_active_flag"
    "control_scripted_emit=false"
    "control_scripted_return=false"
    "control_scripted_cleanup=false"
) | Set-Content -LiteralPath $manifest -Encoding UTF8

Write-Host ""
Write-Host "Transport comparison deployed."
Write-Host "  Mission:          PLAYER DEFENSE - NATO versus RUSSIA"
Write-Host "  Scripted trucks:  turn at 75s, pause, dismount, return; infantry attack reasserted"
Write-Host "  Control trucks:   NATO FMTV + Russian Ural at +45s"
Write-Host "  Control behavior: one normal advance order; no scripted dismount or return"
Write-Host "  Expected total:   two NATO trucks and two Russian trucks"
