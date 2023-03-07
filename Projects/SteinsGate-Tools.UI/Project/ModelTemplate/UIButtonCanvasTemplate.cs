/*
 * 작성자: 윤정도
 * 생성일: 3/7/2023 2:55:42 PM
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
using SGToolsCommon.Extension;
using SGToolsUI.Model;

namespace SGToolsUI.ModelTemplate
{
    public partial class UIButtonCanvasTemplate : ResourceDictionary
    {
        public UIButtonCanvasTemplate()
        {
        }

        private void ButtonImage_OnMouseEnter(object sender, MouseEventArgs e)
        {
            if (!TryCast(sender, out Image img, out SGUIButton button))
                return;

        }

        private void ButtonImage_OnMouseLeave(object sender, MouseEventArgs e)
        {
        }

        private void ButtonImage_OnMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
        }

        private void ButtonImage_OnMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
        }

        private bool TryCast(object sender, out Image img, out SGUIButton button)
        {
            img = sender as Image;
            button = null;

            if (img != null)
                button = img.DataContext as SGUIButton;

            return img != null && button != null;
        }
    }
}
