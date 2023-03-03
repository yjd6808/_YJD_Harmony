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
        Keep        // 기존 셀렉션 유지 및 겹치는 대상 제거
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
                return;
            case IEnumerable<SGUIElement> elementList:
                SelectMultiElements(elementList, mode);
                return;
            }
        }

        private void SelectSingleElement(SGUIElement element, SelectMode mode)
        {
            if (mode == SelectMode.New)
            {
                ViewModel.View.CanvasShapesControl.ReleaseAllSelection();
                ViewModel.GroupMaster.DeselectAll();
            }

            if (element.Selected)
            {
                element.Selected = false;

                if (ViewModel.GroupMaster.SelectedElement == element)
                    ViewModel.GroupMaster.SelectedElement = null;

                ViewModel.View.CanvasShapesControl.ReleaseSelection(element);
                return;
            }

            element.Selected = true;
            ViewModel.GroupMaster.SelectedElement = element;
            ViewModel.View.CanvasShapesControl.ArrangeSelection(element);
        }

        private void SelectMultiElements(IEnumerable<SGUIElement> elementList, SelectMode mode)
        {
            ViewModel.GroupMaster.SelectedElement = null;
            elementList.ForEach(element =>
            {
                element.Selected = true;
                ViewModel.View.CanvasShapesControl.ArrangeSelection(element);
            });
        }
    }
}
