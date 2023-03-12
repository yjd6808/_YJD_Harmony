﻿using System;
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
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Resources;
using System.Windows.Shapes;
using System.Windows.Threading;
using MoreLinq.Extensions;
using SGToolsCommon;
using SGToolsCommon.CustomControl;
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;
using SGToolsCommon.ThirdParty;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.CustomControl;
using SGToolsUI.FileSystem;
using SGToolsUI.Model;
using SGToolsUI.ViewModel;
using Xceed.Wpf.AvalonDock.Controls;
using Xceed.Wpf.AvalonDock.Properties;

namespace SGToolsUI.View
{
    public partial class MainView : Window
    {

        
        public MainViewModel ViewModel { get; }

        public MainView()
        {
            ViewModel = new MainViewModel();
            ViewModel.View = this;
            ViewModel.KeyState.KeyDown += MainView_OnKeyDown;
            ViewModel.KeyState.KeyUp += MainView_OnKeyUp;
            InitializeComponent();

            if (!Constant.UseDebugData)
                return;

            #region DebugElements

            ViewModel.GroupMaster = SGUIGroupMaster.Create(ViewModel);
            ViewModel.GroupMaster.Children.Add(new SGUIGroup()
            {
                VisualName = "그룹 1",
                Children = new ObservableCollection<SGUIElement>()
                {
                    new SGUIButton() { VisualName = "버튼 1-1"},
                    new SGUIButton() { VisualName = "버튼 1-2"},
                    new SGUIButton() { VisualName = "버튼 1-3"},
                    new SGUIButton() { VisualName = "버튼 1-4"},
                    new SGUIGroup()
                    {
                        VisualName = "그룹 1-5",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { VisualName = "버튼 1-5-1"},
                            new SGUIButton() { VisualName = "버튼 1-5-2"},
                            new SGUIButton() { VisualName = "버튼 1-5-3"},
                            new SGUIButton() { VisualName = "버튼 1-5-4"},
                            new SGUIGroup()
                            {
                                VisualName = "그룹 1-5-5"
                            }
                        }
                    },
                    new SGUIGroup()
                    {
                        VisualName = "그룹 1-6",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { VisualName = "버튼 1-6-1"},
                            new SGUIButton() { VisualName = "버튼 1-6-2"},
                            new SGUIButton() { VisualName = "버튼 1-6-3"},
                            new SGUIButton() { VisualName = "버튼 1-6-4"},
                            new SGUIGroup()
                            {
                                VisualName = "그룹 1-6-5"
                            }
                        }
                    }
                }

            });
            ViewModel.GroupMaster.Children.Add(new SGUIGroup()
            {
                VisualName = "그룹 2",
                Children = new ObservableCollection<SGUIElement>()
                {
                    new SGUIButton() { VisualName = "버튼 2-1"},
                    new SGUIButton() { VisualName = "버튼 2-2"},
                    new SGUIButton() { VisualName = "버튼 2-3"},
                    new SGUIButton() { VisualName = "버튼 2-4"},
                    new SGUIGroup()
                    {
                        VisualName = "그룹 2-5",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { VisualName = "버튼 2-5-1"},
                            new SGUIButton() { VisualName = "버튼 2-5-2"},
                            new SGUIButton() { VisualName = "버튼 2-5-3"},
                            new SGUIButton() { VisualName = "버튼 2-5-4"},
                            new SGUIGroup()
                            {
                                VisualName = "그룹 2-5-5"
                            }
                        }
                    },
                    new SGUIGroup()
                    {
                        VisualName = "그룹 2-6",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { VisualName ="버튼 2-6-1"},
                            new SGUIButton() { VisualName ="버튼 2-6-2"},
                            new SGUIButton() { VisualName ="버튼 2-6-3"},
                            new SGUIButton() { VisualName ="버튼 2-6-4"},
                            new SGUIGroup()
                            {
                                VisualName = "그룹 2-6-5"
                            }
                        }
                    }
                }
            });



            ViewModel.GroupMaster.Children.Add(new SGUIGroup() { VisualName = "그룹 3" });
            ViewModel.GroupMaster.Children.Add(new SGUIGroup() { VisualName = "그룹 4" });


            void DebugManualUpdate(SGUIGroup group)
            {
                group.VisualSize = new Size(Constant.ResolutionWidth, Constant.ResolutionHeight);
                group.ViewModel = ViewModel;

                if (!group.IsMaster)
                {
                    ViewModel.GroupMaster.AddGroup(group);
                    group.SetDepth(group.Parent.Depth + 1);
                }

                group.Children.ForEach(x =>
                {
                    x.Parent = group;
                    x.ViewModel = ViewModel;

                    if (x.IsGroup)
                        DebugManualUpdate(x.Cast<SGUIGroup>());
                    else
                        ViewModel.GroupMaster.AddElement(x);
                });
            }

            // 임시데이트 기본 데이터 주입
            DebugManualUpdate(ViewModel.GroupMaster);

            #endregion

        }

        private void MainView_OnLoaded(object sender, RoutedEventArgs e)
        {
            ViewModel.Loaded();
            InitializeZoomStateBinding();       // 컴포넌트가 모두 초기화된 후에 윈도우 사이즈가 최종결정되기 때문에.. Xaml에서 작성하지 않고 C# 코드로 작성하도록 한다.
            InitializeDragTargets();
        }


        private async void MainView_OnClosing(object? sender, CancelEventArgs e)
        {
            Task t1 = ViewModel.Saver.SaveAutoAsync(SaveMode.UIToolData, false);
            Task t2 = ViewModel.Exporter.ExportAsync();

            await t1;
            await t2;

            ViewModel.LogView.Close();
            ViewModel.JobQueue.Dispose();
            ViewModel.KeyState.Dispose();
            ViewModel.Commander.Finalize();
        }

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

        private void InitializeDragTargets()
        {
            ViewModel.DragState.EndTargets.Add(UIElementPropertyGrid);
            ViewModel.DragState.EndTargets.Add(UIElementTreeView);
        }

        private async void MainView_OnKeyDown(SGKey key)
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
                    await ViewModel.Saver.SaveAutoAsync(SaveMode.Full, false);
                }
                else if (state.IsPressed(SGKey.L))
                {
                    ViewModel.Commander.OpenLogView.Execute(null);
                }

            }

            if (state.IsPressed(SGKey.Escape))
            {
                ViewModel.GroupMaster.DeselectAll();
                ViewModel.GroupMaster.Depick();
                ViewModel.Commander.ClipboardOperateUIElement.Clear();
            }

            else if (state.IsPressed(SGKey.F6) && MessageBoxEx.ShowTopMost("다시 로딩하시겠습니까?", "질문", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.Yes)
            {
                ViewModel.Commander.FileUIToolDataLoadAsync.Execute(SGUIFileSystem.LoadKey);
            }

            else if (state.IsPressed(SGKey.F7))
            {
                BackUpTextBox.Focus();
                BackUpTextBox.Text = string.Empty;
            }

            else if (state.IsPressed(SGKey.X))
            {
                ViewModel.IsEventMode = !ViewModel.IsEventMode;

                if (ViewModel.IsEventMode)
                    ViewModel.GroupMaster.PickedElements.ForEach(element => element.State = SGUIElement.StateNormal);
            }

            else if (state.IsPressed(SGKey.C) && ViewModel.IsEventMode)
            {
                ViewModel.GroupMaster.PickedElements.ForEach(element => element.State = SGUIElement.StateDisabled);
            }

            CanvasShapesControl.OnKeyDown(key);
            UIElementTreeView.OnKeyDown(key);
            UIElementPropertyGrid.OnKeyDown(key);
        }



        private void MainView_OnKeyUp(SGKey key)
        {
            KeyState state = ViewModel.KeyState;
            if (!state.IsShiftPressed && !state.IsCtrlPressed)
            {
                TitlePanel.Draggable = true;
                ViewModel.UIElementSelectMode = SelectMode.New;
            }

            CanvasShapesControl.OnKeyUp(key);
            // UIElementTreeView.OnKeyUp(key);
        }

        public void MainView_OnPreviewMouseMove(object sender, MouseEventArgs e)
        {
            ViewModel.DragState.OnDragMove(e.GetPosition(this));
        }

        public void MainView_OnPreviewMouseUp(object sender, MouseButtonEventArgs e)
        {
            ViewModel.DragState.OnDragEnd(e.GetPosition(this));
        }

        public void MainView_OnMouseLeave(object sender, MouseEventArgs e)
        {
            CanvasShapesControl.DragEnd(e);
        }

        private void SgaResourceSearchTextBox_OnKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key != Key.Enter)
                return;

            ViewModel.Commander.SearchSgaResource.Execute(SgaResourceSearchTextBox.Text);
        }

        private async void BackUpTextBox_OnKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key != Key.Enter)
                return;

            string backuptag = BackUpTextBox.Text.Trim();
            if (backuptag.Length == 0)
                return;

            await ViewModel.Saver.BackupAsync(backuptag);
            BackUpTextBox.Text = string.Empty;
            BackUpTextBox.FocusClear();

        }
    }
}
