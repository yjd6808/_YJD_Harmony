/*
 * 복붙
 * 생성일: 2/27/2023 10:52:42 AM
 *
 */

using System.Windows;
using System.Windows.Media;

namespace SGToolsCommon.CustomStyle
{
    public class ImageButtonProperty
    {
        public static readonly DependencyProperty ImageProperty =
            DependencyProperty.RegisterAttached("Image", typeof(ImageSource), typeof(ImageButtonProperty), new UIPropertyMetadata((ImageSource)null));

        //////////////////////////////////////////////////////////////////////////////////
        public static ImageSource GetImage(DependencyObject _obj)
        {
            return (ImageSource)_obj.GetValue(ImageProperty);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void SetImage(DependencyObject _obj, ImageSource _value)
        {
            _obj.SetValue(ImageProperty, _value);
        }
    }
}
