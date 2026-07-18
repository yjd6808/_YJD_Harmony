/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 8:29:00 AM
 *
 */

using System.Collections.Generic;
using System.Linq;
using MoreLinq;
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public enum SelectMode
    {
        New,        // 기존 셀력 해제 후 선택
        KeepExcept, // 기존 셀렉션 유지 및 겹치는 대상 제거
        Keep        // 기존 셀렉션 유지 및 겹치더라도 제거안함.
    }

    public class SelectUIElement : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public SelectUIElement(MainViewModel _viewModel)
            : base(_viewModel, "SGUI 엘리먼트를 선택합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            SelectMode mode = ViewModel.UIElementSelectMode;

            switch (_parameter)
            {
            case SGUIElement element:
                SelectSingleElement(element, mode);
                break;
            case IEnumerable<SGUIElement> elementList:
                SelectMultiElements(elementList, mode);
                break;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SelectSingleElement(SGUIElement _element, SelectMode _mode)
        {
            switch (_mode)
            {
                case SelectMode.New:        SelectSingleElementNew(_element);       break;
                case SelectMode.KeepExcept: SelectSingleElementKeepExcept(_element); break;
                case SelectMode.Keep:       SelectSingleElementKeep(_element);      break;
            }

            if (_element.Picked && _element.Selected)
            {
                // ViewModel.View.UIElementPropertyGrid.SelectedObject = element;
                // ViewModel.RootGroup.NotifyProperty("PickedSelectedElement");
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SelectSingleElementNew(SGUIElement _element)
        {
            ViewModel.RootGroup.DeselectAll();

            if (_element.Selected)
            {
                _element.Selected = false;
                return;
            }

            _element.Selected = true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SelectSingleElementKeepExcept(SGUIElement _element)
        {
            if (_element.Selected)
            {
                _element.Selected = false;
                return;
            }

            _element.Selected = true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SelectSingleElementKeep(SGUIElement _element)
        {
            if (_element.Selected)
                return;

            _element.Selected = true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SelectMultiElements(IEnumerable<SGUIElement> _elementList, SelectMode _mode)
        {
            switch (_mode)
            {
                case SelectMode.New:        SelectMultiElementNew(_elementList);       break;
                case SelectMode.KeepExcept: SelectMultiElementKeepExcept(_elementList); break;
                case SelectMode.Keep:       SelectMultiElementKeep(_elementList);      break;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SelectMultiElementNew(IEnumerable<SGUIElement> _elementList)
        {
            ViewModel.RootGroup.DeselectAll();
            _elementList.ForEach(newElement => newElement.Selected = true);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SelectMultiElementKeepExcept(IEnumerable<SGUIElement> _elementList)
        {
            IEnumerable<SGUIElement> alreadySelectedElements = ViewModel.RootGroup.SelectedElements.Intersect(_elementList);

            // 이미 존재하는 대상은 선택해제
            if (alreadySelectedElements.Any())
                alreadySelectedElements.ForEach(selectedElement => selectedElement.Selected = false);

            // 겹치는 대상을 제외하고 새로 선택해줌
            _elementList.Except(alreadySelectedElements).ForEach(newElement => newElement.Selected = true);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SelectMultiElementKeep(IEnumerable<SGUIElement> _elementList)
        {
            // 어차피 이미 셀릭트되었다면 내부 프로퍼티에서 바로 나오기때문에 이렇게 해도댐
            _elementList.ForEach(element =>
            {
                element.Selected = false;
                element.Selected = true;
            });
        }
    }
}
