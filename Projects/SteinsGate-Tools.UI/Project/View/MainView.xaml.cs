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
using SGToolsCommon.Custom;
using SGToolsCommon.Sga;
using SGToolsCommon.ThirdParty;
using SGToolsUI.ViewModel;

namespace SGToolsUI.View
{
    public partial class MainView : Window
    {
        public MainViewModel ViewModel { get; }

        public MainView()
        {
            ViewModel = new MainViewModel();
            ViewModel.View = this;
            Resources.Add("ViewModel", ViewModel);
            InitializeComponent();

            var f = PackageListBox.Items[0];
            SetVerticalScrollToEnd();
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
            //MainCanvas.ScrollToVerticalOffset(20000);
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

      
        private void View_OnMouseWheel(object sender, MouseWheelEventArgs e)
        {
            double prevWidth = MainCanvas.Width;
            double prevHeight = MainCanvas.Height;

            if (e.Delta > 0)
            {
                MainCanvas.Width += 10.0f;
                MainCanvas.Height = MainCanvas.Width * Constant.ResoltionRatio;
            }
            else
            {
                if (MainCanvas.Width <= 100.0f)
                    return;

                MainCanvas.Width -= 10.0f;
                MainCanvas.Height = MainCanvas.Width * Constant.ResoltionRatio;
            }

            double deltaX = MainCanvas.Width / Constant.ResolutionWidth;
            double deltaY = MainCanvas.Height / Constant.ResolutionHeight;

            ScaleTransform scaleTransform = new ScaleTransform(deltaX, deltaY);
            MainCanvas.LayoutTransform = scaleTransform;

            Debug.WriteLine(this.ActualWidth);
            Debug.WriteLine(MainDockPanel.ActualWidth);
        }
    }
}
