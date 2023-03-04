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
using SGToolsCommon.Sga;
using SGToolsUI.Command;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.Model;
using SGToolsUI.View;

namespace SGToolsUI.ViewModel
{
    public class MainViewModel : Bindable
    {
        public MainViewModel()
        {
            Setting = new Setting();
            Setting.Load();
            PackManager = SgaManager.Instance;
            Commander = new MainCommandCenter(this);
            Commander.Execute(nameof(ReloadSgaPackage));
            GroupMaster = new SGUIGroupMaster(this)
            {
                Name = "group_master",
                VisualRect = new Rect(0, 0, Constant.CanvasWidth, Constant.CanvasHeight),
                VisualName = "그룹 마스터",
                Code = 0,
                Selected = false,
                VerticalAlignment = VerticalAlignment.Top,
                HorizontalAlignment = HorizontalAlignment.Left,
            };
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
        private SGUIGroupMaster _groupMaster;
    }
}

