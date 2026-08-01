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
$quadrants = Join-Path $RepoRoot "tools\apply_four_quadrant_transport_patrol.py"
$perimeters = Join-Path $RepoRoot "tools\apply_transport_flag_perimeter_waypoints.py"
foreach ($required in @($stage2Wrapper, $quadrants, $perimeters)) {
    if (-not (Test-Path -LiteralPath $required)) {
        throw "Missing required transport deployment component: $required"
    }
}

# Build the validated linked-package base, all support includes, and the proven
# whole-package entry geometry. The production overlay below disables every old
# timer-driven motor dispatch and replaces it with ordinary AI combat transports.
$wrapperText = [System.IO.File]::ReadAllText($stage2Wrapper)
$oldBranch = "feature/motor-friendly-defender-one-shot"
if (-not $wrapperText.Contains($oldBranch)) {
    throw "Stage-2 branch assertion was not found; refusing to approximate deployment."
}
$wrapperText = $wrapperText.Replace($oldBranch, $ExpectedBranch)
$tempWrapper = Join-Path $ScriptDirectory "deploy_four_quadrant_transport_base.generated.ps1"
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

# Generate five fixed route slots per map. Every slot is centred approximately
# 32m from a campaign flag and carries a 14m arrival radius, so no transport is
# ever ordered directly onto the sandbag post or flag entity.
& python $perimeters --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) {
    throw "Transport flag-perimeter waypoint generation failed with exit code $LASTEXITCODE."
}
& python $perimeters --root $WorkshopRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "Transport flag-perimeter waypoints did not validate."
}

# Install one ordinary friendly and one ordinary enemy transport for either
# mission perspective. The four support engines are mutually gated, so exactly
# two trucks can activate in one battle. Normal game AI owns all dismount timing.
& python $quadrants --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) {
    throw "Four-quadrant normal transport overlay failed with exit code $LASTEXITCODE."
}
& python $quadrants --root $WorkshopRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "Four-quadrant normal transport deployment did not validate."
}

$manifest = Join-Path $WorkshopRoot "four_quadrant_normal_transport_patrol.txt"
@(
    "branch=$ExpectedBranch"
    "validated_baseline=$ValidatedBaseline"
    "coverage=player_attack_friendly|player_attack_enemy|player_defense_friendly|player_defense_enemy"
    "active_transport_count_per_mission=2"
    "factions=rusa|ukr|nato|prc"
    "rusa_vehicle=ural"
    "ukr_vehicle=ural_vsu"
    "nato_vehicle=fmtv"
    "prc_vehicle=shaanxi_sx2190_passenger"
    "spawn_seconds_after_engine_arm=45"
    "route_waypoints=5"
    "route_waypoint_offset_map_units=320"
    "route_waypoint_radius_map_units=140"
    "minimum_requested_distance_from_flag_map_units=180"
    "route_interval_seconds=45"
    "route_continues_while_passengers_within_map_units=80"
    "dismount=engine_controlled"
    "scripted_emit=false"
    "scripted_turnaround=false"
    "scripted_return=false"
    "scripted_delete=false"
    "scripted_cleanup=false"
) | Set-Content -LiteralPath $manifest -Encoding UTF8

Write-Host ""
Write-Host "Four-quadrant normal transport patrol deployed."
Write-Host "  Coverage:    friendly + enemy trucks on player attack and player defense"
Write-Host "  Factions:    Russia, Ukraine, NATO, PRC"
Write-Host "  Route:       five flag-perimeter waypoints, never the flag post itself"
Write-Host "  Contact:     patrol re-orders stop after passengers leave the truck"
Write-Host "  Dismount:    ordinary game AI under fire"
Write-Host "  Removed:     timed emit, forced turnaround, return-to-edge, deletion, cleanup"
Write-Host "  Total:       exactly two active transport trucks per mission"
