param(
    [string]$RepoRoot = "",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799"
)

$ErrorActionPreference = "Stop"
$ExpectedBranch = "tune/motor-dismount-35s"
$BaselineCommit = "e74ef6e4a1977e0e7188c2f4a4f360080b7f8353"

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

& git -C $RepoRoot merge-base --is-ancestor $BaselineCommit HEAD
if ($LASTEXITCODE -ne 0) {
    throw "The test branch is not descended from the known-good baseline $BaselineCommit."
}

$sourceDeploy = Join-Path $RepoRoot "tools\deploy_attack_support_probe.ps1"
$timingOverlay = Join-Path $RepoRoot "tools\apply_known_good_motor_30s_test.py"
$placementOverlay = Join-Path $RepoRoot "tools\apply_motor_visible_package_overlay.py"
$lifecycleOverlay = Join-Path $RepoRoot "tools\apply_motor_lifecycle_tuning_overlay.py"
if (-not (Test-Path -LiteralPath $sourceDeploy)) {
    throw "Missing trusted deployer: $sourceDeploy"
}
if (-not (Test-Path -LiteralPath $timingOverlay)) {
    throw "Missing motor timing overlay: $timingOverlay"
}
if (-not (Test-Path -LiteralPath $placementOverlay)) {
    throw "Missing base-entry package placement overlay: $placementOverlay"
}
if (-not (Test-Path -LiteralPath $lifecycleOverlay)) {
    throw "Missing final motor lifecycle overlay: $lifecycleOverlay"
}

# The trusted e74ef6e deployer pins its historical experiment branch. Run an
# ephemeral copy with only that branch assertion changed; the deployment logic
# itself remains the known-good implementation. Keep the copy in tools so every
# relative path used by the historical deployer resolves exactly as it did in
# the successful session.
$deployText = [System.IO.File]::ReadAllText($sourceDeploy)
$oldBranchPin = '$ExpectedBranch = "experiment/attack-mate-slot-proof"'
$newBranchPin = '$ExpectedBranch = "tune/motor-dismount-35s"'
if (-not $deployText.Contains($oldBranchPin)) {
    throw "Trusted deployer branch pin was not found; refusing to guess."
}
$deployText = $deployText.Replace($oldBranchPin, $newBranchPin)

$tempDeploy = Join-Path $ScriptDirectory "deploy_known_good_motor_30s_inner.generated.ps1"
[System.IO.File]::WriteAllText($tempDeploy, $deployText, [System.Text.UTF8Encoding]::new($false))

try {
    & powershell -ExecutionPolicy Bypass -File $tempDeploy -RepoRoot $RepoRoot -WorkshopRoot $WorkshopRoot
    if ($LASTEXITCODE -ne 0) {
        throw "Known-good deployment failed with exit code $LASTEXITCODE."
    }
} finally {
    Remove-Item -LiteralPath $tempDeploy -Force -ErrorAction SilentlyContinue
}

$multiRoot = Join-Path $WorkshopRoot "resource\map\multi"

& python $timingOverlay --multi-root $multiRoot
if ($LASTEXITCODE -ne 0) {
    throw "The 30-second motor dispatch overlay failed with exit code $LASTEXITCODE."
}
& python $placementOverlay --multi-root $multiRoot
if ($LASTEXITCODE -ne 0) {
    throw "The base-entry package placement overlay failed with exit code $LASTEXITCODE."
}
& python $lifecycleOverlay --multi-root $multiRoot
if ($LASTEXITCODE -ne 0) {
    throw "The final motor lifecycle overlay failed with exit code $LASTEXITCODE."
}

& python $timingOverlay --multi-root $multiRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "The deployed 30-second motor dispatch overlay did not validate."
}
& python $placementOverlay --multi-root $multiRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "The deployed base-entry package placement overlay did not validate."
}
& python $lifecycleOverlay --multi-root $multiRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "The deployed final motor lifecycle overlay did not validate."
}

$manifest = Join-Path $WorkshopRoot "known_good_motor_30s_test.txt"
@(
    "branch=$ExpectedBranch"
    "baseline=$BaselineCommit"
    "friendly_attacker_truck=30s"
    "enemy_attacker_truck=30s_after_prep"
    "truck_count_per_active_motor_path=1"
    "motor_package_placement=whole_linked_package"
    "motor_spawn_waypoint=base_entry_centroid"
    "motor_passenger_dismount=45s_after_drive_order"
    "motor_withdrawal=return_to_original_base_entry"
    "motor_cleanup=90s_after_dismount"
    "off_map_rear_pads=disabled_for_motor"
    "recurring_motor_scheduler=disabled_by_consumed_budget"
    "attack_helicopter_test=off"
) | Set-Content -LiteralPath $manifest -Encoding UTF8

Write-Host ""
Write-Host "Known-good motor test deployed."
Write-Host "  Branch:   $ExpectedBranch"
Write-Host "  Baseline: $BaselineCommit"
Write-Host "  Workshop: $WorkshopRoot"
Write-Host "  Result:   one linked truck at +30s; dismount at 45s; return to base entry; cleanup after 90s"
