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
$timing75 = Join-Path $RepoRoot "tools\apply_defense_motor_turnaround.py"
if (-not (Test-Path -LiteralPath $stage2Wrapper)) { throw "Missing friendly-defender stage deployer: $stage2Wrapper" }
if (-not (Test-Path -LiteralPath $correction)) { throw "Missing tested motor drive/exit correction: $correction" }
if (-not (Test-Path -LiteralPath $timing75)) { throw "Missing turnaround-first 75-second overlay: $timing75" }

# Rebuild the exact Stage-2 package used by the successful runtime test.
$wrapperText = [System.IO.File]::ReadAllText($stage2Wrapper)
$oldBranch = "feature/motor-friendly-defender-one-shot"
if (-not $wrapperText.Contains($oldBranch)) {
    throw "Stage-2 branch assertion was not found; refusing to approximate its deploy path."
}
$wrapperText = $wrapperText.Replace($oldBranch, $ExpectedBranch)
$tempWrapper = Join-Path $ScriptDirectory "deploy_motor_drive_exit_base.generated.ps1"
[System.IO.File]::WriteAllText($tempWrapper, $wrapperText, [System.Text.UTF8Encoding]::new($false))
try {
    & powershell -ExecutionPolicy Bypass -File $tempWrapper -RepoRoot $RepoRoot -WorkshopRoot $WorkshopRoot
    if ($LASTEXITCODE -ne 0) { throw "Friendly-defender Stage-2 deployment failed with exit code $LASTEXITCODE." }
} finally {
    Remove-Item -LiteralPath $tempWrapper -Force -ErrorAction SilentlyContinue
}

# Reapply the exact movement retry and origin-side exits that passed runtime.
& python $correction --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) { throw "Motor drive/exit correction failed with exit code $LASTEXITCODE." }
& python $correction --root $WorkshopRoot --check
if ($LASTEXITCODE -ne 0) { throw "Motor drive/exit correction did not validate." }

# Preserve the tested package and ownership lifecycle. At 75 seconds, begin the
# origin turn before emit, pause, dismount, resume withdrawal, then reassert the
# infantry attack order after the truck's final return command.
& python $timing75 --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) { throw "Turnaround-first drop alignment failed with exit code $LASTEXITCODE." }
& python $timing75 --root $WorkshopRoot --check
if ($LASTEXITCODE -ne 0) { throw "Turnaround-first drop alignment did not validate." }

$manifest = Join-Path $WorkshopRoot "motor_drive_origin_exit.txt"
@(
    "branch=$ExpectedBranch"
    "stacked_on_stage2=$Stage2Head"
    "test_mission=player_defense"
    "runtime_baseline=game51_seated_both_trucks_and_origin_return"
    "enemy_total_ride_seconds=75"
    "enemy_retry_split=2s_plus_73s"
    "friendly_defender_total_ride_seconds=75"
    "friendly_attacker_total_ride_seconds=60"
    "origin_turn_started_before_emit=true"
    "turn_dwell_seconds=0.5"
    "hull_stop_dwell_seconds=1"
    "passenger_emit=unchanged"
    "infantry_attack_reasserted_after_hull_exit=true"
    "passenger_ai_changes=none"
    "ownership_changes=none"
    "seating_changes=none"
    "placement_changes=none"
    "cleanup_seconds=90"
    "recurring_scheduler=disabled"
) | Set-Content -LiteralPath $manifest -Encoding UTF8

Write-Host ""
Write-Host "Turnaround-first drop alignment deployed on the tested lifecycle."
Write-Host "  Friendly truck:  drive 75s; begin return turn; pause; dismount; return"
Write-Host "  Enemy truck:     retry at 2s; drive 75s total; begin return turn; pause; dismount; return"
Write-Host "  Infantry:        attack order reasserted after the hull withdrawal command"
Write-Host "  Preserved:       seating, ownership, placement, passenger-only emit, 90s cleanup"
