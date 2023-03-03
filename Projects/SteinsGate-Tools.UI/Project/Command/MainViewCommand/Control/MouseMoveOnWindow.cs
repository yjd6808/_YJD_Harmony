/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 12:28:59 AM
 *
 */

using SGToolsUI.Model;
using SGToolsUI.View;
using SGToolsUI.ViewModel;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net;
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

namespace SGToolsUI.Command.MainViewCommand.Control
{
    public class MouseMoveOnWindow : MainCommandAbstract
    {
        public bool StopRecord { get; set; } = false;

        public MouseMoveOnWindow(MainViewModel viewModel)
            : base(viewModel, "윈도우상에서 발생하는 마우스 움직임을 처리합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            MouseEventArgs mouseEventArgs = parameter as MouseEventArgs;

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
