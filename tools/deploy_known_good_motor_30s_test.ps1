param(
    [string]$RepoRoot = "",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799"
)

$ErrorActionPreference = "Stop"
$ExpectedBranch = "test/known-good-motor-30s"
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
$overlay = Join-Path $RepoRoot "tools\apply_known_good_motor_30s_test.py"
if (-not (Test-Path -LiteralPath $sourceDeploy)) {
    throw "Missing trusted deployer: $sourceDeploy"
}
if (-not (Test-Path -LiteralPath $overlay)) {
    throw "Missing motor test overlay: $overlay"
}

# The trusted e74ef6e deployer pins its historical experiment branch. Run an
# ephemeral copy with only that branch assertion changed; the deployment logic
# itself remains byte-for-byte the known-good implementation. Keep the copy in
# tools so every relative path used by the historical deployer resolves exactly
# as it did in the successful session.
$deployText = [System.IO.File]::ReadAllText($sourceDeploy)
$oldBranchPin = '$ExpectedBranch = "experiment/attack-mate-slot-proof"'
$newBranchPin = '$ExpectedBranch = "test/known-good-motor-30s"'
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
& python $overlay --multi-root $multiRoot
if ($LASTEXITCODE -ne 0) {
    throw "The 30-second motor overlay failed with exit code $LASTEXITCODE."
}
& python $overlay --multi-root $multiRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "The deployed 30-second motor overlay did not validate."
}

$manifest = Join-Path $WorkshopRoot "known_good_motor_30s_test.txt"
@(
    "branch=$ExpectedBranch"
    "baseline=$BaselineCommit"
    "friendly_attacker_truck=30s"
    "enemy_attacker_truck=30s_after_prep"
    "truck_count_per_active_motor_path=1"
    "recurring_motor_scheduler=disabled_by_consumed_budget"
    "attack_helicopter_test=off"
) | Set-Content -LiteralPath $manifest -Encoding UTF8

Write-Host ""
Write-Host "Known-good motor test deployed."
Write-Host "  Branch:   $ExpectedBranch"
Write-Host "  Baseline: $BaselineCommit"
Write-Host "  Workshop: $WorkshopRoot"
Write-Host "  Result:   one truck at +30 seconds in each proven attacker path"
