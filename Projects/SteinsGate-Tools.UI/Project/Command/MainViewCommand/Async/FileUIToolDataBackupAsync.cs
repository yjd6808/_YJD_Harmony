/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 11:19:17 AM
 *
 */

using System;
using System.Threading.Tasks;
using SGToolsUI.FileSystem;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand.Async
{
    public class FileUIToolDataBackupAsync : MainCommandAbstractAsync
    {
        //////////////////////////////////////////////////////////////////////////////////
        public FileUIToolDataBackupAsync(MainViewModel _viewModel)
            : base(_viewModel, "UI툴데이터를 백업합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override async Task ExecuteAsync(object? _parameter)
        {
            if (_parameter == null)
                return;

            if (_parameter is not string param)
                throw new Exception("파라미터가 string 타입이 아닙니다.");

            var saver = new SGUISaver(ViewModel);
            await saver.BackupAsync(param);
        }
    }
}
