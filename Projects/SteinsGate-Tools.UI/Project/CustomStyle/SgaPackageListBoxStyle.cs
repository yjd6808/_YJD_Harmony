/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 6:29:11 AM
 *
 */

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using SGToolsCommon.Sga;
using SGToolsUI.View;

namespace SGToolsUI.CustomStyle
{
    public partial class SgaPackageListBoxStyle : ResourceDictionary
    {
        //private void OnListBoxItemLoaded(object sender, RoutedEventArgs e)
        //{
        //    GetPackage(sender, out ListBoxItem item, out SgaPackage package);
        //    package.Item = item;
        //}

        //////////////////////////////////////////////////////////////////////////////////
        private void OnListBoxItemPreviewMouseLeftDown(object _sender, MouseButtonEventArgs _e)
        {
            GetPackage(_sender, out ListBoxItem item, out SgaPackage package);
            ((MainView)Application.Current.MainWindow).ViewModel.Commander.SelectSgaPackage.Execute(package);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void GetPackage(object _sender, out ListBoxItem _item, out SgaPackage _package)
        {
            _package = null;
            _item = _sender as ListBoxItem;

            if (_item == null)
                throw new Exception("이럴 수 없어요 어떻게 리스트박스 아이템 아닐 수 있죠?");

            _package = _item.DataContext as SgaPackage;
            if (_package == null)
                throw new Exception("로드된 리스트박스 아이템에 데이터 컨텍스트가 설정되어있지 않습니다.");
        }
    }
}
