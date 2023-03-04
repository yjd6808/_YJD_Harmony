/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 12:52:59 PM
 *
 */

using SGToolsUI.Model;
using SGToolsUI.ViewModel;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using MoreLinq;
using SGToolsCommon.Extension;

namespace SGToolsUI.Command.MainViewCommand
{
    public class AddUIElement : MainCommandAbstract
    {
        public AddUIElement(MainViewModel viewModel)
            : base(viewModel, "선택한 그룹에 UIElement를 추가합니다. " +
                              "루트에는 그룹만 추가 가능합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            SGUIGroup selectedGroup = null;//ViewModel.GroupMaster.SelectedElement as SGUIGroup;


            switch (parameter)
            {
                case SGUIElement element:
                    AddSingleElement(element, selectedGroup);
                    return;
                case IEnumerable<SGUIElement> elementList:
                    AddMultiElements(elementList, selectedGroup);
                    return;
            }
        }
        private void AddSingleElement(SGUIElement element, SGUIGroup selectedGroup)
        {
            if (selectedGroup == null && element is not SGUIGroup)
            {
                MessageBox.Show("선택한 그룹이 없어서 루트 노드에 추가할려고 했지만 추가할려는 엘리먼트가 그룹이 아닙니다.");
                return;
            }

            if (selectedGroup == null)
                selectedGroup = ViewModel.GroupMaster;

            selectedGroup.Children.Add(element);
        }
        private void AddMultiElements(IEnumerable<SGUIElement> elementList, SGUIGroup selectedGroup)
        {
            if (selectedGroup == null && elementList.Any(element => element.UIElementType != SGUIElementType.Group))
            {
                MessageBox.Show("선택한 그룹이 없어서 루트 노드에 추가할려고 했지만 추가할려는 엘리먼트중에 그룹이 아닌 원소가 있습니다.");
                return;
            }

            if (selectedGroup == null)
                selectedGroup = ViewModel.GroupMaster;


            foreach (var element in elementList)
                selectedGroup.Children.Add(element);
        }


    }
}
