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

namespace SGToolsUI.Customize.Style
{
    public partial class SgaElementListBoxStyle : ResourceDictionary
    {
        //private void OnListBoxItemLoaded(object sender, RoutedEventArgs e)
        //{
        //    GetElementHeader(sender, out ListBoxItem item, out SgaElementHeader header);
        //    header.Item = item;
        //}

        //////////////////////////////////////////////////////////////////////////////////
        private void OnListBoxItemPreviewMouseLeftDown(object _sender, MouseButtonEventArgs _e)
        {
            GetElementHeader(_sender, out ListBoxItem? item, out SgaElementHeader? header);
            ((MainView)Application.Current.MainWindow).ViewModel.Commander.SelectSgaElement.Execute(header);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void GetElementHeader(object _sender, out ListBoxItem? _item, out SgaElementHeader? _header)
        {
            _header = null;
            _item = _sender as ListBoxItem;

            if (_item == null)
                throw new Exception("이럴 수 없어요 어떻게 리스트박스 아이템 아닐 수 있죠?");

            _header = _item.DataContext as SgaElementHeader;
            if (_header == null)
                throw new Exception("로드된 리스트박스 아이템에 데이터 컨텍스트가 설정되어있지 않습니다.");
        }
    }
}
