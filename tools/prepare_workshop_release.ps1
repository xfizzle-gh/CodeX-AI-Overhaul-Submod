[CmdletBinding()]
param(
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799",
    [string]$BackupRoot = "E:\Steam\steamapps\common\Allied-Support-Backups",
    [switch]$SkipBackup
)

$ErrorActionPreference = "Stop"
$WorkshopRoot = [System.IO.Path]::GetFullPath($WorkshopRoot)
$BackupRoot = [System.IO.Path]::GetFullPath($BackupRoot)

if (-not (Test-Path -LiteralPath $WorkshopRoot -PathType Container)) {
    throw "Workshop folder does not exist: $WorkshopRoot"
}
if ((Split-Path -Leaf $WorkshopRoot) -ne "3636883799") {
    throw "Refusing to clean an unexpected Workshop item: $WorkshopRoot"
}
if (-not (Test-Path -LiteralPath (Join-Path $WorkshopRoot "resource") -PathType Container)) {
    throw "Workshop folder is missing its resource directory: $WorkshopRoot"
}

if (-not $SkipBackup) {
    New-Item -ItemType Directory -Force -Path $BackupRoot | Out-Null
    $stamp = Get-Date -Format "yyyyMMdd_HHmmss"
    $backup = Join-Path $BackupRoot "workshop_3636883799_pre_release_$stamp"
    Copy-Item -LiteralPath $WorkshopRoot -Destination $backup -Recurse -Force
    Write-Host "External pre-release backup created:"
    Write-Host "  $backup"
}

# These are deployment diagnostics and pristine-map backups used only by the
# development tooling. None are read by the game and none belong in the Workshop
# upload. Keep this list explicit so release cleanup cannot touch runtime content.
$knownArtifacts = @(
    "_attack_support_probe_backups",
    "known_good_motor_30s_test.txt",
    "runtime_proven_motor_60s.txt",
    "friendly_defender_motor_one_shot.txt",
    "motor_drive_origin_exit.txt",
    "transport_control_comparison.txt",
    "four_quadrant_normal_transport_patrol.txt",
    "four_quadrant_transport_dropoff.txt"
)

$removed = @()
foreach ($relative in $knownArtifacts) {
    $path = Join-Path $WorkshopRoot $relative
    if (-not (Test-Path -LiteralPath $path)) { continue }
    Remove-Item -LiteralPath $path -Recurse -Force
    $removed += $relative
}

# Generated Python/PowerShell cache material is never runtime content.
$cacheDirs = Get-ChildItem -LiteralPath $WorkshopRoot -Directory -Recurse -Force -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -eq "__pycache__" }
foreach ($dir in $cacheDirs) {
    Remove-Item -LiteralPath $dir.FullName -Recurse -Force
    $removed += $dir.FullName.Substring($WorkshopRoot.Length).TrimStart('\')
}

$generatedFiles = Get-ChildItem -LiteralPath $WorkshopRoot -File -Recurse -Force -ErrorAction SilentlyContinue |
    Where-Object {
        $_.Extension -eq ".pyc" -or
        $_.Name -like "*.generated.ps1" -or
        $_.Name -like "*.generated.py"
    }
foreach ($file in $generatedFiles) {
    Remove-Item -LiteralPath $file.FullName -Force
    $removed += $file.FullName.Substring($WorkshopRoot.Length).TrimStart('\')
}

# Do not silently delete unknown material. Report and fail so it can be reviewed.
$suspicious = Get-ChildItem -LiteralPath $WorkshopRoot -Recurse -Force -ErrorAction SilentlyContinue |
    Where-Object {
        $_.Name -match '(?i)(\.bak$|\.backup$|\.old$|\.orig$|\.tmp$|\.temp$|\.log$|~$)' -or
        $_.Name -match '(?i)(^backup$|^backups$|^_backup|^_temp|^temp$)'
    }

Write-Host ""
Write-Host "Release cleanup complete."
Write-Host "  Workshop: $WorkshopRoot"
Write-Host "  Removed:  $($removed.Count) known non-runtime artifact(s)"
foreach ($item in $removed) {
    Write-Host "    - $item"
}

if ($suspicious.Count -gt 0) {
    Write-Host ""
    Write-Host "Unknown suspicious files remain:" -ForegroundColor Yellow
    foreach ($item in $suspicious) {
        Write-Host "  - $($item.FullName)"
    }
    throw "Workshop release audit failed: review the suspicious files listed above."
}

Write-Host "  Audit:    no backup, temporary, generated, or log files remain"
Write-Host "  Runtime:  resource/localization content was not altered"
