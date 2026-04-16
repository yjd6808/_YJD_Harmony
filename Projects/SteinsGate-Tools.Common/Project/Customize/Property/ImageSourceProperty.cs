// jdyun 24/04/13(토)
using System.Windows;
using System.Windows.Media;

namespace SGToolsCommon.Customize.Property
{
    public class ImageSourceProperty
    {
        public static ImageSource GetImage(DependencyObject _obj)
        {
            return (ImageSource)_obj.GetValue(Property);
        }

        public static void SetImage(DependencyObject _obj, ImageSource _value)
        {
            _obj.SetValue(Property, _value);
        }

        public static readonly DependencyProperty Property =
            DependencyProperty.RegisterAttached("Image", typeof(ImageSource), typeof(ImageSourceProperty), new UIPropertyMetadata(null));
    }
}
