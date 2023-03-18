/*
 * 작성자: 윤정도
 * 생성일: 3/15/2023 9:31:42 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
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
using SGToolsCommon.Extension;
using SGToolsUI.Model.Main;
using Xceed.Wpf.AvalonDock.Controls;
using Xceed.Wpf.Toolkit;

namespace SGToolsUI.ModelTemplate
{
    public partial class UIEditBoxCanvasTemplate : ResourceDictionary
    {
        public UIEditBoxCanvasTemplate()
        {
        }

        private bool TryCast(object sender, out WatermarkTextBox tb, out SGUIEditBox editBox)
        {
            tb = sender as WatermarkTextBox;
            editBox = null;

            if (tb != null)
                editBox = tb.DataContext as SGUIEditBox;

            return tb != null && editBox != null;
        }


        private void TextBox_OnLoaded(object sender, RoutedEventArgs e)
        {
            if (!TryCast(sender, out WatermarkTextBox tb, out SGUIEditBox editBox))
                return;

            editBox.TextBox = tb;
        }

    }
}
