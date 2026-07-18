/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 12:52:59 PM
 *
 */

using System;
using System.Threading.Tasks;
using System.Windows;
using SGToolsUI.FileSystem;
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class CreateUIElement : MainCommandAbstractAsync
    {
        //////////////////////////////////////////////////////////////////////////////////
        public CreateUIElement(MainViewModel _viewModel)
            : base(_viewModel, "선택한 그룹에 UIElement를 추가합니다. " +
                              "선택한 그룹이 없을 경우 루트에 추가합니다. " +
                              "루트에는 그룹 엘리먼트만 추가가능합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override async Task ExecuteAsync(object? _parameter)
        {
            SGUIElementType createElementType = (SGUIElementType)Enum.Parse(typeof(SGUIElementType), _parameter!.ToString()!);
            SGUIGroup group = ViewModel.RootGroup.SelectedGroup;

            if (group == null)
            {
                MessageBox.Show($"{createElementType}타입의 엘리먼트를 추가하는데 실패했습니다.\n그룹을 선택해주세요.");
                return;
            }

            // 마스터 그룹엔 그룹만 추가가능
            bool isRootGroup = group == ViewModel.RootGroup;
            if (isRootGroup && createElementType != SGUIElementType.Group)
            {
                MessageBox.Show($"루트 그룹에는 그룹만 추가 가능합니다.");
                return;
            }

            SGUIElement newElement = SGUIElement.Create(createElementType);
            if (newElement.IsGroup)
                newElement.VisualSize = new Size(Constant.ResolutionWidth, Constant.ResolutionHeight);

            newElement.CreateInit();
            newElement.ViewModel = ViewModel;
            group.AddChild(newElement);
            var saver = new SGUISaver(ViewModel);
            await saver.BackupAsync($"{createElementType} 생성");
            // 루트그룹은 트리뷰에서 관리를 안하므로..
            if (!isRootGroup)
                group.Item.IsExpanded = true;
        }
    }
}
