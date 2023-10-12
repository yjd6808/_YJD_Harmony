using SGToolsCommon;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
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

using MoreLinq;
using System.Collections;
using System.Threading;
using Microsoft.Win32;

namespace SGToolsResearch
{

    public partial class MainWindow : Window
    {
        private double previousScale = 1.0;
        private Point previousPosition = new Point(0, 0);

        public static readonly string Str = "안\r\n녕";

        public static readonly BitmapImage  g1 = new BitmapImage(new Uri(SGToolsCommon.Resource.R.IconCommonDownKey));
        public static readonly BitmapImage g2 = new BitmapImage(new Uri(SGToolsCommon.Resource.R.IconCommonCheckedKey));
        public static readonly BitmapImage g3 = new BitmapImage(new Uri(SGToolsCommon.Resource.R.IconCommonMusicKey));
        public static readonly BitmapImage g4 = new BitmapImage(new Uri(SGToolsCommon.Resource.R.IconCommonUpKey));
        public MainWindow()
        {
            InitializeComponent();
            Loaded += OnLoaded;

            foreach (var aa in Str)
            {
                int g = (int)aa;
                Debug.WriteLine(g);
            }

            //65
            //92
            //114
            //92
            //110
            //66
        }


        private void OnLoaded(object sender, RoutedEventArgs e)
        {
        }

    }
}
