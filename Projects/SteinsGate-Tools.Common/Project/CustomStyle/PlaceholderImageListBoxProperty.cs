/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 10:44:47 AM
 *
 */

using System.Windows;
using System.Windows.Media;

namespace SGToolsCommon.CustomStyle
{
    public class PlaceholderImageListBoxProperty
    {
        public static readonly DependencyProperty SourceProperty = DependencyProperty.RegisterAttached(
            "Source",
            typeof(ImageSource),
            typeof(PlaceholderImageListBoxProperty),
            new PropertyMetadata(null));

        public static readonly DependencyProperty WidthProperty = DependencyProperty.RegisterAttached(
            "Width",
            typeof(double),
            typeof(PlaceholderImageListBoxProperty),
            new PropertyMetadata(null) { DefaultValue = 30.0 });

        public static readonly DependencyProperty HeightProperty = DependencyProperty.RegisterAttached(
            "Height",
            typeof(double),
            typeof(PlaceholderImageListBoxProperty),
            new PropertyMetadata(null) { DefaultValue = 30.0 });

        public static readonly DependencyProperty OpacityProperty = DependencyProperty.RegisterAttached(
            "Opacity",
            typeof(double),
            typeof(PlaceholderImageListBoxProperty),
            new PropertyMetadata(null) { DefaultValue = 0.5 });

        //////////////////////////////////////////////////////////////////////////////////
        public static ImageSource GetSource(DependencyObject _obj)
            => (ImageSource)_obj.GetValue(SourceProperty);

        //////////////////////////////////////////////////////////////////////////////////
        public static void SetSource(DependencyObject _obj, ImageSource _value)
            => _obj.SetValue(SourceProperty, _value);

        //////////////////////////////////////////////////////////////////////////////////
        public static double GetWidth(DependencyObject _obj)
            => (double)_obj.GetValue(WidthProperty);

        //////////////////////////////////////////////////////////////////////////////////
        public static void SetWidth(DependencyObject _obj, double _value)
            => _obj.SetValue(WidthProperty, _value);

        //////////////////////////////////////////////////////////////////////////////////
        public static double GetHeight(DependencyObject _obj)
            => (double)_obj.GetValue(HeightProperty);

        //////////////////////////////////////////////////////////////////////////////////
        public static void SetHeight(DependencyObject _obj, double _value)
            => _obj.SetValue(HeightProperty, _value);

        //////////////////////////////////////////////////////////////////////////////////
        public static double GetOpacity(DependencyObject _obj)
            => (double)_obj.GetValue(OpacityProperty);

        //////////////////////////////////////////////////////////////////////////////////
        public static void SetOpacity(DependencyObject _obj, double _value)
            => _obj.SetValue(OpacityProperty, _value);
    }
}
