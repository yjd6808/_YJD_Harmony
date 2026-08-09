@echo off
setlocal EnableDelayedExpansion
chcp 65001 >nul
set "BAT_DIR=%~dp0"
cd /d "%BAT_DIR%..\.."

set "CONFIG=Debug"
set "PLATFORM=x64"

:parse
if "%~1"=="" goto :parsed
if /i "%~1"=="-C" ( set "CONFIG=%~2" & shift & shift & goto :parse )
if /i "%~1"=="-P" ( set "PLATFORM=%~2" & shift & shift & goto :parse )
echo [WARN] Unknown option: %~1
shift
goto :parse
:parsed

set "BUILD_PS1=%BAT_DIR%..\..\Scripts\Build.ps1"
call :build jc
if errorlevel 1 exit /b 1

for %%f in ("%BAT_DIR%sgs-*.log") do set "LASTLOG=%%f"
echo [OK] jc build completed. [%CONFIG%/%PLATFORM%]
if defined LASTLOG echo [OK] Log file: !LASTLOG!
exit /b 0

:build
echo.
echo ================================================================
echo  [%CONFIG%/%PLATFORM%] Building %~1 ...
echo ================================================================
powershell -ExecutionPolicy Bypass -File "%BUILD_PS1%" -ProjectName %~1 -Configuration %CONFIG% -Platform %PLATFORM% -FileLog -NoConsoleLogging
if errorlevel 1 (
    set "EC=!ERRORLEVEL!"
    echo.
    echo [FAILED] %~1 build failed ^(ExitCode=!EC!^)
    for %%f in ("%BAT_DIR%sgs-*.log") do set "LASTLOG=%%f"
    if defined LASTLOG echo          Log file: !LASTLOG!
    exit /b 1
)
exit /b 0
