/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 8:29:00 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using MoreLinq;
using SGToolsUI.Model;
using SGToolsUI.CustomControl;
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
        public SelectUIElement(MainViewModel viewModel) 
            : base(viewModel, "SGUI 엘리먼트를 선택합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            SelectMode mode = ViewModel.UIElementSelectMode;

            switch (parameter)
            {
            case SGUIElement element:
                SelectSingleElement(element, mode);
                break;
            case IEnumerable<SGUIElement> elementList:
                SelectMultiElements(elementList, mode);
                break;
            }

        }

        

        private void SelectSingleElement(SGUIElement element, SelectMode mode)
        {
            switch (mode)
            {
                case SelectMode.New:  SelectSingleElementNew(element);   break;
                case SelectMode.KeepExcept: SelectSingleElementKeepExcept(element);  break;
                case SelectMode.Keep: SelectSingleElementKeep(element);  break;
            }

            // 음.. 프로퍼티 그리드 성능이 엄청 안좋네
            if (element.Picked && element.Selected)
            {
                // ViewModel.View.UIElementPropertyGrid.SelectedObject = element; 
                // ViewModel.GroupMaster.NotifyProperty("PickedSelectedElement");
            }

        }

        

        private void SelectSingleElementNew(SGUIElement element)
        {
            ViewModel.GroupMaster.DeselectAll();

            if (element.Selected)
            {
                element.Selected = false;
                return;
            }


            element.Selected = true;
        }

        private void SelectSingleElementKeepExcept(SGUIElement element)
        {
            if (element.Selected)
            {
                element.Selected = false;
                return;
            }

            element.Selected = true;
        }

        private void SelectSingleElementKeep(SGUIElement element)
        {
            if (element.Selected)
                return;

            element.Selected = true;
        }

        private void SelectMultiElements(IEnumerable<SGUIElement> elementList, SelectMode mode)
        {
            switch (mode)
            {
                case SelectMode.New:  SelectMultiElementNew(elementList);  break;
                case SelectMode.KeepExcept: SelectMultiElementKeepExcept(elementList); break;
                case SelectMode.Keep: SelectMultiElementKeep(elementList); break;
            }

            
        }


        private void SelectMultiElementNew(IEnumerable<SGUIElement> elementList)
        {
            ViewModel.GroupMaster.DeselectAll();
            elementList.ForEach(newElement => newElement.Selected = true);
        }

        private void SelectMultiElementKeepExcept(IEnumerable<SGUIElement> elementList)
        {
            
            IEnumerable<SGUIElement> alreadySelectedElements = ViewModel.GroupMaster.SelectedElements.Intersect(elementList);

            // 이미 존재하는 대상은 선택해제
            if (alreadySelectedElements.Any())
                alreadySelectedElements.ForEach(selectedElement => selectedElement.Selected = false);

            // 겹치는 대상을 제외하고 새로 선택해줌
            elementList.Except(alreadySelectedElements).ForEach(newElement => newElement.Selected = true);
        }

        private void SelectMultiElementKeep(IEnumerable<SGUIElement> elementList)
        {
            // 어차피 이미 셀릭트되었다면 내부 프로퍼티에서 바로 나오기때문에 이렇게 해도댐
            elementList.ForEach(element => element.Selected = true);
        }
    }
}
