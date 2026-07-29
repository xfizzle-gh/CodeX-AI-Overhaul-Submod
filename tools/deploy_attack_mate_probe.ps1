param(
    [string]$RepoRoot = "E:\Steam\steamapps\workshop\content\400750\CodeX AI Overhaul Submod",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799"
)

$ErrorActionPreference = "Stop"

$files = @(
    "resource\set\multiplayer\games\campaign_capture_the_flag.set",
    "resource\script\multiplayer\bot.main.lua",
    "resource\script\multiplayer\modes\attacker_mate.lua"
)

Write-Host "Deploying attack-mate slot proof"
Write-Host "Repository: $RepoRoot"
Write-Host "Workshop:   $WorkshopRoot"

foreach ($relative in $files) {
    $source = Join-Path $RepoRoot $relative
    $target = Join-Path $WorkshopRoot $relative

    if (-not (Test-Path -LiteralPath $source)) {
        throw "Missing source file: $source"
    }

    New-Item -ItemType Directory -Force -Path (Split-Path $target) | Out-Null
    Copy-Item -LiteralPath $source -Destination $target -Force

    $sourceHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $source).Hash
    $targetHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $target).Hash
    if ($sourceHash -ne $targetHash) {
        throw "Hash mismatch after copying: $relative"
    }

    Write-Host "OK $relative"
}

Write-Host "`nVerification markers:"
Select-String -LiteralPath `
    (Join-Path $WorkshopRoot "resource\set\multiplayer\games\campaign_capture_the_flag.set") `
    -Pattern "aiTeamPlayers 1"
Select-String -LiteralPath `
    (Join-Path $WorkshopRoot "resource\script\multiplayer\bot.main.lua") `
    -Pattern "CODEX_ATTACK_MATE_ROUTER|attacker_mate"
Select-String -LiteralPath `
    (Join-Path $WorkshopRoot "resource\script\multiplayer\modes\attacker_mate.lua") `
    -Pattern "CODEX_ATTACK_MATE_PROBE|diagnostics_only"

Write-Host "`nDeployment complete. Fully restart Gates of Hell before testing."
