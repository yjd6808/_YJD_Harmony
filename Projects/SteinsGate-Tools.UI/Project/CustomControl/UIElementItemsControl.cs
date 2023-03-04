using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Shapes;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Security.RightsManagement;
using MoreLinq;
using SGToolsCommon.Extension;
using SGToolsUI.Model;
using SGToolsUI.View;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;
using SGToolsUI.ViewModel;

namespace SGToolsUI.CustomControl
{
    public class UIElementItemsControl : ItemsControl, INotifyPropertyChanged
    {
        public ObservableCollection<SGUIElement> UIElements
        {
            get => _uiElements;
            set
            {
                _uiElements = value;
                OnPropertyChanged();
            }
        }

        public MainViewModel ViewModel { get; private set; }
        public Canvas CanvasPanel => _canvasPanel;
        public ItemsPresenter Presenter => _canvasPresenter;

        private ObservableCollection<SGUIElement> _uiElements;
        private Canvas _canvasPanel;
        private ItemsPresenter _canvasPresenter;
        
        public UIElementItemsControl()
        {
            Loaded += OnLoaded;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            InitializeViewModel();
            InitializePanel();
        }

        private void InitializeViewModel()
        {
            ViewModel = DataContext as MainViewModel;

            // 디자이너에서는 무조건 예외가 터지기땜에 막아줘야함.
            if (DesignerProperties.GetIsInDesignMode(this))
                return;

            if (ViewModel == null)
                throw new Exception("UIElementsControl에서 뷰모델 초기화 실패");
        }

        private void InitializePanel()
        {
            _canvasPresenter = this.FindChild<ItemsPresenter>();
            _canvasPanel = _canvasPresenter.FindChild<Canvas>();

            if (_canvasPresenter == null)
                throw new Exception("캔버스 프레젠터를 찾지 못했슴당.");

            if (_canvasPanel == null)
                throw new Exception("캔버스 패널을 찾지 못했슴당.");
        }


        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
        {


            
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
