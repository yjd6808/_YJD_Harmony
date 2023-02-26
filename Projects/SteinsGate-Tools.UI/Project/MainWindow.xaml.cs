using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
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

namespace SGToolsUI
{

    public class TestSize : INotifyPropertyChanged
    {
        private int _width = 0;
        private int _height = 0;

        public int Width
        {
            get { return (int)_width;}
            set
            {
                _width = value;
                OnPropertyChanged("Width");
            }
        }
        public int Height
        {
            get { return (int)_height; }
            set
            {
                _height = value;
                OnPropertyChanged("Height");
            }
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

    }
    

    public class Student
    {
        // XAML에서 생성가능하도록 하기위한 빈 생성자
        public Student()
        {
            Age = 0;
            Name = "";
        }

        // C#에서 활용하기위한 생성자
        public Student(int age, string name)
        {
            Age = age;
            Name = name;
        }

        public int Age { get; set; }
        public string Name { get; set; }
    }





    public partial class MainWindow : Window
    {
        public static string Test = "오하요";

        private static string _binaryDir = Environment.CurrentDirectory;

        


        public MainWindow()
        {
            InitializeComponent();
            SetVerticalScrollToEnd();

         

            // s.Width -> 100
            // s.Height -> 100
            // 타겟 -> 소스로 전파 제대로 됨.
        }

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

        private void TimerOnTick(object? sender, EventArgs e)
        {
        }

        private void MainWindow_OnMouseWheel(object sender, MouseWheelEventArgs e)
        {
            double prevWidth = MainCanvasScrollViewer.Width;
            double prevHeight = MainCanvasScrollViewer.Height;

            if (e.Delta > 0)
            {
                MainCanvasScrollViewer.Width += 10.0f;
                MainCanvasScrollViewer.Height = MainCanvasScrollViewer.Width * Constant.ResoltionRatio;
            }
            else
            {
                if (MainCanvasScrollViewer.Width <= 100.0f)
                    return;

                MainCanvasScrollViewer.Width -= 10.0f;
                MainCanvasScrollViewer.Height = MainCanvasScrollViewer.Width * Constant.ResoltionRatio;
            }

            double deltaX = MainCanvasScrollViewer.Width / Constant.ResolutionWidth;
            double deltaY = MainCanvasScrollViewer.Height / Constant.ResolutionHeight;

            ScaleTransform scaleTransform = new ScaleTransform(deltaX, deltaY);
            MainCanvas.LayoutTransform = scaleTransform;

            var g = MainCanvas.Children[0];
            Debug.WriteLine(g);

            
        }
    }
}
