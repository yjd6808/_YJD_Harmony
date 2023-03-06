using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Runtime.CompilerServices;
using System.Security.Policy;
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
using System.Windows.Resources;
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
            Timer.Interval = TimeSpan.FromSeconds(1 / 60.0f);
            Timer.Tick += TimerTick;
            Resources.Add("ViewModel", ViewModel);

            InitializeComponent();

            // Mouse.OverrideCursor = SGToolsCommon.Resource.R.DragAndDropCursor.Value;


            #region  A



            ViewModel.GroupMaster.Children.Add(new SGUIGroup(0)
            {
                VisualName = "그룹 1",
                Children = new ObservableCollection<SGUIElement>()
                {
                    new SGUIButton() { VisualName = "그룹 1-1"},
                    new SGUIButton() { VisualName = "그룹 1-2"},
                    new SGUIButton() { VisualName = "그룹 1-3"},
                    new SGUIButton() { VisualName = "그룹 1-4"},
                    new SGUIGroup(1)
                    {
                        VisualName = "그룹 1-5",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { VisualName = "그룹 1-5-1"},
                            new SGUIButton() { VisualName = "그룹 1-5-2"},
                            new SGUIButton() { VisualName = "그룹 1-5-3"},
                            new SGUIButton() { VisualName = "그룹 1-5-4"},
                            new SGUIGroup(2)
                            {
                                VisualName = "그룹 1-5-5"
                            }
                        }
                    },
                    new SGUIGroup(1)
                    {
                        VisualName = "그룹 1-6",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { VisualName = "그룹 1-6-1"},
                            new SGUIButton() { VisualName = "그룹 1-6-2"},
                            new SGUIButton() { VisualName = "그룹 1-6-3"},
                            new SGUIButton() { VisualName = "그룹 1-6-4"},
                            new SGUIGroup(2)
                            {
                                VisualName = "그룹 1-6-5"
                            }
                        }
                    }
                }

            });
            ViewModel.GroupMaster.Children.Add(new SGUIGroup(0)
            {
                VisualName = "그룹 2",
                Children = new ObservableCollection<SGUIElement>()
                {
                    new SGUIButton() { VisualName = "그룹 2-1"},
                    new SGUIButton() { VisualName = "그룹 2-2"},
                    new SGUIButton() { VisualName = "그룹 2-3"},
                    new SGUIButton() { VisualName = "그룹 2-4"},
                    new SGUIGroup(1)
                    {
                        VisualName = "그룹 2-5",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { VisualName = "그룹 2-5-1"},
                            new SGUIButton() { VisualName = "그룹 2-5-2"},
                            new SGUIButton() { VisualName = "그룹 2-5-3"},
                            new SGUIButton() { VisualName = "그룹 2-5-4"},
                            new SGUIGroup(2)
                            {
                                VisualName = "그룹 2-5-5"
                            }
                        }
                    },
                    new SGUIGroup(1)
                    {
                        VisualName = "�׷� 2-6",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { VisualName ="그룹 2-6-1"},
                            new SGUIButton() { VisualName ="그룹 2-6-2"},
                            new SGUIButton() { VisualName ="그룹 2-6-3"},
                            new SGUIButton() { VisualName ="그룹 2-6-4"},
                            new SGUIGroup(2)
                            {
                                VisualName = "그룹 2-6-5"
                            }
                        }
                    }
                }
            });
            ViewModel.GroupMaster.Children.Add(new SGUIGroup(0) { VisualName = "그룹 3" });
            ViewModel.GroupMaster.Children.Add(new SGUIGroup(0) { VisualName = "그룹 4" });


            #endregion

            ViewModel.GroupMaster.ForEachRecursive(x => x.ViewModel = ViewModel);
            ViewModel.GroupMaster.____Update();

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
            InitializeZoomStateBinding();       // 컴포넌트가 모두 초기화된 후에 윈도우 사이즈가 최종결정되기 때문에.. Xaml에서 작성하지 않고 C# 코드로 작성하도록 한다.
        }

        


        // ================================================================================
        // XAML로 구현이 힘든 기능들
        // ================================================================================


        private void InitializeZoomStateBinding()
        {
            // 수동 너비, 높이 조절을 위해 메뉴얼로 전환
            SizeToContent = SizeToContent.Manual;
            ViewModel.ZoomState.BaseWindowWidth = ActualWidth;
            ViewModel.ZoomState.BaseWindowHeight = ActualHeight;

            // https://stackoverflow.com/questions/2673600/problems-with-binding-to-window-height-and-width
            // Width, Height 프로퍼티가 부모 요소의 레이아웃 로직을 따라 간다는 설명.
            // 윈도우에 부모 요소가 있었나 ㄷㄷ..
            // 그래서 MinWidth, MaxHeight에 각각 바인딩해줬다.
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

            // 컨트롤키를 우선토록 한다.
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

        protected override void OnPreviewMouseMove(MouseEventArgs e)
        {
            CanvasShapesControl.DragMove(e);
            ViewModel.DragState.OnDragMove(e.GetPosition(this));
        }

        protected override void OnPreviewMouseUp(MouseButtonEventArgs e)
        {
            CanvasShapesControl.DragEnd(e);
            ViewModel.DragState.OnDragEnd(e.GetPosition(this));
        }

        protected override void OnMouseLeave(MouseEventArgs e)
        {
            CanvasShapesControl.DragEnd(e);
            ViewModel.DragState.OnDragEnd(e.GetPosition(this));

        }

    }
}
