/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 1:41:46 AM
 *
 */

using System.Windows;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class OpenBackupView : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public OpenBackupView(MainViewModel _viewModel)
            : base(_viewModel, "백업 뷰를 보여주거나 숨깁니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            if (ViewModel.BackupView.IsVisible)
                ViewModel.BackupView.Visibility = Visibility.Collapsed;
            else
                ViewModel.BackupView.Show();
        }
    }
}
