/*
 * 작성자: 윤정도
 * 생성일: 3/14/2023 11:00:42 PM
 *
 */

using System.Windows;
using System.Windows.Controls;
using SGToolsUI.Model.Main;

namespace SGToolsUI.ModelTemplate.Main
{
    public partial class UISpriteCanvasTemplate : ResourceDictionary
    {
        //////////////////////////////////////////////////////////////////////////////////
        public UISpriteCanvasTemplate()
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool TryCast(object _sender, out Image _img, out SGUISprite _sprite)
        {
            _img = _sender as Image;
            _sprite = null;

            if (_img != null)
                _sprite = _img.DataContext as SGUISprite;

            return _img != null && _sprite != null;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SpriteImage_OnLoaded(object _sender, RoutedEventArgs _e)
        {
            if (!TryCast(_sender, out Image img, out SGUISprite sprite))
                return;
        }
    }
}
