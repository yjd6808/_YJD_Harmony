/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 8:07:02 PM
 *
 */

using System;
using System.Diagnostics;

using static Vanara.PInvoke.User32;

using Path = System.IO.Path;
using Process = System.Diagnostics.Process;

namespace SGToolsCommon.Extension
{
    public static class ProcessEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        // 파일이면 열어서 선택
        // https://stackoverflow.com/questions/334630/opening-a-folder-in-explorer-and-selecting-a-file
        public static void OpenDirectory(string _path)
        {
            string directoryName = Path.GetFileName(_path);
            if (FileEx.IsFile(_path))
                throw new Exception($"{_path}는 폴더가 아닙니다.");

            if (directoryName == string.Empty)
                throw new Exception($"{_path}에서 디렉토리 명을 얻는데 실패했습니다.");

            foreach (Process process in Process.GetProcesses())
            {
                if (process.MainWindowTitle.Contains(directoryName))
                {
                    // 열려있는 폴더가 있다면 해당 프로세스를 활성화
                    IntPtr hWnd = process.MainWindowHandle;
                    SetForegroundWindow(hWnd);
                    return;
                }
            }

            string explorerDir = Environment.GetEnvironmentVariable("WINDIR") + @"\explorer.exe";

            if (FileEx.IsFile(_path))
            {
                string argument = "/select, \"" + _path + "\"";
                Process.Start(explorerDir, argument);
            }
            else
            {
                Process.Start(explorerDir, _path);
            }
        }
    }
}
