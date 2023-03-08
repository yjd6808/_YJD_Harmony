/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 6:29:11 AM
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
using SGToolsUI.Model;

namespace SGToolsUI.CustomStyle
{
    public partial class UIElementTreeViewStyle : ResourceDictionary
    {
        // 아이템이 추가되거나
        // 확장할때 아이템 로딩수행됨
        private void OnTreeViewItemLoaded(object sender, RoutedEventArgs e)
        {
            TreeViewItem item = sender as TreeViewItem;
            if (item == null)
                 throw new Exception("이럴 수 없어요 어떻게 트리뷰 아이템 아닐 수 있죠?");

             SGUIElement element = item.DataContext as SGUIElement;
             if (element == null)
                 throw new Exception("로드된 트리뷰 아이템에 데이터 컨텍스트가 설정되어있지 않습니다.");
             element.OnTreeViewItemLoaded(item);
        }
    }
}
