@echo off
set "OUTPUT_DIR=%~dp0..\Output\x64\Debug"
set "LOG_FILE=%~dp0..\Output\x64\Debug\jc_gtest_result.log"

cd /d "%OUTPUT_DIR%"

> "%LOG_FILE%" (
    for /f "delims=" %%a in ('jc_gtest.exe 2^>^&1') do (
        echo %%a
    )
)

echo.
echo Result saved to: %LOG_FILE%
pause