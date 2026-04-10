using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Media.Imaging;
using Microsoft.Win32;
using SGToolsCommon;

namespace SGToolsResearch
{
    public partial class MainWindow : Window
    {
        private double previousScale_ = 1.0;
        private System.Windows.Point previousPosition_ = new System.Windows.Point(0, 0);

        public static readonly string Str = "안\r\n녕";

        public static readonly BitmapImage g1 = new BitmapImage(new Uri(SGToolsCommon.Resource.R.IconCommonDownKey));
        public static readonly BitmapImage g2 = new BitmapImage(new Uri(SGToolsCommon.Resource.R.IconCommonCheckedKey));
        public static readonly BitmapImage g3 = new BitmapImage(new Uri(SGToolsCommon.Resource.R.IconCommonMusicKey));
        public static readonly BitmapImage g4 = new BitmapImage(new Uri(SGToolsCommon.Resource.R.IconCommonUpKey));

        //////////////////////////////////////////////////////////////////////////////////
        public MainWindow()
        {
            InitializeComponent();
            Loaded += OnLoaded;

            foreach (var aa in Str)
            {
                int g = (int)aa;
                Debug.WriteLine(g);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnLoaded(object _sender, RoutedEventArgs _e)
        {
        }
    }
}
