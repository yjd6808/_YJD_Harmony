/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 8:33:20 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using SGToolsCommon;
using SGToolsCommon.Customize.Control;
using SGToolsCommon.Customize.View;
using SGToolsCommon.Extension;
using SGToolsCommon.Model;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using SGToolsUI.Command;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.Command.MainViewCommand.Async;
using SGToolsUI.FileSystem;
using SGToolsUI.Model.Main;
using SGToolsUI.Model.Setting;
using SGToolsUI.View;

namespace SGToolsUI.ViewModel
{
    public class MainViewModel : Bindable
    {
        private SelectMode uiElementSelectMode_;
        private KeyState keyState_ = new();
        private ZoomState zoomState_ = new();
        private Vector mouseOnWindow_ = new(0, 0);
        private Vector mouseOnMonitor_ = new(0, 0);
        private Vector mouseOnCanvas_ = new(0, 0);
        private string resourceSelectionStatus_ = string.Empty;
        private string canvasSelectionStatus_ = string.Empty;
        private bool isEventMode_ = false;
        private SgaPackage selectedPackage_ = new();
        private SgaImage selectedImage_ = new();
        private IKeyboardInputReceiver? selectedKeyboardInputReceiver_;
        private SgaSpriteAbstract selectedSprite_ = new SgaSprite();
        private SGUIGroupMaster groupMaster_ = null!;

        //////////////////////////////////////////////////////////////////////////////////
        public MainViewModel()
        {
            PackManager = SgaManager.Instance;
            Setting = new Setting();
            Setting.Load();
            LogBox = new LogListBox() { MaxItemCount = 1500 };
            LogBox.Width = 400;
            LogBox.Height = 600;
            GroupMaster = SGUIGroupMaster.Create(this);
            LogErrorHandler = new Action<Exception>((ex) => LogBox.AddLog(ex));
            LogView = new LogView(LogBox);
            AlbumView = new AlbumView(this);
            BackupView = new BackupView(this);
            Loader = new SGUILoader(this);
            Saver = new SGUISaver(this);
            Exporter = new SGUIExporter(this);
            Commander = new MainCommandCenter(this);
            Commander.Execute(nameof(ReloadSgaPackage));
            DragState = new DataDragState();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void Loaded()
        {
            KeyboardInputReceivers.Add(View.UIElementTreeView);
            KeyboardInputReceivers.Add(View.UIElementPropertyGrid);
            KeyboardInputReceivers.Add(View.CanvasShapesControl);

            double zoomLevelDelta = (double)Setting.ZoomLevel / 100 - 1.0;

            ZoomState.ZoomLevelY += zoomLevelDelta * Constant.ResolutionRatio;
            ZoomState.ZoomLevelX += zoomLevelDelta;

            // ZoomLevel 업데이트를 한다고해서 윈도우의 Width, Height 업데이트가 즉시 이뤄지지 않는다.
            // 이번 업데이트가 지나간 후 가운데로 옮겨주기 위해서 BeginInvoke로 처리하였다.
            View.Dispatcher.BeginInvoke(async () =>
            {
                await Task.Delay(10);
                View.MoveToClosestDisplayCenter();
            });

            LogBox.Style = (Style)Application.Current.FindResource("LogListBox");

            if (Setting.ShowLogViewWhenProgramLaunched)
                LogView.Show();

            if (Setting.ShowLogViewWhenProgramLaunched && Setting.LogViewPositionWhenProgramLaunched != IntPoint.Zero)
                LogView.MoveTo(Setting.LogViewPositionWhenProgramLaunched);

            if (!Constant.UseDebugData)
            {
                Commander.Execute(nameof(FileUIToolDataLoadAsync), SGUIFileSystem.LoadKey); // 그룹마스터 로딩
                return;
            }

            #pragma warning disable CS0162
            #region DebugElements

            this.GroupMaster = SGUIGroupMaster.Create(this);
            this.GroupMaster.Children.Add(new SGUIGroup()
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
            this.GroupMaster.Children.Add(new SGUIGroup()
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
                            new SGUIButton() { VisualName = "버튼 2-6-1"},
                            new SGUIButton() { VisualName = "버튼 2-6-2"},
                            new SGUIButton() { VisualName = "버튼 2-6-3"},
                            new SGUIButton() { VisualName = "버튼 2-6-4"},
                            new SGUIGroup()
                            {
                                VisualName = "그룹 2-6-5"
                            }
                        }
                    }
                }
            });

            this.GroupMaster.Children.Add(new SGUIGroup() { VisualName = "그룹 3" });
            this.GroupMaster.Children.Add(new SGUIGroup() { VisualName = "그룹 4" });

