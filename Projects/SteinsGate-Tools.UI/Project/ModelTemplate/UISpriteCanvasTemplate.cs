/*
 * 작성자: 윤정도
 * 생성일: 3/14/2023 11:00:42 PM
 *
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
using SGToolsUI.Model;

namespace SGToolsUI.ModelTemplate
{
    public partial class UISpriteCanvasTemplate : ResourceDictionary
    {
        public UISpriteCanvasTemplate()
        {
        }

        private bool TryCast(object sender, out Image img, out SGUISprite sprite)
        {
            img = sender as Image;
            sprite = null;

            if (img != null)
                sprite = img.DataContext as SGUISprite;

            return img != null && sprite != null;
        }


        private void SpriteImage_OnLoaded(object sender, RoutedEventArgs e)
        {
            if (!TryCast(sender, out Image img, out SGUISprite sprote))
                return;

        }
    }
}
