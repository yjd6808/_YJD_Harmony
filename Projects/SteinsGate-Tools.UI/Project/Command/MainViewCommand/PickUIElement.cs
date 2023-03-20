/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 4:00:38 AM
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
using SGToolsCommon.Extension;
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class PickUIElement : MainCommandAbstract
    {
        public PickUIElement(MainViewModel viewModel) 
            : base(viewModel, "트리뷰에서 엘리먼트 더블클릭시 해당 엘리먼트만 선택하여 보여줍니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            SelectMode mode = ViewModel.UIElementSelectMode;

            switch (parameter)
            {
            case SGUIElement element: 
                PickSingleElement(element, mode);
                break;
            case IEnumerable<SGUIElement> elements: 
                PickMultiElement(elements, mode);
                break;
            }

            //ViewModel.View.UIElementPropertyGrid.SelectedObject = selectedElement;
        }

        private void PickSingleElement(SGUIElement element, SelectMode mode)
        {
            switch (mode)
            {
                case SelectMode.New: PickSingleElementNew(element); break;
                case SelectMode.KeepExcept: PickSingleElementKeepExcept(element); break;
                case SelectMode.Keep: PickSingleElementKeep(element); break;
            }
        }

      
        private void PickSingleElementNew(SGUIElement element)
        {
            // 픽 해제 전에는 무조건 셀렉트를 먼저해제해줘야한다. SGUIElement.Selected 주석 참고
            ViewModel.GroupMaster.DeselectAll();
            ViewModel.GroupMaster.Depick();
            element.Picked = true;
            element.Selected = true;
        }

        private void PickSingleElementKeepExcept(SGUIElement element)
        {
            if (element.Picked)
            {
                element.Picked = false;
                return;
            }

            element.Picked = true;
        }


        private void PickSingleElementKeep(SGUIElement element)
        {
            if (element.Picked)
                return;

            element.Picked = true;
        }



        private void PickMultiElement(IEnumerable<SGUIElement> elements, SelectMode mode)
        {
        }
    }
}
