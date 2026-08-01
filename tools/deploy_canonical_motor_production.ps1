param(
    [string]$RepoRoot = "",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799",
    [ValidateSet(0, 1, 2, 3)]
    [int]$E2TestMode = 0
)

$ErrorActionPreference = "Stop"
$ExpectedBranch = "fix/motor-linked-seat-regression"
$SourceBranch = "experiment/attack-mate-slot-proof"

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

$baseDeployer = Join-Path $RepoRoot "tools\deploy_attack_support_probe.ps1"
$overlay = Join-Path $RepoRoot "tools\apply_canonical_motor_production_overlay.py"
$seatHotfix = Join-Path $RepoRoot "tools\apply_motor_linked_seat_hotfix.py"
if (-not (Test-Path -LiteralPath $baseDeployer)) {
    throw "Missing base support deployer: $baseDeployer"
}
if (-not (Test-Path -LiteralPath $overlay)) {
    throw "Missing canonical motor overlay: $overlay"
}
if (-not (Test-Path -LiteralPath $seatHotfix)) {
    throw "Missing motor isolation hotfix: $seatHotfix"
}

# Reuse the current four-engine deployer exactly, changing only its historical
# branch assertion in a temporary copy. Keeping the copy beside the source script
# preserves every relative path used by the deployer.
$deployText = [System.IO.File]::ReadAllText($baseDeployer)
$oldBranchPin = '$ExpectedBranch = "' + $SourceBranch + '"'
$newBranchPin = '$ExpectedBranch = "' + $ExpectedBranch + '"'
if (-not $deployText.Contains($oldBranchPin)) {
    throw "Base deployer branch assertion was not found; refusing to guess."
}
$deployText = $deployText.Replace($oldBranchPin, $newBranchPin)
$tempDeployer = Join-Path $ScriptDirectory "deploy_canonical_motor_inner.generated.ps1"
[System.IO.File]::WriteAllText($tempDeployer, $deployText, [System.Text.UTF8Encoding]::new($false))

try {
    & powershell -ExecutionPolicy Bypass -File $tempDeployer `
        -RepoRoot $RepoRoot `
        -WorkshopRoot $WorkshopRoot `
        -E2TestMode $E2TestMode
    if ($LASTEXITCODE -ne 0) {
        throw "Base four-engine deployment failed with exit code $LASTEXITCODE."
    }
} finally {
    Remove-Item -LiteralPath $tempDeployer -Force -ErrorAction SilentlyContinue
}

# Build the four-engine cadence/lifecycle first, then isolate the motor command
# and package tags LAST. The production overlay intentionally generates common
# structures; the runtime isolation pass removes all motor use of infantry
# wave_cmd/deploy selectors before the workshop build is tested.
& python $overlay --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) {
    throw "Canonical motor production overlay failed with exit code $LASTEXITCODE."
}
& python $overlay --root $WorkshopRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "Canonical motor production deployment did not validate before isolation."
}

& python $seatHotfix --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) {
    throw "Motor dispatch/staging isolation failed with exit code $LASTEXITCODE."
}
& python $seatHotfix --root $WorkshopRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "Motor dispatch/staging isolation did not validate."
}

$manifest = Join-Path $WorkshopRoot "canonical_motor_production.txt"
@(
    "branch=$ExpectedBranch"
    "quadrants=4"
    "factions=4"
    "packages_per_faction=4"
    "first_dispatch_seconds=30"
    "recurring_dispatch_seconds=180|240|300"
    "dispatch_lane=dedicated_motor_bit_per_engine"
    "infantry_wave_cmd_shared=false"
    "staging_lane=dedicated_motor_transfer_tag"
    "generic_infantry_deploy_tag_shared=false"
    "ride_before_dismount_seconds=60"
    "post_dismount_cleanup_seconds=90"
    "package_claim=full_linked_hull_crew_passengers"
    "placement=hull_only_at_base_entry_links_carry_occupants"
    "pre_drive_actor_state=hull_only"
    "cab_crew=driver_and_commander_remain_linked"
    "passenger_ai=post_emit_only"
    "withdrawal=return_to_original_base_entry"
) | Set-Content -LiteralPath $manifest -Encoding UTF8

Write-Host ""
Write-Host "Canonical motor isolation build deployed."
Write-Host "  Branch:      $ExpectedBranch"
Write-Host "  Workshop:    $WorkshopRoot"
Write-Host "  Coverage:    both sides, four factions, all four support scenarios"
Write-Host "  Cadence:     first truck +30s; subsequent trucks at random 180/240/300s"
Write-Host "  Lifecycle:   60s ride; passenger-only dismount; return; 90s cleanup"
Write-Host "  Isolation:   dedicated motor command + staging lane; no infantry selector overlap"
Write-Host "  Seats:       hull-only placement/drive state; linked cab and cargo preserved"
