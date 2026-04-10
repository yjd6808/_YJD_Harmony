/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 6:05:12 AM
 *
 */

using System.Windows.Input;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand.Control
{
    public class CanvasZoomWheel : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public CanvasZoomWheel(MainViewModel _viewModel)
            : base(_viewModel, "UI 툴의 캔버스 크기를 휠로 조절합니다. 이에따라 윈도우 크기도 같이 조절됩니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            if (!ViewModel.KeyState.IsCtrlPressed)
                return;

            MouseWheelEventArgs wheelEventArgs = _parameter as MouseWheelEventArgs;

            if (wheelEventArgs == null)
                return;

            double canvasWidth = ViewModel.View.UIElementsControl.DesiredSize.Width;
            double canvasHeight = ViewModel.View.UIElementsControl.DesiredSize.Height;
            double deltaWidth = Constant.CanvasWheelDelta;
            double deltaHeight = Constant.CanvasWheelDelta * Constant.ResolutionRatio;
            double scaleX;
            double scaleY;

            if (wheelEventArgs.Delta < 0)
            {
                deltaWidth *= -1;
                deltaHeight *= -1;
            }

            scaleX = (canvasWidth + deltaWidth) / Constant.CanvasWidth;
            scaleY = (canvasHeight + deltaHeight) / Constant.CanvasHeight;

            if (scaleX < 0.51 || scaleY < 0.51)
                return;

            ViewModel.ZoomState.ZoomLevelX = scaleX;
            ViewModel.ZoomState.ZoomLevelY = scaleY;
        }
    }
}
