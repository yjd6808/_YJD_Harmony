/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 6:26:46 PM
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
using SGToolsCommon.Extension;
using SGToolsUI.ViewModel;
using Path = System.IO.Path;

namespace SGToolsUI.Command.MainViewCommand
{
    public enum ToolDirectory
    {
        Exe,
        Backup,
        Export
    }

    public class OpenDirectory : MainCommandAbstract
    {
        public OpenDirectory(MainViewModel viewModel) 
            : base(viewModel, "폴더를 연다.")
        {
        }

        public override void Execute(object? parameter)
        {
            ToolDirectory toolDirectory = (ToolDirectory)Enum.Parse(typeof(ToolDirectory), (string)parameter);

            string path = string.Empty;

            switch (toolDirectory)
            {
                case ToolDirectory.Exe:
                    path = Environment.CurrentDirectory;
                    break;
                case ToolDirectory.Backup:
                    path = Path.Combine(Environment.CurrentDirectory, Constant.BackupDirectoryRoot);
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            if (path.Length == 0 || !Directory.Exists(path))
            {
                MessageBoxEx.ShowTopMost($"경로가 존재하지 않거나 경로 길이가 0입니다.\n");
                return;
            }

            ProcessEx.OpenDirectory(path);
        }
    }
}
