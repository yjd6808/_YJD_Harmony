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

namespace SGToolsCommon.Custom
{
    /// <summary>
    /// Interaction logic for PlaceholderImageListBox.xaml
    /// </summary>
    public partial class PlaceholderImageListBox : ListBox
    {


        public ImageSource PlaceholderImageSource
        {
            get { return (ImageSource)GetValue(PlaceholderImageSourceProperty); }
            set { SetValue(PlaceholderImageSourceProperty, value); }
        }

        public double PlaceholderImageSourceWidth
        {
            get { return (double)GetValue(PlaceholderImageSourceWidthProperty); }
            set { SetValue(PlaceholderImageSourceWidthProperty, value); }
        }

        public double PlaceholderImageSourceHeight
        {
            get { return (double)GetValue(PlaceholderImageSourceHeightProperty); }
            set { SetValue(PlaceholderImageSourceHeightProperty, value); }
        }

        public double PlaceholderImageSourceOpacity
        {
            get { return (double)GetValue(PlaceholderImageSourceOpacityProperty); }
            set { SetValue(PlaceholderImageSourceOpacityProperty, value); }
        }

        public PlaceholderImageListBox()
        {
            InitializeComponent();
        }

        public static readonly DependencyProperty PlaceholderImageSourceProperty = DependencyProperty.Register(
            "PlaceholderImageSource",
            typeof(ImageSource),
            typeof(PlaceholderImageListBox),
            new PropertyMetadata(null) );

        public static readonly DependencyProperty PlaceholderImageSourceWidthProperty = DependencyProperty.Register(
            "PlaceholderImageSourceWidth",
            typeof(double),
            typeof(PlaceholderImageListBox),
            new PropertyMetadata(null) { DefaultValue = 30.0 });

        public static readonly DependencyProperty PlaceholderImageSourceHeightProperty = DependencyProperty.Register(
            "PlaceholderImageSourceHeight",
            typeof(double),
            typeof(PlaceholderImageListBox),
            new PropertyMetadata(null) { DefaultValue = 30.0 } );

        public static readonly DependencyProperty PlaceholderImageSourceOpacityProperty = DependencyProperty.Register(
            "PlaceholderImageSourceOpacity",
            typeof(double),
            typeof(PlaceholderImageListBox),
            new PropertyMetadata(null) { DefaultValue = 0.5 });
    }
}
