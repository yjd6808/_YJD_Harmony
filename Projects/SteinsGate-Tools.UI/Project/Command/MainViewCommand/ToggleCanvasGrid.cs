/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 7:10:16 AM
 *
 */

using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class ToggleCanvasGrid : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public ToggleCanvasGrid(MainViewModel _viewModel)
            : base(_viewModel, "캔버스에 그리드를 표시합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            ViewModel.View.CanvasShapesControl.IsGridVisible = !ViewModel.View.CanvasShapesControl.IsGridVisible;
        }
    }
}
