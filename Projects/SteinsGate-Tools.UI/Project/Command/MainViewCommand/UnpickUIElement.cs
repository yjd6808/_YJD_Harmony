/*
 * 작성자: 윤정도
 * 생성일: 3/8/2023 3:17:03 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
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
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class UnpickUIElement : MainCommandAbstract
    {
        public UnpickUIElement(MainViewModel viewModel) 
            : base(viewModel, "픽된 엘리먼트를 해제합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            var groupMaster = ViewModel.GroupMaster;

            if (!groupMaster.HasPickedElement)
                return;


            ObservableCollection<SGUIElement> pickedElements = groupMaster.PickedElements;
            pickedElements.ToList().ForEach(element => element.Picked = false);
            Debug.Assert(pickedElements.Count == 0, "아직 픽된 엘리먼트가 남아있습니다.");
            ViewModel.View.CanvasShapesControl.AdjustAnchor();
        }
    }
}
