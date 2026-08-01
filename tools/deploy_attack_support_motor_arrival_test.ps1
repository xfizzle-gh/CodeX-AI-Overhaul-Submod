param(
    [string]$RepoRoot = "",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799",
    [ValidateSet(0, 1, 2, 3)]
    [int]$E2TestMode = 0
)

$ErrorActionPreference = "Stop"

$ScriptDirectory = Split-Path -Parent $MyInvocation.MyCommand.Path
if ([string]::IsNullOrWhiteSpace($RepoRoot)) {
    $RepoRoot = (Resolve-Path -LiteralPath (Join-Path $ScriptDirectory "..")).Path
} else {
    $RepoRoot = (Resolve-Path -LiteralPath $RepoRoot).Path
}
$WorkshopRoot = [System.IO.Path]::GetFullPath($WorkshopRoot)

$baseDeployer = Join-Path $RepoRoot "tools\deploy_attack_support_probe.ps1"
$overlay = Join-Path $RepoRoot "tools\apply_motor_arrival_release_overlay.py"

if (-not (Test-Path -LiteralPath $baseDeployer)) {
    throw "Missing base deployer: $baseDeployer"
}
if (-not (Test-Path -LiteralPath $overlay)) {
    throw "Missing motor overlay: $overlay"
}

Write-Host "Deploying the current Code:X attack-support branch to the Workshop 799 folder..."
& $baseDeployer -RepoRoot $RepoRoot -WorkshopRoot $WorkshopRoot -E2TestMode $E2TestMode
if ($LASTEXITCODE -ne 0) {
    throw "Base attack-support deployment failed with exit code $LASTEXITCODE"
}

Write-Host "Applying arrival-gated motor unload overlay..."
& python $overlay --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) {
    throw "Motor arrival-release overlay failed with exit code $LASTEXITCODE"
}

$requiredFiles = @(
    "resource\map\multi\attack_support_waves.inc",
    "resource\map\multi\defense_support_waves.inc",
    "resource\map\multi\enemy_attack_support.inc",
    "resource\map\multi\enemy_defense_support.inc"
)
$marker = "; MOTOR ARRIVAL-GATED RELEASE OVERLAY: <=150m, 60s fallback"
foreach ($relative in $requiredFiles) {
    $path = Join-Path $WorkshopRoot $relative
    if (-not (Test-Path -LiteralPath $path)) {
        throw "Deployed motor engine is missing: $path"
    }
    if (-not (Select-String -Quiet -LiteralPath $path -SimpleMatch $marker)) {
        throw "Arrival-release overlay marker is missing from: $path"
    }
}

Write-Host ""
Write-Host "Motor arrival test build is ready."
Write-Host "Workshop target: $WorkshopRoot"
Write-Host "Unload rule: within 150 m of an active flag, or after a 60-second fallback."
Write-Host "Passengers remain truly seated; empty trucks retain the existing edge withdrawal and cleanup."
