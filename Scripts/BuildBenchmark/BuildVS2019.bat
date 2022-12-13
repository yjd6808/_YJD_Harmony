:: �ۼ���: ������
:: �ۼ���: 2022/11/24

set BuildDir=ThirdParty/Build

cd ..
cd ..

rem /S Removes all directories and files in the specified directory in rem addition to the directory itself.  Used to remove a directory tree.
rem /Q Quiet mode, do not ask if ok to remove a directory tree with /S
rem https://stackoverflow.com/questions/7331056/how-to-delete-a-folder-and-all-contents-using-a-bat-file-in-windows
rmdir /S /Q "%BuildDir%"
git submodule update --init --recursive


:: 32��Ʈ ����
cmake -S . -B %BuildDir%/Win32 -G"Visual Studio 16 2019" -A Win32
cmake --build %BuildDir%/Win32 --config Debug
cmake --build %BuildDir%/Win32 --config Release

:: 64��Ʈ ����
cmake -S . -B %BuildDir%/x64 -G"Visual Studio 16 2019" -A x64
cmake --build %BuildDir%/x64 --config Debug
cmake --build %BuildDir%/x64 --config Release

echo "���尡 ��� �Ϸ�Ǿ����ϴ�"
timeout /t 2
