/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 8:07:02 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using static Vanara.PInvoke.Kernel32.PSS_HANDLE_ENTRY;
using Path = System.IO.Path;
using Process = System.Diagnostics.Process;

using static Vanara.PInvoke.User32;

namespace SGToolsCommon.Extension
{
    public static class ProcessEx
    {

        // 파일이면 열어서 선택
        // https://stackoverflow.com/questions/334630/opening-a-folder-in-explorer-and-selecting-a-file
        public static void OpenDirectory(string path)
        {
            string directoryName = Path.GetFileName(path);
            if (FileEx.IsFile(path))
                throw new Exception($"{path}는 폴더가 아닙니다.");

            if (directoryName == string.Empty)
                throw new Exception($"{path}에서 디렉토리 명을 얻는데 실패했습니다.");

            foreach (Process process in Process.GetProcesses())
            {
                if (process.MainWindowTitle.Contains(directoryName))
                {
                    // 열려있는 폴더가 있다면 해당 프로세스를 활성화
                    IntPtr hWnd = process.MainWindowHandle;
                    SetForegroundWindow(hWnd);
                    // WindowEx.Flash(hWnd);
                    return;
                }
            }

            string explorerDir = Environment.GetEnvironmentVariable("WINDIR") + @"\explorer.exe";

            if (FileEx.IsFile(path))
            {
                string argument = "/select, \"" + path + "\"";
                Process.Start(explorerDir, argument);
            }
            else
            {
                Process.Start(explorerDir, path);
            }
        }
    }
}
