/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 7:10:06 AM
 *
 */

using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class ToggleCanvasAnchor : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public ToggleCanvasAnchor(MainViewModel _viewModel)
            : base(_viewModel, "그룹의 앵커 위치를 표시합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            ViewModel.View.CanvasShapesControl.IsAnchorVisible =
                !ViewModel.View.CanvasShapesControl.IsAnchorVisible;
        }
    }
}
