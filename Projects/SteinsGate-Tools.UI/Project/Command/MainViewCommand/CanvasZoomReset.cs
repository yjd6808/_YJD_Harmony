/*
 * 작성자: 윤정도
 * 생성일: 3/4/2023 10:21:24 AM
 *
 */

using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class CanvasZoomReset : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public CanvasZoomReset(MainViewModel _viewModel)
            : base(_viewModel, "줌을 가장 기본 상태로 리셋합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            ViewModel.ZoomState.ZoomLevelX = 1.0;
            ViewModel.ZoomState.ZoomLevelY = 1.0;
        }
    }
}
