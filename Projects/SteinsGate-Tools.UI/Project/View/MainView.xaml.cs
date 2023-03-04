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
        public CanvasGrid GridRect { get; } = new (100, 1, Brushes.White);



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

            #region  a

            //#region MyRegion

            ViewModel.GroupMaster.Children.Add(new SGUIGroup(0)
            {
                VisualName = "�׷� 1",
                Children = new ObservableCollection<SGUIElement>()
                {
                    new SGUIButton() { VisualName = "��ư 1-1"},
                    new SGUIButton() { VisualName = "��ư 1-2"},
                    new SGUIButton() { VisualName = "��ư 1-3"},
                    new SGUIButton() { VisualName = "��ư 1-4"},
                    new SGUIGroup(1)
                    {
                        VisualName = "�׷� 1-5",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { VisualName = "��ư 1-5-1"},
                            new SGUIButton() { VisualName = "��ư 1-5-2"},
                            new SGUIButton() { VisualName = "��ư 1-5-3"},
                            new SGUIButton() { VisualName = "��ư 1-5-4"},
                            new SGUIGroup(2)
                            {
                                VisualName = "�׷� 1-5-5"
                            }
                        }
                    },
                    new SGUIGroup(1)
                    {
                        VisualName = "�׷� 1-6",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { VisualName = "��ư 1-6-1"},
                            new SGUIButton() { VisualName = "��ư 1-6-2"},
                            new SGUIButton() { VisualName = "��ư 1-6-3"},
                            new SGUIButton() { VisualName = "��ư 1-6-4"},
                            new SGUIGroup(2)
                            {
                                VisualName = "�׷� 1-6-5"
                            }
                        }
                    }
                }

            });
            ViewModel.GroupMaster.Children.Add(new SGUIGroup(0)
            {
                VisualName = "�׷� 2",
                Children = new ObservableCollection<SGUIElement>()
                {
                    new SGUIButton() { VisualName = "��ư 2-1"},
                    new SGUIButton() { VisualName = "��ư 2-2"},
                    new SGUIButton() { VisualName = "��ư 2-3"},
                    new SGUIButton() { VisualName = "��ư 2-4"},
                    new SGUIGroup(1)
                    {
                        VisualName = "�׷� 2-5",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { VisualName = "��ư 2-5-1"},
                            new SGUIButton() { VisualName = "��ư 2-5-2"},
                            new SGUIButton() { VisualName = "��ư 2-5-3"},
                            new SGUIButton() { VisualName = "��ư 2-5-4"},
                            new SGUIGroup(2)
                            {
                                VisualName = "�׷� 2-5-5"
                            }
                        }
                    },
                    new SGUIGroup(1)
                    {
                        VisualName = "�׷� 2-6",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { VisualName = "��ư 2-6-1"},
                            new SGUIButton() { VisualName = "��ư 2-6-2"},
                            new SGUIButton() { VisualName = "��ư 2-6-3"},
                            new SGUIButton() { VisualName = "��ư 2-6-4"},
                            new SGUIGroup(2)
                            {
                                VisualName = "�׷� 2-6-5"
                            }
                        }
                    }
                }
            });
            ViewModel.GroupMaster.Children.Add(new SGUIGroup(0) { VisualName = "�׷� 3" });
            ViewModel.GroupMaster.Children.Add(new SGUIGroup(0) { VisualName = "�׷� 4" });
            ViewModel.GroupMaster.ForEachRecursive(x => x.ViewModel = ViewModel);
            ViewModel.GroupMaster.UpdateParent();

            #endregion


            /*
            for (int i = 0; i < 100; ++i)
            {
                var g = new SGUIGroup() { Name = "�׷� 3", VisualPosition = new Point(10 * i, 10 * i) };
                g.SetViewModel(ViewModel); 
                ViewModel.Commander.Execute(nameof(AddUIElement), g);
            }
            */


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
            Debug.WriteLine(this.Focusable);
            ViewModel.KeyState.Down(e.Key);

            if (ViewModel.KeyState.IsShiftPressed)
            {
                TitlePanel.Draggable = false;
                ViewModel.UIElementSelectMode = SelectMode.Keep;
                Debug.WriteLine("���1");
            }

            // ��Ʈ��Ű�� �켱��� �Ѵ�.
            if (ViewModel.KeyState.IsCtrlPressed)
            {
                TitlePanel.Draggable = false;
                ViewModel.UIElementSelectMode = SelectMode.KeepExcept;
                Debug.WriteLine("���2");
            }
        }


        private void MainView_OnKeyUp(object sender, KeyEventArgs e)
        {
            ViewModel.KeyState.Up(e.Key);

            if (!ViewModel.KeyState.IsShiftPressed && !ViewModel.KeyState.IsCtrlPressed)
            {
                TitlePanel.Draggable = true;
                ViewModel.UIElementSelectMode = SelectMode.New;
                Debug.WriteLine("����");
            }
        } 
    }
}
