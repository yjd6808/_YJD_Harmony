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
using SGToolsCommon;
using SGToolsCommon.CustomControl;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using SGToolsUI.Command;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.File;
using SGToolsUI.Model;
using SGToolsUI.View;

namespace SGToolsUI.ViewModel
{
    public class MainViewModel : Bindable
    {
        public MainViewModel()
        {
            SGUIGroupMaster master = SGUIGroupMaster.Create(this);

            
            Setting = new Setting();
            Setting.Load();
            LogBox = new LogListBox() { MaxItemCount = 1500 };
            LogBox.Width = 400;
            LogBox.Height = 600;
            LogView = new LogView(LogBox);
            Loader = new SGUILoader(this);
            Saver = new SGUISaver(this);
            Exporter = new SGUIExporter(this);
            GroupMaster = master;
            Loader.Load(master);
            PackManager = SgaManager.Instance;
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

        public DataDragState DragState { get; }
        public JobQueue JobQueue { get; } = new ();
        public LogListBox LogBox { get; }
        public LogView LogView { get; }
        public SGUILoader Loader { get; }
        public SGUISaver Saver { get; }
        public SGUIExporter Exporter { get; }

        private SelectMode _uiElementSelectMode;
        private KeyState _keyState = new ();
        private ZoomState _zoomState = new ();
        private Vector _mouseOnWindow = new (0, 0);
        private Vector _mouseOnMonitor = new(0, 0);
        private Vector _mouseOnCanvas = new(0, 0);
        private string _resourceSelectionStatus = string.Empty;
        private string _canvasSelectionStatus = string.Empty;
        private SgaPackage _selectedPackage = new ();
        private SgaImage _selectedImage = new ();
        private SgaSpriteAbstract _selectedSprite = new SgaSprite();
        private SGUIGroupMaster _groupMaster;
    }
}

