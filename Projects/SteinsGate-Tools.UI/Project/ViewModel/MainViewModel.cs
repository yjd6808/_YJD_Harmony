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
            Commander.Execute(nameof(ReloadPackage));

            GroupMaster.Children.Add(new SGUIGroup()
            {
                Name = "그룹 1" ,
                Children = new ObservableCollection<SGUIElement>()
                {
                    new SGUIButton() { Name = "버튼 1-1"},
                    new SGUIButton() { Name = "버튼 1-2"},
                    new SGUIButton() { Name = "버튼 1-3"},
                    new SGUIButton() { Name = "버튼 1-4"},
                    new SGUIGroup()
                    {
                        Name = "그룹 1-5",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { Name = "버튼 1-5-1"},
                            new SGUIButton() { Name = "버튼 1-5-2"},
                            new SGUIButton() { Name = "버튼 1-5-3"},
                            new SGUIButton() { Name = "버튼 1-5-4"},
                            new SGUIGroup()
                            {
                                Name = "그룹 1-5-5"
                            }
                        }
                    },
                    new SGUIGroup()
                    {
                        Name = "그룹 1-6",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { Name = "버튼 1-6-1"},
                            new SGUIButton() { Name = "버튼 1-6-2"},
                            new SGUIButton() { Name = "버튼 1-6-3"},
                            new SGUIButton() { Name = "버튼 1-6-4"},
                            new SGUIGroup()
                            {
                                Name = "그룹 1-6-5"
                            }
                        }
                    }
                }
                
            });
            GroupMaster.Children.Add(new SGUIGroup()
            {
                Name = "그룹 2",
                Children = new ObservableCollection<SGUIElement>()
                {
                    new SGUIButton() { Name = "버튼 2-1"},
                    new SGUIButton() { Name = "버튼 2-2"},
                    new SGUIButton() { Name = "버튼 2-3"},
                    new SGUIButton() { Name = "버튼 2-4"},
                    new SGUIGroup()
                    {
                        Name = "그룹 2-5",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { Name = "버튼 2-5-1"},
                            new SGUIButton() { Name = "버튼 2-5-2"},
                            new SGUIButton() { Name = "버튼 2-5-3"},
                            new SGUIButton() { Name = "버튼 2-5-4"},
                            new SGUIGroup()
                            {
                                Name = "그룹 2-5-5"
                            }
                        }
                    },
                    new SGUIGroup()
                    {
                        Name = "그룹 2-6",
                        Children = new ObservableCollection<SGUIElement>()
                        {
                            new SGUIButton() { Name = "버튼 2-6-1"},
                            new SGUIButton() { Name = "버튼 2-6-2"},
                            new SGUIButton() { Name = "버튼 2-6-3"},
                            new SGUIButton() { Name = "버튼 2-6-4"},
                            new SGUIGroup()
                            {
                                Name = "그룹 2-6-5"
                            }
                        }
                    }
                }
            });
            GroupMaster.Children.Add(new SGUIGroup() { Name = "그룹 3" });
            GroupMaster.Children.Add(new SGUIGroup() { Name = "그룹 4" });
        }

        public MainView View { get; set; }
        public Setting Setting { get; set; }
        public MainCommandCenter Commander { get; }
        public SgaManager PackManager { get; }

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

        private Vector _mouseOnWindow = new (0, 0);
        private Vector _mouseOnMonitor = new(0, 0);
        private Vector _mouseOnCanvas = new(0, 0);
        private string _resourceSelectionStatus = string.Empty;
        private string _canvasSelectionStatus = string.Empty;
        private SgaPackage _selectedPackage = new ();
        private SgaImage _selectedImage = new ();
        private SGUIGroupMaster _groupMaster = new()
        {
            Name = "group_master",
            VisualRect = new Rect(0, 0, Constant.CanvasWidth, Constant.CanvasHeight),
            VisualName = "그룹 마스터",
            Code = 0,
            ElementType = SGUIElementType.Group,
            Selected = false,
            VerticalAlignment = VerticalAlignment.Top,
            HorizontalAlignment = HorizontalAlignment.Left,
        };
    }
}

