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
        private SGUIRootGroup rootGroup_ = null!;
        private WorkspaceTreeItem? workspaceRoot_;
        private string? currentXmlFilePath_;

        //////////////////////////////////////////////////////////////////////////////////
        public MainViewModel()
        {
            PackManager = SgaManager.Instance;
            Setting = new Setting();
            Setting.Load();
            LogBox = new LogListBox() { MaxItemCount = 1500 };
            LogBox.Width = 400;
            LogBox.Height = 600;
            RootGroup = SGUIRootGroup.Create(this);
            LogErrorHandler = new Action<Exception>((ex) => LogBox.AddLog(ex));
            LogView = new LogView(LogBox);
            AlbumView = new AlbumView(this);
            BackupView = new BackupView(this);
            Commander = new MainCommandCenter(this);
            Commander.Execute(nameof(ReloadSgaPackage));
            DragState = new DataDragState();
            RecentDirectories = new ObservableCollection<string>();
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
        }

        //////////////////////////////////////////////////////////////////////////////////
        public async Task LoadRootGroupAsync(WorkspaceTreeItem _item)
        {
            if (_item.IsDirectory || _item.XmlFilePath == null)
                return;

            if (_item.IsLoaded && _item.RootGroup != null)
            {
                RootGroup = _item.RootGroup;
                currentXmlFilePath_ = _item.XmlFilePath;
                return;
            }

            try
            {
                var loader = new SGUILoader(this);
                SGUIRootGroup loaded = await loader.LoadAsync(_item.XmlFilePath);
                loaded.ViewModel = this;

                _item.RootGroup = loaded;
                _item.IsLoaded = true;
                _item.RootGroup.VisualName = _item.Name;

                RootGroup = loaded;
                currentXmlFilePath_ = _item.XmlFilePath;

                LogBox.AddLog($"UI 로드 완료: {_item.XmlFilePath}");
            }
            catch (Exception ex)
            {
                LogBox.AddLog($"UI 로드 실패: {_item.XmlFilePath}", ex.Message);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void SaveCurrentRootGroup()
        {
            if (currentXmlFilePath_ == null || RootGroup == null)
                return;

            try
            {
                var saver = new SGUISaver(this);
                saver.SaveAsync(currentXmlFilePath_, RootGroup).Wait();
                LogBox.AddLog($"UI 저장 완료: {currentXmlFilePath_}");
            }
            catch (Exception ex)
            {
                LogBox.AddLog($"UI 저장 실패", ex.Message);
            }
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

        public SGUIRootGroup RootGroup
        {
            get => rootGroup_;
            set
            {
                if (rootGroup_ != null)
                    rootGroup_.Clear();

                rootGroup_ = value;
                OnPropertyChanged();
            }
        }

        public WorkspaceTreeItem? WorkspaceRoot
        {
            get => workspaceRoot_;
            set
            {
                workspaceRoot_ = value;
                OnPropertyChanged();
            }
        }

        public string? CurrentXmlFilePath
        {
            get => currentXmlFilePath_;
            set
            {
                currentXmlFilePath_ = value;
                OnPropertyChanged();
            }
        }

        public ObservableCollection<string> RecentDirectories { get; }

        public SelectMode UIElementSelectMode
        {
            get => uiElementSelectMode_;
            set
            {
                uiElementSelectMode_ = value;
                OnPropertyChanged();
            }
        }

        public bool IsEventMode
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
        public List<IKeyboardInputReceiver> KeyboardInputReceivers = new();
        public readonly Action<Exception> LogErrorHandler;
    }
}
