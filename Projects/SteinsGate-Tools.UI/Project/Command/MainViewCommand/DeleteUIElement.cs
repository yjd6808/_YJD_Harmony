/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 12:48:46 PM
 *
 */

using System.Linq;
using System.Threading.Tasks;
using MoreLinq;
using SGToolsUI.FileSystem;
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class DeleteUIElement : MainCommandAbstractAsync
    {
        //////////////////////////////////////////////////////////////////////////////////
        public DeleteUIElement(MainViewModel _viewModel)
            : base(_viewModel, "파라미터로 전달된 SGUIElement를 트리뷰에서 삭제합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override async Task ExecuteAsync(object? _parameter)
        {
            var rootGroup = ViewModel.RootGroup;

            if (!rootGroup.HasSelectedElement)
                return;

            var lookup = rootGroup.SelectedElements.ToLookup(element => element.IsGroup);

            var saver = new SGUISaver(ViewModel);
            await saver.BackupAsync("삭제");

            lookup[true].OrderBy(element => element.Depth).Cast<SGUIGroup>().ToList().ForEach(deletedGroupElement =>
            {
                deletedGroupElement.Selected = false;
                deletedGroupElement.Picked = false;
                deletedGroupElement.DeleteSelf();
            });

            // 이후 무지성 원소 삭제
            lookup[false].ToList().ForEach(deletedElement =>
            {
                deletedElement.Selected = false;
                deletedElement.Picked = false;
                deletedElement.DeleteSelf();
            });
        }
    }
}
