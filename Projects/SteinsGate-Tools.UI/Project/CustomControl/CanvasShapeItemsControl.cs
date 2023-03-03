/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 3:10:12 PM
 *
 */

using SGToolsCommon.Extension;
using SGToolsUI.Model;
using SGToolsUI.ViewModel;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
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
using MoreLinq;
using SGToolsUI.Command.MainViewCommand;

namespace SGToolsUI.CustomControl
{
    public class CanvasShapeItemsControl : ItemsControl, INotifyPropertyChanged
    {
        public ObservableCollection<CanvasShape> CanvasShapes
        {
            get => _canvasShapes;
            set
            {
                _canvasShapes = value;
                OnPropertyChanged();
            }
        }

        public bool IsViewportVisible
        {
            get => _isViewportVisible;
            set
            {
                if (_isViewportVisible == value)
                    return;

                _isViewportVisible = value;

                if (value)
                    _canvasShapes.Add(_viewPort);
                else
                    _canvasShapes.Remove(_viewPort);


                OnPropertyChanged();
            }
        }

        public bool IsGridVisible
        {
            get => _isGridVisible;
            set
            {
                if (_isGridVisible == value)
                    return;

                _isGridVisible = value;

                if (value)
                    _canvasShapes.Add(_grid);
                else
                    _canvasShapes.Remove(_grid);


                OnPropertyChanged();
            }
        }

        public MainViewModel ViewModel { get; private set; }
        public CanvasRect Viewport => _viewPort;
        public CanvasGrid Grid => _grid;
        public Canvas CanvasPanel => _canvasPanel;
        public ItemsPresenter Presenter => _canvasPresenter;

        private ObservableCollection<CanvasShape> _canvasShapes = new();
        private LinkedList<CanvasSelection> _selectionPool = new();
        private Dictionary<SGUIElement, CanvasSelection> _selectionMap = new();
        private CanvasGrid _grid;
        private CanvasRect _viewPort;
        private CanvasRect _dragBox;
        private Canvas _canvasPanel;
        private ItemsPresenter _canvasPresenter;

        private bool _isGridVisible;
        private bool _isViewportVisible;

        public CanvasShapeItemsControl()
        {
            Loaded += OnLoaded;
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            base.OnKeyDown(e);

            if (e.Key == Key.LeftShift || e.Key == Key.RightShift)
                ViewModel.UIElementSelectMode = SelectMode.Keep;
        }

        protected override void OnKeyUp(KeyEventArgs e)
        {
            base.OnKeyDown(e);

            if (e.Key == Key.LeftShift || e.Key == Key.RightShift)
                ViewModel.UIElementSelectMode = SelectMode.New;

        }

        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
        {
            HitTestResult hitTestResult = VisualTreeHelper.HitTest(this, e.GetPosition(this));
            if (hitTestResult == null) 
                return;

            var element = hitTestResult.VisualHit;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            InitializeViewModel();
            InitializePanel();
            InitializeGrid();
            InitializeViewPort();
            InitializeDragBox();
            initializeSelectionPool();
        }

        private void InitializeViewPort() => _viewPort = new CanvasRect(new Rect(0, 0, Constant.ResolutionWidth, Constant.ResolutionHeight), 1, Brushes.DodgerBlue);
        private void InitializeGrid() => _grid = new CanvasGrid(100, 1, Brushes.White);

        private void InitializeViewModel()
        {
            ViewModel = DataContext as MainViewModel;

            // 디자이너에서는 무조건 예외가 터지기땜에 막아줘야함.
            if (DesignerProperties.GetIsInDesignMode(this))
                return;

            if (ViewModel == null)
                throw new Exception("CanvasShapesControl에서 뷰모델 초기화 실패");
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


        private void InitializeDragBox()
        {
            //_dragBox = new CanvasRect();
        }


        private void initializeSelectionPool()
        {
            for (int i = 0; i < 200; ++i)
            {
                var canvasSelection = new CanvasSelection();
                canvasSelection.Selection = new ItemsControl();
                canvasSelection.Selection.Style = (Style)Application.Current.FindResource("Selection");
                _selectionPool.AddLast(canvasSelection);
            }
        }


        public void ReleaseAllSelection()
        {
            _selectionMap.Clear();

            ObservableCollection<CanvasShape> newShapes = new (new List<CanvasShape>(_canvasShapes.Count + 10));

            foreach (CanvasShape shape in _canvasShapes)
            {
                if (shape == _grid) continue;
                if (shape == _viewPort) continue;

                if (!shape.IsSelection)
                {
                    newShapes.Add(shape);
                    continue;
                }

                PushSelection(shape as CanvasSelection);
            }

            ExchangeShapes(newShapes);
        }

        public CanvasSelection PopSelection()
        {
            if (_selectionPool.Count == 0)
            {
                var canvasSelection = new CanvasSelection();
                canvasSelection.Selection = new ItemsControl();
                canvasSelection.Selection.Style = (Style)Application.Current.FindResource("Selection");
                return canvasSelection;
            }

            CanvasSelection popped = _selectionPool.First.Value;
            _selectionPool.RemoveFirst();
            return popped;
        }

        public void PushSelection(CanvasSelection selection)
            => _selectionPool.AddLast(selection);

        private void ExchangeShapes(IEnumerable<CanvasShape>? shapes)
        {
            // shape null 전달시 빈 컬렉션 만듬
            _canvasShapes = new ObservableCollection<CanvasShape>(shapes ?? new List<CanvasShape>(100));

            // 이전 컬렉션에서 뷰포트나 그리드 포함되어 있을 경우 중복포함 방지
            // 무지성으로 범용성있게 쉐이프목록 교체할 수 있도록 하기 위함
            bool hasViewPort = false;
            bool hasGrid = false;

            foreach (CanvasShape shape in _canvasShapes)
            {
                if (shape == _grid) hasGrid = true;
                if (shape == _viewPort) hasViewPort = true;
            }

            if (!hasViewPort && _isViewportVisible)
                _canvasShapes.Add(_viewPort);

            if (!hasGrid && _isGridVisible)
                _canvasShapes.Add(_grid);

            OnPropertyChanged(nameof(CanvasShapes));
        }

       

        public void ArrangeSelection(SGUIElement element)
        {
            var canvasSelection = PopSelection();
            canvasSelection.Selection.Width = element.VisualSize.Width;
            canvasSelection.Selection.Height = element.VisualSize.Height;
            Canvas.SetLeft(canvasSelection.Selection, element.VisualPosition.X);
            Canvas.SetTop(canvasSelection.Selection, element.VisualPosition.Y);
            _canvasShapes.Add(canvasSelection);
            _selectionMap.Add(element, canvasSelection);
        }

        public void ReleaseSelection(SGUIElement element)
        {
            if (!_selectionMap.ContainsKey(element))
                throw new Exception($"해당 엘리먼트는 선택된 상태가 아닌데 Selection해제를 시도했습니다. 상태:{element.Selected}");

             CanvasSelection selection = _selectionMap[element];
             _selectionMap.Remove(element);
             PushSelection(selection);
        }


        public event PropertyChangedEventHandler? PropertyChanged;
        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
            => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }
}
