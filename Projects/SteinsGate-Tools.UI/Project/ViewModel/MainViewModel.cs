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
using MoreLinq.Extensions;
using SGToolsCommon;
using SGToolsCommon.CustomControl;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using SGToolsUI.Command;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.Command.MainViewCommand.Async;
using SGToolsUI.FileSystem;
using SGToolsUI.Model;
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
            LogErrorHandler = new Action<Exception>((ex) => LogBox.AddLog(ex));
            LogView = new LogView(LogBox);
            AlbumView = new AlbumView(this);
            Loader = new SGUILoader(this);
            Saver = new SGUISaver(this);
            Exporter = new SGUIExporter(this);
            Commander = new MainCommandCenter(this);
            Commander.Execute(nameof(ReloadSgaPackage));
            DragState = new DataDragState(this);
        }

        public void Loaded()
        {
            LogBox.Style = (Style)Application.Current.FindResource("LogListBox");
            
            if (Setting.ShowLogViewWhenProgramLaunched)
                LogView.Show();

            if (Setting.ShowLogViewWhenProgramLaunched && Setting.LogViewPositionWhenProgramLaunched != PointEx.Zero)
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
            }
        }

        public DataDragState DragState { get; }
        public JobQueue JobQueue { get; } = new ();
        public LogListBox LogBox { get; }
        public LogView LogView { get; }
        public AlbumView AlbumView { get; }
        public SGUILoader Loader { get; }
        public SGUISaver Saver { get; }
        public SGUIExporter Exporter { get; }
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
        private SgaSpriteAbstract _selectedSprite = new SgaSprite();
        private SGUIGroupMaster _groupMaster;
    }
}

