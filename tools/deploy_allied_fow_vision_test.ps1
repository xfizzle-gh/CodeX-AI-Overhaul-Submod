[CmdletBinding()]
param(
    [string]$RepoRoot = "",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799"
)

$ErrorActionPreference = "Stop"
$ExpectedBranch = "fix/allied-support-shared-fow-vision"

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

$runningGame = Get-Process -ErrorAction SilentlyContinue |
    Where-Object { $_.ProcessName -match 'call_to_arms|gates.of.hell|gatesofhell' }
if ($runningGame) {
    throw "Call to Arms is running. Close the game before applying the FoW vision test."
}

$patcher = Join-Path $RepoRoot "tools\apply_allied_fow_vision_owner.py"
$runtimeLua = Join-Path $WorkshopRoot "resource\script\multiplayer\modes\attack_support.lua"
foreach ($required in @($patcher, $runtimeLua)) {
    if (-not (Test-Path -LiteralPath $required -PathType Leaf)) {
        throw "Missing required FoW vision test file: $required"
    }
}

# This is deliberately an overlay, not a full deployment. Run the normal Allied
# Support deployer first, then this script last. It changes only the owner ID that
# attack-support waves receive. The units remain AI-controlled and non-player-owned.
& python $patcher --root $WorkshopRoot
if ($LASTEXITCODE -ne 0) {
    throw "Allied FoW owner overlay failed with exit code $LASTEXITCODE."
}
& python $patcher --root $WorkshopRoot --check
if ($LASTEXITCODE -ne 0) {
    throw "Allied FoW owner overlay did not validate."
}

$runtimeText = [System.IO.File]::ReadAllText($runtimeLua)
foreach ($marker in @(
    'local ownerId = positiveId(id.defenderBotId, id.playerId)',
    'sc:SetVar("id_attack_support", ownerId)',
    '"controller_playerId", id.playerId',
    '"defenderBotId", id.defenderBotId',
    '"team", id.team'
)) {
    if (-not $runtimeText.Contains($marker)) {
        throw "Workshop attack_support.lua is missing FoW test marker: $marker"
    }
}
foreach ($forbidden in @(
    'sc:SetVar("id_attack_support", id.playerId)',
    'sc:SetVar("id_attack_support", id.firstPlayerId)'
)) {
    if ($runtimeText.Contains($forbidden)) {
        throw "Workshop attack_support.lua still contains forbidden owner assignment: $forbidden"
    }
}

Write-Host ""
Write-Host "Allied FoW vision ownership test deployed."
Write-Host "  Changed file: resource\script\multiplayer\modes\attack_support.lua"
Write-Host "  Old owner:    phantom attack-support controller playerId"
Write-Host "  New owner:    Conquest DefenderBotId (real allied AI teammate)"
Write-Host "  Fallback:     controller playerId only if DefenderBotId is unavailable"
Write-Host "  AI control:   unchanged"
Write-Host "  Human owner:  never used"
Write-Host "  Defense path: unchanged; it already uses id_defenderbot"
Write-Host ""
Write-Host "Expected game.log line on a PLAYER-ATTACK mission:"
Write-Host "  CODEX_ATTACK_SUPPORT: identity_published id_attack_support 4 controller_playerId 1 defenderBotId 4 team a mi_waves 1"
