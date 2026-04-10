/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 6:26:46 PM
 *
 */

using System;
using System.IO;
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
        //////////////////////////////////////////////////////////////////////////////////
        public OpenDirectory(MainViewModel _viewModel)
            : base(_viewModel, "폴더를 연다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            if (_parameter is not string param)
                throw new Exception("파라미터가 문자열 타입이 아닙니다.");

            ToolDirectory toolDirectory = (ToolDirectory)Enum.Parse(typeof(ToolDirectory), param);

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
