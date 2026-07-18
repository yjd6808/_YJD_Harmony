/*
 * 작성자: 윤정도
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media.Animation;
using SGToolsCommon.Customize.Control;
using SGToolsCommon.Extension;
using SGToolsCommon.Model;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.FileSystem;
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;
using Xceed.Wpf.Toolkit.PropertyGrid;

using Path = System.IO.Path;

namespace SGToolsUI.View
{
    public partial class MainView : Window
    {
        public MainViewModel ViewModel { get; }

        //////////////////////////////////////////////////////////////////////////////////
        public MainView()
        {
            ViewModel = new MainViewModel();
            ViewModel.View = this;
            ViewModel.KeyState.KeyDown += MainView_OnKeyDown;
            ViewModel.KeyState.KeyUp += MainView_OnKeyUp;
            InitializeComponent();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void MainView_OnLoaded(object _sender, RoutedEventArgs _e)
        {
            ViewModel.Loaded();
            InitializeZoomStateBinding();       // 컴포넌트가 모두 초기화된 후에 윈도우 사이즈가 최종결정되기 때문에.. Xaml에서 작성하지 않고 C# 코드로 작성하도록 한다.
            InitializeDragTargets();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void MainView_OnClosing(object? _sender, CancelEventArgs _e)
        {
            ViewModel.Terminated = true;
            ViewModel.LogView.Close();
            ViewModel.AlbumView.Close();
            ViewModel.BackupView.Close();
            ViewModel.ProgressView.Close();
            ViewModel.JobQueue.Dispose();
            ViewModel.KeyState.Dispose();
            ViewModel.Commander.Finalize();
        }

        //////////////////////////////////////////////////////////////////////////////////
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

        //////////////////////////////////////////////////////////////////////////////////
        private void InitializeDragTargets()
        {
            ViewModel.DragState.EndTargets.Add(UIElementPropertyGrid);
            ViewModel.DragState.EndTargets.Add(UIElementsControl);
            ViewModel.DragState.EndTargets.Add(UIElementTreeView);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private async void MainView_OnKeyDown(SGKey _key)
        {
            KeyState state = ViewModel.KeyState;

            bool shift = state.IsShiftPressed;
            bool ctrl = state.IsCtrlPressed;
            bool alt = state.IsAltPressed;

            if (shift)
            {
                TitlePanel.Draggable = false;
                ViewModel.UIElementSelectMode = SelectMode.Keep;
            }

            // 컨트롤키를 우선토록 한다.
            if (ctrl)
            {
                TitlePanel.Draggable = false;
                ViewModel.UIElementSelectMode = SelectMode.KeepExcept;

                if (state.IsPressed(SGKey.S))
                {
                    string savePath = System.IO.Path.Combine(Environment.CurrentDirectory, Constant.UIToolDataFileName);
                    var saver = new SGUISaver(ViewModel);
                    await saver.SaveAsync(savePath, ViewModel.RootGroup);

                    DoubleAnimation animation = new DoubleAnimation();
                    animation.From = 0;
                    animation.To = 1;
                    animation.Duration = TimeSpan.FromMilliseconds(300);

                    UIElementTreeView.BeginAnimation(UIElement.OpacityProperty, animation);
                }
                else if (state.IsPressed(SGKey.L))
                {
                    ViewModel.Commander.OpenLogView.Execute(null);
                }
                else if (state.IsPressed(SGKey.B))
                {
                    ViewModel.Commander.OpenBackupView.Execute(null);
                }
            }

            if (!state.IsModifierKeyPressed)
            {
                if (state.IsPressed(SGKey.Escape))
                {
                    ViewModel.RootGroup.DeselectAll();
                    ViewModel.RootGroup.Depick();
                    ViewModel.Commander.ClipboardOperateUIElement.Clear();
                }

                else if (state.IsPressed(SGKey.F2) && ViewModel.RootGroup.SelectedElement != null)
                {
                    UIElementPropertyGrid.SelectWithPropertyFocus(ViewModel.RootGroup.SelectedElement, SGUIElement.VisualNameKey);
                }

                else if (state.IsPressed(SGKey.F6) && ViewModel.CurrentXmlFilePath != null && MessageBoxEx.ShowTopMost("다시 로딩하시겠습니까?", "질문", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.Yes)
                {
                    _ = ReloadCurrentXmlAsync();
                }

                else if (state.IsPressed(SGKey.F7))
                {
                    BackUpTextBox.Focus();
                    BackUpTextBox.Text = string.Empty;
                }

                else if (state.IsPressed(SGKey.F8))
                {
                    ViewModel.Commander.OpenDirectory.Execute(ToolDirectory.Backup.ToString());
                }
            }

            ViewModel.FocusedKeyboardInputReceiver?.OnKeyDown(_key);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void MainView_OnKeyUp(SGKey _key)
        {
            KeyState state = ViewModel.KeyState;
            if (!state.IsShiftPressed && !state.IsCtrlPressed)
            {
                TitlePanel.Draggable = true;
                ViewModel.UIElementSelectMode = SelectMode.New;
            }

            CanvasShapesControl.OnKeyUp(_key);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void MainView_OnPreviewMouseMove(object _sender, MouseEventArgs _e)
        {
            IntPoint pos = _e.GetPosition(this);

            if (!CanvasShapesControl.ContainPoint(pos))
                CanvasShapesControl.DragMove(_e);

            if (!UIElementsControl.ContainPoint(pos))
                UIElementsControl.OnMouseMoveManipulation(_e);

            ViewModel.DragState.OnDragMove(pos);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void MainView_OnPreviewMouseUp(object _sender, MouseButtonEventArgs _e)
        {
            CanvasShapesControl.DragEnd(_e);
            UIElementsControl.OnMouseUpManipulation();
            ViewModel.DragState.OnDragEnd(_e.GetPosition(this));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void MainView_OnMouseLeave(object _sender, MouseEventArgs _e)
        {
            CanvasShapesControl.DragEnd(_e);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SgaResourceSearchTextBox_OnKeyDown(object _sender, KeyEventArgs _e)
        {
            if (_e.Key != Key.Enter)
                return;

            ViewModel.Commander.SearchSgaResource.Execute(SgaResourceSearchTextBox.Text);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private async void BackUpTextBox_OnKeyDown(object _sender, KeyEventArgs _e)
        {
            if (_e.Key != Key.Enter)
                return;

            string backuptag = BackUpTextBox.Text.Trim();
            if (backuptag.Length == 0)
                return;

            var saver = new SGUISaver(ViewModel);
            await saver.BackupAsync(backuptag);
            BackUpTextBox.Text = string.Empty;
            BackUpTextBox.FocusClear();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private async void MainView_Drop(object _sender, DragEventArgs _e)
        {
            if (_e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                List<string> files = ((string[])_e.Data.GetData(DataFormats.FileDrop)).ToList();

                // 이미지 파일이 아닌녀석들 제거
                if (files.Count > 1)
                {
                    MessageBoxEx.ShowTopMost("하나의 .xml 파일만 드래그 앤 드롭 해주세요.");
                    return;
                }

                string file = files[0];

                if (Path.GetExtension(file) != ".xml")
                {
                    MessageBoxEx.ShowTopMost(".xml 형식의 파일이 아닙니다.");
                    return;
                }

                var loader = new SGUILoader(ViewModel);
                ViewModel.RootGroup = await loader.LoadAsync(file);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private async Task ReloadCurrentXmlAsync()
        {
            try
            {
                var loader = new SGUILoader(ViewModel);
                SGUIRootGroup loaded = await loader.LoadAsync(ViewModel.CurrentXmlFilePath!);
                ViewModel.RootGroup = loaded;
            }
            catch (Exception ex)
            {
                ViewModel.LogBox.AddLog($"다시 로딩 실패", ex.Message);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void MainView_OnPreviewMouseDown(object _sender, MouseButtonEventArgs _e)
        {
            IntPoint p = _e.GetPosition(this);

            ViewModel.FocusedKeyboardInputReceiver = ViewModel.KeyboardInputReceivers.FirstOrDefault(receiver => ((FrameworkElement)receiver).ContainPoint(p));
        }
    }
}
