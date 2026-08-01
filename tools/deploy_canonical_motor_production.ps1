param(
    [string]$RepoRoot = "",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799",
    [ValidateSet(0, 1, 2, 3)]
    [int]$E2TestMode = 0
)

$ErrorActionPreference = "Stop"
$ExpectedBranch = "feature/canonical-motor-all-quadrants"
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
if (-not (Test-Path -LiteralPath $baseDeployer)) {
    throw "Missing base support deployer: $baseDeployer"
}
if (-not (Test-Path -LiteralPath $overlay)) {
    throw "Missing canonical motor overlay: $overlay"
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

& python $overlay --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) {
    throw "Canonical motor production overlay failed with exit code $LASTEXITCODE."
}
& python $overlay --root $WorkshopRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "Canonical motor production deployment did not validate."
}

$manifest = Join-Path $WorkshopRoot "canonical_motor_production.txt"
@(
    "branch=$ExpectedBranch"
    "quadrants=4"
    "factions=4"
    "packages_per_faction=4"
    "first_dispatch_seconds=30"
    "recurring_dispatch_seconds=180|240|300"
    "ride_before_dismount_seconds=60"
    "post_dismount_cleanup_seconds=90"
    "placement=whole_linked_package_at_base_entry"
    "withdrawal=return_to_original_base_entry"
) | Set-Content -LiteralPath $manifest -Encoding UTF8

Write-Host ""
Write-Host "Canonical motor production build deployed."
Write-Host "  Branch:      $ExpectedBranch"
Write-Host "  Workshop:    $WorkshopRoot"
Write-Host "  Coverage:    both sides, four factions, all four support scenarios"
Write-Host "  Cadence:     first truck +30s; subsequent trucks at random 180/240/300s"
Write-Host "  Lifecycle:   60s ride; passenger-only dismount; return; 90s cleanup"
