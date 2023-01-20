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

        public void AddChildBitmapImage(BitmapImage bitmap, double x, double y)
        {
            Image image = new Image() { Source = bitmap };
            MainCanvas.Children.Add(image);
            Canvas.SetLeft(image, x);
            Canvas.SetTop(image, MainCanvas.Height - bitmap.PixelHeight);
        }

        public void SetVerticalScrollToEnd()
        {
            MainCanvasScrollViewer.ScrollToVerticalOffset(20000);
        }

        // 높이 변경은 다시 보정해줘야함
        public void ChangeCanvasHeight(double width, double height)
        {
            double changedHeight = height - MainCanvas.Height;

            foreach (UIElement child in MainCanvas.Children)
            {
                double beforeTop = Canvas.GetTop(child);
                Canvas.SetTop(child, beforeTop + height);
            }
        }

        public MainWindow()
        {
            
            InitializeComponent();
            SetVerticalScrollToEnd();
            const double interval = 1.0f / 60.0f * 1000;
            
            
            DispatcherTimer timer = new();    //객체생성
            timer.Interval = TimeSpan.FromMilliseconds(interval); 
            timer.Tick += TimerOnTick;
            timer.Start();

            var bitmap = new BitmapImage(new Uri(Path.Combine(_binaryDir, "image/1.png")));

            AddChildBitmapImage(bitmap, 0, 0);
        }

        private void TimerOnTick(object? sender, EventArgs e)
        {

        }

        private void MainCanvas_OnMouseMove(object sender, MouseEventArgs e)
        {
        }

        private void MainCanvas_OnMouseDown(object sender, MouseButtonEventArgs e)
        {

        }

        private void ToggleButton_Check(object sender, RoutedEventArgs e)
        {
        }
    }
}
