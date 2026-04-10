/*
 * 작성자: 윤정도
 * 생성일: 3/12/2023 7:08:06 PM
 *
 */

using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class SelectPropertyGridElement : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public SelectPropertyGridElement(MainViewModel _viewModel)
            : base(_viewModel, "프로퍼티 그리드에 오브젝트를 할당합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            if (_parameter is not SGUIElement element)
                return;

            ViewModel.View.UIElementPropertyGrid.SelectedObject = element;
        }
    }
}
