param(
    [string]$RepoRoot = "",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799"
)

$ErrorActionPreference = "Stop"
$ExpectedBranch = "experiment/defense-transport-control-comparison"
$ValidatedBaseline = "b182a28c7675bd70b084970c4a685fac628d975f"
$RollbackCommit = "3f51eea3aeb76a7189dc85be1595e73013ec7100"
$RollbackBranch = "archive/four-quadrant-patrol-before-dropoff-2026-08-01"

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
$dropoff = Join-Path $RepoRoot "tools\apply_four_quadrant_transport_dropoff.py"
$perimeters = Join-Path $RepoRoot "tools\apply_transport_flag_perimeter_waypoints_fixed.py"
foreach ($required in @($stage2Wrapper, $dropoff, $perimeters)) {
    if (-not (Test-Path -LiteralPath $required)) {
        throw "Missing required transport deployment component: $required"
    }
}

# Build the validated linked-package base, all support includes, and the proven
# whole-package entry geometry. The overlay below disables every old timed motor
# dispatch and replaces it with one reversible perimeter drop-off per active side.
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

# Generate five fixed route slots per map. Each point is centred approximately
# 32m from a campaign flag and carries a 14m arrival radius. The drop-off engine
# uses one assigned point per truck and never orders the truck onto the flag post.
& python $perimeters --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) {
    throw "Transport flag-perimeter waypoint generation failed with exit code $LASTEXITCODE."
}
& python $perimeters --root $WorkshopRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "Transport flag-perimeter waypoints did not validate."
}

# Install one friendly and one enemy transport for either mission perspective.
# Passengers are held while linked. Near an active flag the truck stops, emits
# passengers, sends the squad forward, and receives no later movement order.
& python $dropoff --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) {
    throw "Four-quadrant perimeter drop-off overlay failed with exit code $LASTEXITCODE."
}
& python $dropoff --root $WorkshopRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "Four-quadrant perimeter drop-off deployment did not validate."
}

$manifest = Join-Path $WorkshopRoot "four_quadrant_transport_dropoff.txt"
@(
    "branch=$ExpectedBranch"
    "validated_baseline=$ValidatedBaseline"
    "rollback_commit=$RollbackCommit"
    "rollback_branch=$RollbackBranch"
    "coverage=player_attack_friendly|player_attack_enemy|player_defense_friendly|player_defense_enemy"
    "active_transport_count_per_mission=2"
    "factions=rusa|ukr|nato|prc"
    "rusa_vehicle=ural"
    "ukr_vehicle=ural_vsu"
    "nato_vehicle=fmtv"
    "prc_vehicle=shaanxi_sx2190_passenger"
    "spawn_seconds_after_engine_arm=45"
    "route_behavior=one_perimeter_dropoff_only"
    "route_waypoint_offset_map_units=320"
    "route_waypoint_radius_map_units=140"
    "arrival_detection_distance_map_units=500"
    "passenger_ai_held_until_dropoff=true"
    "dismount=passenger_emit_at_flag_perimeter"
    "infantry_order=advance_to_active_flag"
    "truck_after_dropoff=stationary_normal_combat_ai"
    "scripted_turnaround=false"
    "scripted_return=false"
    "scripted_delete=false"
    "scripted_cleanup=false"
) | Set-Content -LiteralPath $manifest -Encoding UTF8

Write-Host ""
Write-Host "Four-quadrant perimeter drop-off test deployed."
Write-Host "  Coverage:    friendly + enemy trucks on player attack and player defense"
Write-Host "  Factions:    Russia, Ukraine, NATO, PRC"
Write-Host "  Route:       one flag-perimeter destination per truck; no rotation"
Write-Host "  Passengers:  held while linked, dismounted on perimeter arrival"
Write-Host "  Infantry:    advances toward an active flag after dismount"
Write-Host "  Truck:       stops and remains near the squad"
Write-Host "  Removed:     continued patrol, turnaround, return-to-edge, deletion, cleanup"
Write-Host "  Rollback:    $RollbackBranch at $RollbackCommit"
Write-Host "  Total:       exactly two active transport trucks per mission"
