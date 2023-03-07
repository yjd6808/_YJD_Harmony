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
using SGToolsCommon.Sga;
using SGToolsUI.Model;
using SGToolsUI.View;

namespace SGToolsUI.CustomStyle
{
    public partial class SgaPackageListBoxStyle : ResourceDictionary
    {
        private void OnListBoxItemLoaded(object sender, RoutedEventArgs e)
        {
            GetPackage(sender, out ListBoxItem item, out SgaPackage package);
            package.Item = item;
        }

        private void OnListBoxItemPreviewMouseLeftDown(object sender, MouseButtonEventArgs e)
        {
            GetPackage(sender, out ListBoxItem item, out SgaPackage package);
            ((MainView)Application.Current.MainWindow).ViewModel.Commander.SelectSgaPackage.Execute(package);
        }

        private void GetPackage(object sender, out ListBoxItem item, out SgaPackage package)
        {
            package = null;
            item = sender as ListBoxItem;

            if (item == null)
                throw new Exception("이럴 수 없어요 어떻게 리스트박스 아이템 아닐 수 있죠?");

            package = item.DataContext as SgaPackage;
            if (package == null)
                throw new Exception("로드된 리스트박스 아이템에 데이터 컨텍스트가 설정되어있지 않습니다.");
        }

    }
}
