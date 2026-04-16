/*
 * 작성자: 윤정도
 * 생성일: 3/15/2023 9:31:42 PM
 *
 */

using System.Windows;
using System.Windows.Controls;
using SGToolsUI.Model.Main;
using Xceed.Wpf.Toolkit;

namespace SGToolsUI.ModelTemplate.Main
{
    public partial class UIEditBoxCanvasTemplate : ResourceDictionary
    {
        //////////////////////////////////////////////////////////////////////////////////
        public UIEditBoxCanvasTemplate()
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool TryCast(object _sender, out WatermarkTextBox? _tb, out SGUIEditBox? _editBox)
        {
            _tb = _sender as WatermarkTextBox;
            _editBox = null;

            if (_tb != null)
                _editBox = _tb.DataContext as SGUIEditBox;

            return _tb != null && _editBox != null;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void TextBox_OnLoaded(object _sender, RoutedEventArgs _e)
        {
            if (!TryCast(_sender, out WatermarkTextBox? tb, out SGUIEditBox? editBox))
                return;

            editBox!.TextBox = tb;
        }
    }
}
