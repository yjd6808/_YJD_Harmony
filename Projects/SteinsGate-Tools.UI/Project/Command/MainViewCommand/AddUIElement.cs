/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 12:52:59 PM
 *
 */

using System.Collections.Generic;
using System.Linq;
using System.Windows;
using MoreLinq;
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class AddUIElement : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public AddUIElement(MainViewModel _viewModel)
            : base(_viewModel, "선택한 그룹에 UIElement를 추가합니다. " +
                              "루트에는 그룹만 추가 가능합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            SGUIGroup selectedGroup = null;

            switch (_parameter)
            {
                case SGUIElement element:
                    AddSingleElement(element, selectedGroup);
                    return;
                case IEnumerable<SGUIElement> elementList:
                    AddMultiElements(elementList, selectedGroup);
                    return;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void AddSingleElement(SGUIElement _element, SGUIGroup _selectedGroup)
        {
            if (_selectedGroup == null && _element is not SGUIGroup)
            {
                MessageBox.Show("선택한 그룹이 없어서 루트 노드에 추가할려고 했지만 추가할려는 엘리먼트가 그룹이 아닙니다.");
                return;
            }

            if (_selectedGroup == null)
                _selectedGroup = ViewModel.GroupMaster;

            _selectedGroup.Children.Add(_element);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void AddMultiElements(IEnumerable<SGUIElement> _elementList, SGUIGroup _selectedGroup)
        {
            if (_selectedGroup == null && _elementList.Any(element => element.UIElementType != SGUIElementType.Group))
            {
                MessageBox.Show("선택한 그룹이 없어서 루트 노드에 추가할려고 했지만 추가할려는 엘리먼트중에 그룹이 아닌 원소가 있습니다.");
                return;
            }

            if (_selectedGroup == null)
                _selectedGroup = ViewModel.GroupMaster;

            foreach (var element in _elementList)
                _selectedGroup.Children.Add(element);
        }
    }
}
