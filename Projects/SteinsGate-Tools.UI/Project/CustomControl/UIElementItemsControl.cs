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
            base.OnMouseLeftButtonDown(e);

            
        }

        /*
        private void HideSelectionRect()
        {
            _selectionRect.Visibility = Visibility.Hidden;
        }

        private void ShowSelectionRect()
        {
            _selectionRect.Visibility = Visibility.Visible;

            Children.Remove(_selectionRect);
            Children.Add(_selectionRect);
        }

        private void FitSelectionRect(Rect r)
        {
            _selectionRect.Width = r.Width;
            _selectionRect.Height = r.Height;

            SetLeft(_selectionRect, r.X);
            SetTop(_selectionRect, r.Y);
        }

        private void CustomCanvas_MouseDown(object sender, MouseButtonEventArgs e)
        {
            Point pos = e.GetPosition(this);
            bool find = false;

            foreach (UIElement g in Children)
            {
                if (g == _selectionRect)
                    continue;

                double x = GetLeft(g);
                double y = GetTop(g);

                var k1 = g.DesiredSize;

                Rect r = new Rect();
                r.X = x;
                r.Y = y;
                r.Width = k1.Width;
                r.Height = k1.Height;



                if (r.Contains(pos))
                {
                    if (_selectedElement == g)
                    {
                        _selectedElement = null;
                        HideSelectionRect();
                        continue;
                    }

                    _selectedElement = g;
                    FitSelectionRect(r);
                    ShowSelectionRect();
                    find = true;
                    break;
                }
            }

            if (find == false)
            {
                _selectedElement = null;
                HideSelectionRect();
            }
        }

        protected override Size MeasureOverride(Size constraint)
        {
            base.MeasureOverride(constraint);

            var size = new Size();

            foreach (var child in Children.OfType<FrameworkElement>())
            {
                var x = GetLeft(child) + child.Width;
                var y = GetTop(child) + child.Height;

                if (!double.IsNaN(x) && size.Width < x)
                {
                    size.Width = x;
                }

                if (!double.IsNaN(y) && size.Height < y)
                {
                    size.Height = y;
                }
            }

            return size;
        }
        */
        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
