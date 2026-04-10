/*
 * 작성자: 윤정도
 * 생성일: 3/4/2023 10:21:24 AM
 *
 */

using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class CanvasZoomDown : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public CanvasZoomDown(MainViewModel _viewModel)
            : base(_viewModel, "확대합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            double zoomLevelX = ViewModel.ZoomState.ZoomLevelX;
            double zoomLevelY = ViewModel.ZoomState.ZoomLevelY;

            if (zoomLevelX + 0.1 > 2.1)
                return;

            ViewModel.ZoomState.ZoomLevelY = zoomLevelY + 0.1 * Constant.ResolutionRatio;
            ViewModel.ZoomState.ZoomLevelX = zoomLevelX + 0.1;
        }
    }
}
