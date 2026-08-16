:: 작성자: 윤정도
:: 작성일: 2022/11/24



set BuildDir=ThirdParty/Build

cd ..
cd ..

rem /S Removes all directories and files in the specified directory in rem addition to the directory itself.  Used to remove a directory tree.
rem /Q Quiet mode, do not ask if ok to remove a directory tree with /S
rem https://stackoverflow.com/questions/7331056/how-to-delete-a-folder-and-all-contents-using-a-bat-file-in-windows
rmdir /S /Q "%BuildDir%"
git submodule update --init --recursive
if errorlevel 1 ( echo [FAIL] git submodule init failed & exit /b 1 )


:: 32비트 빌드
cmake -S . -B %BuildDir%/Win32 -G"Visual Studio 17 2022" -A Win32 -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF
cmake --build %BuildDir%/Win32 --config Debug
if errorlevel 1 ( echo [FAIL] Win32 Debug build failed & exit /b 1 )
cmake --build %BuildDir%/Win32 --config Release
if errorlevel 1 ( echo [FAIL] Win32 Release build failed & exit /b 1 )

:: 64비트 빌드
cmake -S . -B %BuildDir%/x64 -G"Visual Studio 17 2022" -A x64 -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF
cmake --build %BuildDir%/x64 --config Debug
if errorlevel 1 ( echo [FAIL] x64 Debug build failed & exit /b 1 )
cmake --build %BuildDir%/x64 --config Release
if errorlevel 1 ( echo [FAIL] x64 Release build failed & exit /b 1 )

REM copy benchmark libs to the location expected by jc_gbench.vcxproj (lib\{cfg}\benchmark.lib)
mkdir "%BuildDir%\x64\lib\Debug" 2>nul
mkdir "%BuildDir%\x64\lib\Release" 2>nul
copy /Y "%BuildDir%\x64\src\Debug\benchmark*.lib" "%BuildDir%\x64\lib\Debug\" >nul
copy /Y "%BuildDir%\x64\src\Release\benchmark*.lib" "%BuildDir%\x64\lib\Release\" >nul
if not exist "%BuildDir%\x64\lib\Debug\benchmark.lib" ( echo [FAIL] benchmark.lib not found after build & exit /b 1 )

echo "빌드가 모두 완료되었습니다"
timeout /t 2 >nul
