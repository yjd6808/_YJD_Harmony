/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 12:48:46 PM
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
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class DeleteUIElement : MainCommandAbstract
    {
        public DeleteUIElement(MainViewModel viewModel)
            : base(viewModel, "파라미터로 전달된 SGUIElement를 트리뷰에서 삭제합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            // 그룹인 요소와 그룹이 아닌 요소로 분리
            var masterGroup = ViewModel.GroupMaster;
            var lookup = masterGroup.SelectedElements.ToLookup(element => element.IsGroup);

            /*선택된 요소중 그룹만 먼저 다 가져온후 그룹을 제일먼저 제거해준 후 원소 제거를 수행한다.
              [이유]
              그룹을 삭제하면서 그룹내 하위 모든 자식들을 "Deleted"로 마크해놓는다. 그리고 그룹(A)은 부모로부터 삭제시킨다.
              이렇게 해주면 이후 그룹내 하위 그룹(B)이나 자식(C)을 삭제하려고 시도할 때 Delted 체크를 확인만하면 굳이 부모에게서 삭제시킬 필요가 없다.
              이미 최상위 부모(A)가 삭제되었기 때문이다. (삭제 여부를 표시할 SGUIElement의 멤버로 Deleted를 추가했다.)

             그리고 SelectedElements를 LookUp 했기 때문에 Selected = false 수행시 "순회중 삭제" 오류가 발생하지 않도록 하기위해 신규리스트로 만듬
             [순회중 삭제란?]
             for (int i = 0; i < 100; ++i)
                   list.Remove(i)와 같은 작업 수행시 연산이 제대로 수행안될 수 있기 때문

             최상위 그룹을 먼저 삭제 우선토록한다. 따라서 깊이가 얕은 순서대로 오름차순 정렬을 해준다.*/
            lookup[true].OrderBy(element => element.Depth).Cast<SGUIGroup>().ToList().ForEach(deletedGroupElement =>
            {
                masterGroup.RemoveGroup(deletedGroupElement);
                deletedGroupElement.Selected = false;
                deletedGroupElement.DeleteSelf();
            });

            // 이후 무지성 원소 삭제
            lookup[false].ToList().ForEach(deletedElement =>
            {
                deletedElement.Selected = false;
                deletedElement.DeleteSelf();
            });
        }
    }
}
