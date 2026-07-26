@echo off
cd /d "%~dp0"

echo Building libcocos2d [Debug/x64]...
powershell -ExecutionPolicy Bypass -File "%~dp0Build.ps1" -ProjectName libcocos2d -Configuration Debug -Platform x64
if %ERRORLEVEL% neq 0 (
    echo [FAILED] Debug/x64 build failed.
    exit /b 1
)

echo Building libcocos2d [Release/x64]...
powershell -ExecutionPolicy Bypass -File "%~dp0Build.ps1" -ProjectName libcocos2d -Configuration Release -Platform x64
if %ERRORLEVEL% neq 0 (
    echo [FAILED] Release/x64 build failed.
    exit /b 1
)

echo [DONE] libcocos2d built successfully for Debug/x64 and Release/x64.
