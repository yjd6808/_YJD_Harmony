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

namespace SGToolsUI.Command.MainViewCommand
{
    public class CreateUIElement : MainCommandAbstract
    {
        public CreateUIElement(MainViewModel viewModel)
            : base(viewModel, "선택한 그룹에 UIElement를 추가합니다. " +
                              "선택한 그룹이 없을 경우 루트에 추가합니다. " +
                              "루트에는 그룹 엘리먼트만 추가가능합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            SGUIElementType createElementType = (SGUIElementType)Enum.Parse(typeof(SGUIElementType), parameter.ToString());
            SGUIGroup group = ViewModel.GroupMaster.SelectedGroup;

            if (group == null)
            {
                MessageBox.Show($"{createElementType}타입의 엘리먼트를 추가하는데 실패했습니다.\n그룹을 선택해주세요.");
                return;
            }

            // 마스터 그룹엔 그룹만 추가가능
            bool isGroupMaster = group == ViewModel.GroupMaster;
            if (isGroupMaster && createElementType != SGUIElementType.Group)
            {
                MessageBox.Show($"마스터 그룹에는 그룹만 추가 가능합니다.");
                return;
            }

            SGUIElement newElement = SGUIElement.Create(createElementType, group);
            newElement.ViewModel = ViewModel;
            group.AddChild(newElement);

            if (createElementType == SGUIElementType.Group)
                ViewModel.GroupMaster.Groups.Add(newElement.Cast<SGUIGroup>());

            // 그룹마스터는 트리뷰에서 관리를 안하므로..
            if (!isGroupMaster)
                group.Item.IsExpanded = true;
        }
    }
}
