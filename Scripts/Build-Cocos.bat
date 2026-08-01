@echo off
setlocal
cd /d "%~dp0"

set "VCXPROJ=%~dp0..\ThirdParty\Cocos2d-x\cocos\2d\libcocos2d.vcxproj"
for %%i in ("%~dp0..") do set "SLNDIR=%%~fi\"

set "MSBUILD="
for /f "delims=" %%i in ('where.exe msbuild 2^>nul') do if not defined MSBUILD set "MSBUILD=%%i"
if not defined MSBUILD (
    for /f "delims=" %%i in ('"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe" 2^>nul') do if not defined MSBUILD set "MSBUILD=%%i"
)
if not defined MSBUILD (
    echo [FAIL] msbuild not found.
    exit /b 1
)

echo Building libcocos2d [Debug/x64]...
"%MSBUILD%" "%VCXPROJ%" /t:Build /p:Configuration=Debug /p:Platform=x64 /p:SolutionDir=%SLNDIR% /m /nologo /v:m
if %ERRORLEVEL% neq 0 (
    echo [FAILED] Debug/x64 build failed.
    exit /b 1
)

echo Building libcocos2d [Release/x64]...
"%MSBUILD%" "%VCXPROJ%" /t:Build /p:Configuration=Release /p:Platform=x64 /p:SolutionDir=%SLNDIR% /m /nologo /v:m
if %ERRORLEVEL% neq 0 (
    echo [FAILED] Release/x64 build failed.
    exit /b 1
)

echo [DONE] libcocos2d built successfully for Debug/x64 and Release/x64.
