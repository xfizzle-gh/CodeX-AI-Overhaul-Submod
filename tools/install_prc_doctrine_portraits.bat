@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0install_prc_doctrine_portraits.ps1" %*
if errorlevel 1 (
    echo.
    echo PRC doctrine portrait installation failed.
    exit /b 1
)
echo.
echo PRC doctrine portraits installed.
endlocal
