/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 10:44:47 AM
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

namespace SGToolsCommon.CustomStyle
{
    public class PlaceholderImageListBoxProperty
    {
        public static ImageSource GetSource(DependencyObject obj)
            => (ImageSource)obj.GetValue(SourceProperty);
        public static void SetSource(DependencyObject obj, ImageSource value)
            => obj.SetValue(SourceProperty, value);

        public static double GetWidth(DependencyObject obj)
            => (double)obj.GetValue(WidthProperty);
        public static void SetWidth(DependencyObject obj, double value)
            => obj.SetValue(WidthProperty, value);

        public static double GetHeight(DependencyObject obj)
            => (double)obj.GetValue(HeightProperty);
        public static void SetHeight(DependencyObject obj, double value)
            => obj.SetValue(HeightProperty, value);


        public static double GetOpacity(DependencyObject obj)
            => (double)obj.GetValue(OpacityProperty);
        public static void SetOpacity(DependencyObject obj, double value)
            => obj.SetValue(OpacityProperty, value);


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
    }
}
