/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 8:33:20 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
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
using MoreLinq.Extensions;
using SGToolsCommon;
using SGToolsCommon.CustomControl;
using SGToolsCommon.CustomView;
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
            View.Dispatcher.BeginInvoke(() => View.MoveToClosestDisplayCenter());

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



            this.GroupMaster.Children.Add(new SGUIGroup() { VisualName = "그룹 3" });
            this.GroupMaster.Children.Add(new SGUIGroup() { VisualName = "그룹 4" });


            void DebugManualUpdate(SGUIGroup group)
            {
                group.VisualSize = new Size(Constant.ResolutionWidth, Constant.ResolutionHeight);
                group.ViewModel = this;

                if (!group.IsMaster)
                {
                    this.GroupMaster.AddGroup(group);
                    group.SetDepth(group.Parent.Depth + 1);
                }

                group.Children.ForEach(x =>
                {
                    x.Parent = group;
                    x.ViewModel = this;

                    if (x.IsGroup)
                        DebugManualUpdate(x.Cast<SGUIGroup>());
                    else
                        this.GroupMaster.AddElement(x);
                });
            }

            // 임시데이트 기본 데이터 주입
            DebugManualUpdate(this.GroupMaster);

            #endregion
        }

        public MainView View { get; set; }
        public Setting Setting { get; set; }
        public MainCommandCenter Commander { get; }
        public SgaManager PackManager { get; }

        public ZoomState ZoomState
        {
            get => _zoomState;
            set
            {
                _zoomState = value;
                OnPropertyChanged();
            }
        }

        public KeyState KeyState
        {
            get => _keyState;
            set
            {
                _keyState = value;
                OnPropertyChanged();
            }
        }


        public string ResourceSelectionStatus
        {
            get => _resourceSelectionStatus;
            set
            {
                _resourceSelectionStatus = value;
                OnPropertyChanged();
            }
        }

        public string CanvasSelectionStatus
        {
            get => _canvasSelectionStatus;
            set
            {
                _canvasSelectionStatus = value;
                OnPropertyChanged();
            }
        }

        public Vector MouseOnMonitor
        {
            get => _mouseOnMonitor;
            set
            {
                _mouseOnMonitor = value;
                OnPropertyChanged();
            }
        }

        public Vector MouseOnWindow
        {
            get => _mouseOnWindow;
            set
            {
                _mouseOnWindow = value;
                OnPropertyChanged();
            }
        }

        public Vector MouseOnCanvas
        {
            get => _mouseOnCanvas;
            set
            {
                _mouseOnCanvas = value;
                OnPropertyChanged();
            }
        }

        public SgaPackage SelectedPackage
        {
            get => _selectedPackage;
            set
            {
                _selectedPackage = value;
                OnPropertyChanged();
            }
        }

        public SgaImage SelectedImage
        {
            get => _selectedImage;
            set
            {
                _selectedImage = value;
                OnPropertyChanged();
            }
        }

        public SgaSpriteAbstract SelectedSprite
        {
            get => _selectedSprite;
            set
            {
                _selectedSprite = value;
                OnPropertyChanged();
            }
        }

        public SGUIGroupMaster GroupMaster
        {
            get => _groupMaster;
            set
            {
                if (_groupMaster != null)
                    _groupMaster.Clear();

                _groupMaster = value;
                OnPropertyChanged();
            }
        }

        public SelectMode UIElementSelectMode
        {
            get => _uiElementSelectMode;
            set
            {
                _uiElementSelectMode = value;
                OnPropertyChanged();
            }
        }

        public bool IsEventMode //  클릭 등 이벤트 처리를 간접적으로 수행하기 위한 용도/ UIElementItemsControl.cs, OnMouseDownEventMode()
        {
            get => _isEventMode;
            set
            {
                _isEventMode = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(IsNotEventMode));
            }
        }

        public IKeyboardInputReceiver? FocusedKeyboardInputReceiver
        {
            get => _focusedKeyboardInputReceiver;
            set
            {
                if (_focusedKeyboardInputReceiver != null && _focusedKeyboardInputReceiver != value)
                    _focusedKeyboardInputReceiver.OnLostFocus();

                _focusedKeyboardInputReceiver = value;
                OnPropertyChanged();
            }
        }

        public bool IsNotEventMode => !IsEventMode;
        public bool Terminated { get; set; }
        public DataDragState DragState { get; }
        public JobQueue JobQueue { get; } = new ();
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

        private SelectMode _uiElementSelectMode;
        private KeyState _keyState = new ();
        private ZoomState _zoomState = new ();
        private Vector _mouseOnWindow = new (0, 0);
        private Vector _mouseOnMonitor = new(0, 0);
        private Vector _mouseOnCanvas = new(0, 0);
        private string _resourceSelectionStatus = string.Empty;
        private string _canvasSelectionStatus = string.Empty;
        private bool _isEventMode = false;
        private SgaPackage _selectedPackage = new ();
        private SgaImage _selectedImage = new ();
        private IKeyboardInputReceiver _focusedKeyboardInputReceiver;
        
        private SgaSpriteAbstract _selectedSprite = new SgaSprite();
        private SGUIGroupMaster _groupMaster;
    }

   
}

