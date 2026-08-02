param(
    [string]$RepoRoot = "",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799"
)

$ErrorActionPreference = "Stop"
$ExpectedBranch = "recovery/runtime-proven-motor-60s"
$RuntimeProvenCommit = "38785d41db871dd989f72a64a532e62dfc1bb4dd"

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

& git -C $RepoRoot merge-base --is-ancestor $RuntimeProvenCommit HEAD
if ($LASTEXITCODE -ne 0) {
    throw "This branch is not descended directly from runtime-proven commit $RuntimeProvenCommit."
}

$knownGoodWrapper = Join-Path $RepoRoot "tools\deploy_known_good_motor_30s_test.ps1"
$timingOverlay = Join-Path $RepoRoot "tools\apply_runtime_proven_motor_60s.py"
if (-not (Test-Path -LiteralPath $knownGoodWrapper)) {
    throw "Missing exact PR #67 deployment wrapper: $knownGoodWrapper"
}
if (-not (Test-Path -LiteralPath $timingOverlay)) {
    throw "Missing timing-only overlay: $timingOverlay"
}

# Reuse the exact runtime-proven PR #67 deployment path. Change only its branch
# assertion in an ephemeral copy; its base deployment, forced +30s dispatch, and
# whole-package entry placement remain untouched.
$wrapperText = [System.IO.File]::ReadAllText($knownGoodWrapper)
$oldBranchPin = '$ExpectedBranch = "test/known-good-motor-30s"'
$newBranchPin = '$ExpectedBranch = "recovery/runtime-proven-motor-60s"'
if (-not $wrapperText.Contains($oldBranchPin)) {
    throw "The PR #67 wrapper branch assertion was not found; refusing to approximate it."
}
$wrapperText = $wrapperText.Replace($oldBranchPin, $newBranchPin)

$tempWrapper = Join-Path $ScriptDirectory "deploy_runtime_proven_motor_base.generated.ps1"
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
        throw "Exact PR #67 deployment failed with exit code $LASTEXITCODE."
    }
} finally {
    Remove-Item -LiteralPath $tempWrapper -Force -ErrorAction SilentlyContinue
}

$multiRoot = Join-Path $WorkshopRoot "resource\map\multi"
& python $timingOverlay --multi-root $multiRoot
if ($LASTEXITCODE -ne 0) {
    throw "Runtime-proven timing overlay failed with exit code $LASTEXITCODE."
}
& python $timingOverlay --multi-root $multiRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "Runtime-proven 60/90 timing did not validate after deployment."
}

$manifest = Join-Path $WorkshopRoot "runtime_proven_motor_60s.txt"
@(
    "branch=$ExpectedBranch"
    "runtime_proven_commit=$RuntimeProvenCommit"
    "architecture=exact_pr67_two_path_build"
    "coverage=friendly_attacker|enemy_attacker"
    "first_dispatch_seconds=30"
    "trucks_per_active_path=1"
    "ride_before_dismount_seconds=60"
    "post_dismount_cleanup_seconds=90"
    "placement=exact_pr67_whole_linked_package_at_base_entry"
    "ownership=exact_pr67"
    "seat_links=exact_pr67"
    "departure_order=exact_pr67_waypoint_0"
    "recurring_scheduler=disabled"
    "four_engine_expansion=absent"
) | Set-Content -LiteralPath $manifest -Encoding UTF8

Write-Host ""
Write-Host "Runtime-proven motor baseline deployed."
Write-Host "  Branch:      $ExpectedBranch"
Write-Host "  Proven base: $RuntimeProvenCommit"
Write-Host "  Coverage:    exact PR #67 friendly-attacker + enemy-attacker paths only"
Write-Host "  Timing:      first truck +30s; ride 60s; cleanup 90s"
Write-Host "  Expansion:   none"