            void DebugManualUpdate(SGUIGroup _group)
            {
                _group.VisualSize = new Size(Constant.ResolutionWidth, Constant.ResolutionHeight);
                _group.ViewModel = this;

                if (!_group.IsMaster)
                {
                    this.GroupMaster.AddGroup(_group);
                    _group.SetDepth(_group.Parent.Depth + 1);
                }

                _group.Children.ForEach((_, x) =>
                {
                    x.Parent = _group;
                    x.ViewModel = this;

                    if (x.IsGroup)
                        DebugManualUpdate(x.Cast<SGUIGroup>());
                    else
                        this.GroupMaster.AddElement(x);
                });
            }

            // 임시데이터 기본 데이터 주입
            DebugManualUpdate(this.GroupMaster);

            #endregion
            #pragma warning restore CS0162
        }

        public MainView View { get; set; } = null!;
        public Setting Setting { get; set; }
        public MainCommandCenter Commander { get; }
        public SgaManager PackManager { get; }

        public ZoomState ZoomState
        {
            get => zoomState_;
            set
            {
                zoomState_ = value;
                OnPropertyChanged();
            }
        }

        public KeyState KeyState
        {
            get => keyState_;
            set
            {
                keyState_ = value;
                OnPropertyChanged();
            }
        }

        public string ResourceSelectionStatus
        {
            get => resourceSelectionStatus_;
            set
            {
                resourceSelectionStatus_ = value;
                OnPropertyChanged();
            }
        }

        public string CanvasSelectionStatus
        {
            get => canvasSelectionStatus_;
            set
            {
                canvasSelectionStatus_ = value;
                OnPropertyChanged();
            }
        }

        public Vector MouseOnMonitor
        {
            get => mouseOnMonitor_;
            set
            {
                mouseOnMonitor_ = value;
                OnPropertyChanged();
            }
        }

        public Vector MouseOnWindow
        {
            get => mouseOnWindow_;
            set
            {
                mouseOnWindow_ = value;
                OnPropertyChanged();
            }
        }

        public Vector MouseOnCanvas
        {
            get => mouseOnCanvas_;
            set
            {
                mouseOnCanvas_ = value;
                OnPropertyChanged();
            }
        }

        public SgaPackage SelectedPackage
        {
            get => selectedPackage_;
            set
            {
                selectedPackage_ = value;
                OnPropertyChanged();
            }
        }

        public SgaImage SelectedImage
        {
            get => selectedImage_;
            set
            {
                selectedImage_ = value;
                OnPropertyChanged();
            }
        }

        public SgaSpriteAbstract SelectedSprite
        {
            get => selectedSprite_;
            set
            {
                selectedSprite_ = value;
                OnPropertyChanged();
            }
        }

        public SGUIGroupMaster GroupMaster
        {
            get => groupMaster_;
            set
            {
                if (groupMaster_ != null)
                    groupMaster_.Clear();

                groupMaster_ = value;
                OnPropertyChanged();
            }
        }

        public SelectMode UIElementSelectMode
        {
            get => uiElementSelectMode_;
            set
            {
                uiElementSelectMode_ = value;
                OnPropertyChanged();
            }
        }

        public bool IsEventMode //  클릭 등 이벤트 처리를 간접적으로 수행하기 위한 용도/ UIElementItemsControl.cs, OnMouseDownEventMode()
        {
            get => isEventMode_;
            set
            {
                isEventMode_ = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(IsNotEventMode));
            }
        }

        public IKeyboardInputReceiver? FocusedKeyboardInputReceiver
        {
            get => selectedKeyboardInputReceiver_;
            set
            {
                if (selectedKeyboardInputReceiver_ != null && selectedKeyboardInputReceiver_ != value)
                    selectedKeyboardInputReceiver_.OnLostFocus();

                selectedKeyboardInputReceiver_ = value;
                OnPropertyChanged();
            }
        }

        public bool IsNotEventMode => !IsEventMode;
        public bool Terminated { get; set; }
        public DataDragState DragState { get; }
        public JobQueue JobQueue { get; } = new();
        public LogListBox LogBox { get; }
        public LogView LogView { get; }
        public ProgressView ProgressView { get; } = new();
        public AlbumView AlbumView { get; }
        public BackupView BackupView { get; }
        public SGUILoader Loader { get; }
        public SGUISaver Saver { get; }
        public SGUIExporter Exporter { get; }
        public List<IKeyboardInputReceiver> KeyboardInputReceivers = new();
        public readonly Action<Exception> LogErrorHandler;
    }
}
