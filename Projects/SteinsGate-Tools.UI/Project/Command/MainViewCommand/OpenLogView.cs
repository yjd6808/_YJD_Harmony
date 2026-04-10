/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 1:41:46 AM
 *
 */

using System.Windows;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class OpenLogView : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public OpenLogView(MainViewModel _viewModel)
            : base(_viewModel, "로그 뷰를 보여주거나 숨깁니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            if (ViewModel.LogView.IsVisible)
                ViewModel.LogView.Visibility = Visibility.Collapsed;
            else
                ViewModel.LogView.Show();
        }
    }
}
