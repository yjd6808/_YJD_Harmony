/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 11:19:17 AM
 *
 */

using Microsoft.Win32;
using SGToolsUI.FileSystem;
using SGToolsUI.ViewModel;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using SGToolsUI.View;
using SGToolsCommon.Resource;

namespace SGToolsUI.Command.MainViewCommand.Async
{


    public class FileUIToolDataBackupAsync : MainCommandAbstractAsync
    {
        public FileUIToolDataBackupAsync(MainViewModel viewModel)
            : base(viewModel, "UI툴데이터를 백업합니다.")
        {
        }

        public override async Task ExecuteAsync(object? parameter)
        {
            if (parameter == null)
                return;

            if (parameter is not string param)
                throw new Exception("파라미터가 string 타입이 아닙니다.");

            await ViewModel.Saver.BackupAsync(param);
        }
    }
}
