/*
 * 작성자: 윤정도
 * 생성일: 3/4/2023 10:21:24 AM
 *
 */

using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class CanvasZoomUp : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public CanvasZoomUp(MainViewModel _viewModel)
            : base(_viewModel, "축소합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            double zoomLevelX = ViewModel.ZoomState.ZoomLevelX;
            double zoomLevelY = ViewModel.ZoomState.ZoomLevelY;

            if (zoomLevelX - 0.1 < 0.53)
                return;

            ViewModel.ZoomState.ZoomLevelY = zoomLevelY - 0.1 * Constant.ResolutionRatio;
            ViewModel.ZoomState.ZoomLevelX = zoomLevelX - 0.1;
        }
    }
}
