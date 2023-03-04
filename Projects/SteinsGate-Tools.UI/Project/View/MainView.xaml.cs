using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
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
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;
using SGToolsCommon.ThirdParty;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.Model;
using SGToolsUI.ViewModel;
using Xceed.Wpf.AvalonDock.Controls;

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
        }

        private void TimerTick(object? sender, EventArgs e)
        {
        }


        private void MainView_OnClosing(object? sender, CancelEventArgs e)
        {
            ViewModel.Commander.Finalize();
        }

        private void MainView_OnLoaded(object sender, RoutedEventArgs e)
        {
            InitializeZoomStateBinding();       // ������Ʈ�� ��� �ʱ�ȭ�� �Ŀ� ������ ����� ���������Ǳ� ������.. Xaml���� �ۼ����� �ʰ� C# �ڵ�� �ۼ��ϵ��� �Ѵ�.

        }



        // ================================================================================
        // XAML�� ������ ���� ��ɵ�
        // ================================================================================


        private void InitializeZoomStateBinding()
        {
            // ���� �ʺ�, ���� ������ ���� �޴���� ��ȯ
            SizeToContent = SizeToContent.Manual;
            ViewModel.ZoomState.BaseWindowWidth = ActualWidth;
            ViewModel.ZoomState.BaseWindowHeight = ActualHeight;

            // https://stackoverflow.com/questions/2673600/problems-with-binding-to-window-height-and-width
            // Width, Height ������Ƽ�� �θ� ����� ���̾ƿ� ������ ���� ���ٴ� ����.
            // �����쿡 �θ� ��Ұ� �־��� ����..
            // �׷��� MinWidth, MaxHeight�� ���� ���ε������.
            Binding windowZoomStateWidthBinding = new Binding();
            windowZoomStateWidthBinding.Source = ViewModel.ZoomState;
            windowZoomStateWidthBinding.Path = new PropertyPath(nameof(ViewModel.ZoomState.BaseWindowWidth));
            windowZoomStateWidthBinding.Mode = BindingMode.OneWay;
            View.SetBinding(Window.MinWidthProperty, windowZoomStateWidthBinding);
            View.SetBinding(Window.MaxWidthProperty, windowZoomStateWidthBinding);

            Binding windowZoomStateHeightBinding = new Binding();
            windowZoomStateHeightBinding.Source = ViewModel.ZoomState;
            windowZoomStateHeightBinding.Path = new PropertyPath(nameof(ViewModel.ZoomState.BaseWindowHeight));
            windowZoomStateHeightBinding.Mode = BindingMode.OneWay;
            View.SetBinding(Window.MinHeightProperty, windowZoomStateHeightBinding);
            View.SetBinding(Window.MaxHeightProperty, windowZoomStateHeightBinding);
        }

        private void MainView_OnKeyDown(object sender, KeyEventArgs e)
        {
            KeyState state = ViewModel.KeyState;

            state.Down(e.Key);

            if (state.IsShiftPressed)
            {
                TitlePanel.Draggable = false;
                ViewModel.UIElementSelectMode = SelectMode.Keep;
            }

            // ��Ʈ��Ű�� �켱��� �Ѵ�.
            else if (state.IsCtrlPressed)
            {
                TitlePanel.Draggable = false;
                ViewModel.UIElementSelectMode = SelectMode.KeepExcept;
            }

            else if (state.IsPressed(Key.Escape))
                ViewModel.GroupMaster.DeselectAll();
        }


        private void MainView_OnKeyUp(object sender, KeyEventArgs e)
        {
            KeyState state = ViewModel.KeyState;

            state.Up(e.Key);

            if (!state.IsShiftPressed && !state.IsCtrlPressed)
            {
                TitlePanel.Draggable = true;
                ViewModel.UIElementSelectMode = SelectMode.New;
            }
        } 
    }
}
