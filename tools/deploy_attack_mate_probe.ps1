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

    Write-Host "OK $relative $sourceHash"
}

$gameSet = Join-Path $WorkshopRoot $files[0]
$botMain = Join-Path $WorkshopRoot $files[1]
$mate = Join-Path $WorkshopRoot $files[2]

if (-not (Select-String -Quiet -LiteralPath $gameSet -SimpleMatch "{aiTeamPlayers 1}")) {
    throw "Workshop game set does not contain the attack-mate AI slot marker"
}
if (-not (Select-String -Quiet -LiteralPath $botMain -SimpleMatch "team_a_attack_safe_route")) {
    throw "Workshop bot.main.lua does not contain the safe Team A attack route"
}
if (Select-String -Quiet -LiteralPath $botMain -SimpleMatch "identity.playerId == identity.firstPlayerId") {
    throw "Workshop bot.main.lua still contains the invalid FirstPlayerId exclusion"
}
if (-not (Select-String -Quiet -LiteralPath $mate -SimpleMatch "primary_attack_mate_candidate")) {
    throw "Workshop attacker_mate.lua does not contain the primary-candidate marker"
}

Write-Host "`nVerification markers:"
Select-String -LiteralPath $gameSet -Pattern "aiTeamPlayers 1"
Select-String -LiteralPath $botMain -Pattern "CODEX_ATTACK_MATE_ROUTER|team_a_attack_safe_route"
Select-String -LiteralPath $mate -Pattern "CODEX_ATTACK_MATE_PROBE|primary_attack_mate_candidate|attack_defenderbot_shadow"

Write-Host "`nDeployment complete. Fully restart Gates of Hell before testing."
