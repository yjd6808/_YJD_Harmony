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

namespace SGToolsCommon.Extension
{
    public static class ProcessEx
    {

        // 파일이면 열어서 선택
        // https://stackoverflow.com/questions/334630/opening-a-folder-in-explorer-and-selecting-a-file
        public static void OpenDirectory(string path)
        {
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
