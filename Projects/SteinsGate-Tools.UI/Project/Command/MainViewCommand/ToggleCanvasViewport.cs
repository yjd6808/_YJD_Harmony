/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 7:10:06 AM
 *
 */

using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class ToggleCanvasViewport : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public ToggleCanvasViewport(MainViewModel _viewModel)
            : base(_viewModel, "게임 화면 영역을 표시합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            ViewModel.View.CanvasShapesControl.IsViewportVisible =
                !ViewModel.View.CanvasShapesControl.IsViewportVisible;
        }
    }
}
