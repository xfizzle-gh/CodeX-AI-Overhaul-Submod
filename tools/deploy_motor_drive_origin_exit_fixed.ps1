param(
    [string]$RepoRoot = "",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799"
)

$ErrorActionPreference = "Stop"
$ExpectedBranch = "fix/enemy-motor-drive-origin-exit"
$Stage2Head = "18a714ee3445a4c193e8ad2abf5e5597ece4ab51"

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

& git -C $RepoRoot merge-base --is-ancestor $Stage2Head HEAD
if ($LASTEXITCODE -ne 0) {
    throw "This branch is not descended from runtime-tested friendly-defender stage $Stage2Head."
}

$stage2Wrapper = Join-Path $RepoRoot "tools\deploy_friendly_defender_motor_one_shot.ps1"
$correction = Join-Path $RepoRoot "tools\apply_motor_drive_origin_exit_fixed.py"
if (-not (Test-Path -LiteralPath $stage2Wrapper)) {
    throw "Missing friendly-defender stage deployer: $stage2Wrapper"
}
if (-not (Test-Path -LiteralPath $correction)) {
    throw "Missing indentation-independent motor drive/exit correction: $correction"
}

# Rebuild the exact runtime-tested Stage-2 package first. The temporary copy
# changes only its branch assertion so it can run from this stacked branch.
$wrapperText = [System.IO.File]::ReadAllText($stage2Wrapper)
$oldBranch = "feature/motor-friendly-defender-one-shot"
if (-not $wrapperText.Contains($oldBranch)) {
    throw "Stage-2 branch assertion was not found; refusing to approximate its deploy path."
}
$wrapperText = $wrapperText.Replace($oldBranch, $ExpectedBranch)
$tempWrapper = Join-Path $ScriptDirectory "deploy_motor_drive_exit_base.generated.ps1"
[System.IO.File]::WriteAllText(
    $tempWrapper,
    $wrapperText,
    [System.Text.UTF8Encoding]::new($false)
)

try {
    & powershell -ExecutionPolicy Bypass -File $tempWrapper `
        -RepoRoot $RepoRoot `
        -WorkshopRoot $WorkshopRoot
    if ($LASTEXITCODE -ne 0) {
        throw "Friendly-defender Stage-2 deployment failed with exit code $LASTEXITCODE."
    }
} finally {
    Remove-Item -LiteralPath $tempWrapper -Force -ErrorAction SilentlyContinue
}

& python $correction --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) {
    throw "Motor drive/exit correction failed with exit code $LASTEXITCODE."
}
& python $correction --root $WorkshopRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "Motor drive/exit correction did not validate."
}

$manifest = Join-Path $WorkshopRoot "motor_drive_origin_exit.txt"
@(
    "branch=$ExpectedBranch"
    "stacked_on_stage2=$Stage2Head"
    "test_mission=player_defense"
    "friendly_defender_runtime_result=core_lifecycle_passed"
    "enemy_attacker_change=drive_order_retry_after_2s"
    "enemy_total_ride_seconds=60"
    "enemy_retry_split=2s_plus_58s"
    "friendly_defender_exit=origin_entry_edge"
    "enemy_attacker_exit=origin_entry_edge"
    "friendly_attacker_exit=origin_entry_edge"
    "generic_waypoint_0_removed=true"
    "passenger_emit=unchanged"
    "linked_package_placement=unchanged"
    "cleanup_seconds=90"
    "recurring_scheduler=disabled"
) | Set-Content -LiteralPath $manifest -Encoding UTF8

Write-Host ""
Write-Host "Motor drive/exit correction deployed."
Write-Host "  Branch:          $ExpectedBranch"
Write-Host "  Test mode:       PLAYER DEFENSE"
Write-Host "  Enemy truck:     advance order reasserted after 2s; dismount remains at 60s"
Write-Host "  Empty trucks:    return to their own insertion edge, not waypoint 0"
Write-Host "  Preserved:       whole-package placement, seated cargo, passenger-only emit, 90s cleanup"
