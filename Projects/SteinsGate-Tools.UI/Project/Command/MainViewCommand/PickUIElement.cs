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
using SGToolsUI.Model;
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
            SGUIGroupMaster groupMaster = ViewModel.GroupMaster;

            if (!groupMaster.HasSelectedElement)
            {
                MessageBoxEx.ShowTopMost("선택된 UI 엘리먼트가 없습니다.");
                return;
            }

            SGUIElement selectedElement = groupMaster.SelectedElement;
            selectedElement.Picked = true;
            groupMaster.PrintPick();
            ViewModel.View.ElementPropertyGrid.SelectedObject = selectedElement;
        }
    }
}
