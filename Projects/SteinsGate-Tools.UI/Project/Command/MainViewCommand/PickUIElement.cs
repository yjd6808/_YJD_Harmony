/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 4:00:38 AM
 *
 */

using System.Collections.Generic;
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class PickUIElement : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public PickUIElement(MainViewModel _viewModel)
            : base(_viewModel, "트리뷰에서 엘리먼트 더블클릭시 해당 엘리먼트만 선택하여 보여줍니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            SelectMode mode = ViewModel.UIElementSelectMode;

            switch (_parameter)
            {
            case SGUIElement element:
                PickSingleElement(element, mode);
                break;
            case IEnumerable<SGUIElement> elements:
                PickMultiElement(elements, mode);
                break;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void PickSingleElement(SGUIElement _element, SelectMode _mode)
        {
            switch (_mode)
            {
                case SelectMode.New: PickSingleElementNew(_element); break;
                case SelectMode.KeepExcept: PickSingleElementKeepExcept(_element); break;
                case SelectMode.Keep: PickSingleElementKeep(_element); break;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void PickSingleElementNew(SGUIElement _element)
        {
            // 픽 해제 전에는 무조건 셀렉트를 먼저해제해줘야한다. SGUIElement.Selected 주석 참고
            ViewModel.RootGroup.DeselectAll();
            ViewModel.RootGroup.Depick();
            _element.Picked = true;
            _element.Selected = true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void PickSingleElementKeepExcept(SGUIElement _element)
        {
            if (_element.Picked)
            {
                _element.Picked = false;
                return;
            }

            _element.Picked = true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void PickSingleElementKeep(SGUIElement _element)
        {
            if (_element.Picked)
                return;

            _element.Picked = true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void PickMultiElement(IEnumerable<SGUIElement> _elements, SelectMode _mode)
        {
        }
    }
}
