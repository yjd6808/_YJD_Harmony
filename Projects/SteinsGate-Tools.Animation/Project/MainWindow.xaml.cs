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
using System.Windows.Threading;

using Path = System.IO.Path;

namespace SteinsGate_Tools.AnimationTool
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private static string _binaryDir = Environment.CurrentDirectory;

        public MainWindow()
        {
            InitializeComponent();
            const double interval = 1.0f / 60.0f * 1000;

            DispatcherTimer timer = new();    //객체생성
            timer.Interval = TimeSpan.FromMilliseconds(interval); 
            timer.Tick += TimerOnTick;
            timer.Start();

            //Image image = new Image() { Source = new BitmapImage(new Uri(Path.Combine(_binaryDir, "image/1.png") )) };
            //MainCanvas.Children.Add(image);
        }

        private void TimerOnTick(object? sender, EventArgs e)
        {

        }

        private void MainCanvas_OnMouseMove(object sender, MouseEventArgs e)
        {
            throw new NotImplementedException();
        }

        private void MainCanvas_OnMouseDown(object sender, MouseButtonEventArgs e)
        {

        }

        private void ToggleButton_Check(object sender, RoutedEventArgs e)
        {
        }
    }
}
