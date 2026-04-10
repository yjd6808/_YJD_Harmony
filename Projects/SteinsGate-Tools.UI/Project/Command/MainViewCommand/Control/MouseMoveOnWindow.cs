/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 12:28:59 AM
 *
 */

using System.Windows;
using System.Windows.Input;
using SGToolsCommon.Extension;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand.Control
{
    public class MouseMoveOnWindow : MainCommandAbstract
    {
        public bool StopRecord { get; set; } = false;

        //////////////////////////////////////////////////////////////////////////////////
        public MouseMoveOnWindow(MainViewModel _viewModel)
            : base(_viewModel, "윈도우상에서 발생하는 마우스 움직임을 처리합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            MouseEventArgs mouseEventArgs = _parameter as MouseEventArgs;

            if (mouseEventArgs == null)
                return;

            if (StopRecord)
                return;

            ViewModel.MouseOnWindow = mouseEventArgs.GetPosition(ViewModel.View).ToVector();
            ViewModel.MouseOnMonitor = new Vector(
                ViewModel.View.Left + ViewModel.MouseOnWindow.X,
                ViewModel.View.Top + ViewModel.MouseOnWindow.Y
            );

            ViewModel.MouseOnCanvas = Mouse.GetPosition(ViewModel.View.UIElementsControl).ToVector();
        }
    }
}
