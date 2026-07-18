/*
 * 작성자: 윤정도
 * 생성일: 3/8/2023 3:17:03 PM
 *
 */

using System.Diagnostics;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class UnpickUIElement : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public UnpickUIElement(MainViewModel _viewModel)
            : base(_viewModel, "픽된 엘리먼트를 해제합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            var rootGroup = ViewModel.RootGroup;

            if (!rootGroup.HasPickedElement)
                return;

            rootGroup.DeselectAll();
            rootGroup.Depick();
            Debug.Assert(rootGroup.PickedElements.Count == 0, "아직 픽된 엘리먼트가 남아있습니다.");
            ViewModel.View.CanvasShapesControl.AdjustAnchor();
            ViewModel.View.UIElementPropertyGrid.SelectedObject = null;
        }
    }
}
