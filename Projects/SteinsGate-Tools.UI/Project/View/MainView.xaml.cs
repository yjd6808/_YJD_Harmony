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
using SGToolsCommon.CustomControl;
using SGToolsCommon.Sga;
using SGToolsCommon.ThirdParty;
using SGToolsUI.ViewModel;

namespace SGToolsUI.View
{
    public partial class MainView : Window
    {
        public MainViewModel ViewModel { get; }
        public DispatcherTimer Timer { get; }

        public MainView()
        {
            ViewModel = new MainViewModel();
            ViewModel.View = this;
            Timer = new DispatcherTimer();
            Timer.Interval = TimeSpan.FromSeconds(1/60.0f);
            Timer.Tick += TimerTick;
            Resources.Add("ViewModel", ViewModel);

            InitializeComponent();
            SetVerticalScrollToEnd();
        }

        private void TimerTick(object? sender, EventArgs e)
        {

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

      
        private void MainView_OnClosing(object? sender, CancelEventArgs e)
        {
            ViewModel.Commander.Finalize();
        }
    }
}
