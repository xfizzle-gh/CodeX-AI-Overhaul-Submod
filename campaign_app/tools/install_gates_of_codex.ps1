param(
    [string]$InstallRoot = "$env:LOCALAPPDATA\GatesOfCodeX"
)

$ErrorActionPreference = "Stop"
$Python = Get-Command py -ErrorAction SilentlyContinue
if (-not $Python) {
    throw "Python launcher 'py' was not found. Install Python 3.11 or newer first."
}

New-Item -ItemType Directory -Force -Path $InstallRoot | Out-Null
$Venv = Join-Path $InstallRoot ".venv"
& py -3.11 -m venv $Venv
$Pip = Join-Path $Venv "Scripts\pip.exe"
$PythonExe = Join-Path $Venv "Scripts\python.exe"
& $Pip install --upgrade pip
& $Pip install (Resolve-Path (Join-Path $PSScriptRoot ".."))

$Launcher = Join-Path $InstallRoot "Gates-of-CodeX.cmd"
@"
@echo off
"$PythonExe" -m gates_of_codex.cli ui %*
"@ | Set-Content -Encoding ASCII $Launcher

Write-Host "Installed Gates of CodeX to $InstallRoot"
Write-Host "Launch with: $Launcher"
