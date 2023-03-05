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
using System.Drawing.Drawing2D;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Security.Policy;
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
using MoreLinq.Extensions;

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
        public DragState DragState => _dragState;



        private ObservableCollection<CanvasShape> _canvasShapes = new();
        private LinkedList<CanvasSelection> _selectionPool = new();
        private Dictionary<SGUIElement, CanvasSelection> _selectionMap = new();
        private CanvasGrid _grid;
        private CanvasRect _viewPort;
        private Canvas _canvasPanel;
        private ItemsPresenter _canvasPresenter;
        private bool _isGridVisible;
        private bool _isViewportVisible;

        private CanvasRect _dragBox;
        private double _draggedDistance;
        private DragState _dragState = DragState.None;
        private Point _dragStartPosition;

        public CanvasShapeItemsControl()
        {
            Loaded += OnLoaded;
        }


        // ======================================================================
        //             이니셜라지
        // ======================================================================

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            InitializeViewModel();
            InitializePanel();
            InitializeGrid();
            InitializeViewPort();
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




        private void initializeSelectionPool()
        {
            for (int i = 0; i < 200; ++i)
            {
                var canvasSelection = new CanvasSelection(new ItemsControl());
                canvasSelection.Selection.Style = (Style)Application.Current.FindResource("Selection");
                _selectionPool.AddLast(canvasSelection);
            }
        }
        // ======================================================================
        //             이벤트
        // ======================================================================

        protected override void OnKeyDown(KeyEventArgs e)
        {
            base.OnKeyDown(e);

            if (e.Key == Key.LeftShift || e.Key == Key.RightShift)
                ViewModel.UIElementSelectMode = SelectMode.KeepExcept;
        }

        protected override void OnKeyUp(KeyEventArgs e)
        {
            base.OnKeyDown(e);
            if (e.Key == Key.LeftShift || e.Key == Key.RightShift)
                ViewModel.UIElementSelectMode = SelectMode.New;

        }

        protected override void OnMouseDown(MouseButtonEventArgs e)
        {
            DragBegin(e);
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            DragMove(e);
        }

        protected override void OnMouseUp(MouseButtonEventArgs e)
        {
            DragEnd(e);
        }



        

        // ======================================================================
        //             기능
        // ======================================================================
        public void ReleaseAllSelection()
        {
            _selectionMap.Clear();

            ObservableCollection<CanvasShape> newShapes = new (new List<CanvasShape>(_canvasShapes.Count + 10));

            foreach (CanvasShape shape in _canvasShapes)
            {
                if (shape == _grid) continue;
                if (shape == _viewPort) continue;
                if (shape == _dragBox) continue;

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
                var canvasSelection = new CanvasSelection(new ItemsControl());
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
            bool hasDragBox = false;

            foreach (CanvasShape shape in _canvasShapes)
            {
                if (shape == _grid) hasGrid = true;
                if (shape == _viewPort) hasViewPort = true;
                if (shape == _dragBox) hasDragBox = true;
            }

            if (!hasViewPort && _isViewportVisible)
                _canvasShapes.Add(_viewPort);

            if (!hasGrid && _isGridVisible)
                _canvasShapes.Add(_grid);

            if (_dragBox != null)
                _canvasShapes.Add(_dragBox);


            OnPropertyChanged(nameof(CanvasShapes));
        }

       

        public void ArrangeSelection(SGUIElement element)
        {
            var canvasSelection = PopSelection();
            canvasSelection.Selection.Width = element.VisualSize.Width;
            canvasSelection.Selection.Height = element.VisualSize.Height;
            canvasSelection.Selection.Margin = new Thickness(element.VisualPosition.X, element.VisualPosition.Y, 0, 0);
            _canvasShapes.Add(canvasSelection);
            _selectionMap.Add(element, canvasSelection);
        }

        public void ReleaseSelection(SGUIElement element)
        {
            if (!_selectionMap.ContainsKey(element))
                return;

             CanvasSelection selection = _selectionMap[element];
             _selectionMap.Remove(element);
             _canvasShapes.Remove(selection);
             PushSelection(selection);
        }

        public void DragBegin(MouseButtonEventArgs e)
        {
            if (_dragState != DragState.None)
            {
                DragEnd(e);
                return;
            }

            Point pos = e.GetPosition(this);
            _dragState = DragState.Wait;
            _dragStartPosition = pos;
            ViewModel.View.TitlePanel.Draggable = false;
        }

        public void DragMove(MouseEventArgs e)
        {
            Point pos = e.GetPosition(this);

            if (_dragState == DragState.Wait)
            {
                double distance = pos.Distance(_dragStartPosition);
                if (distance < Constant.DragActivateDistance)
                    return;

                Rect dragBoxRect = new Rect(pos, _dragStartPosition);
                _dragBox = new CanvasRect(dragBoxRect, 2, Brushes.Black);
                _dragState = DragState.Dragging;
                _canvasShapes.Add(_dragBox);
            }
            else if (_dragState == DragState.Dragging)
                _dragBox.VisualRect = new Rect(pos, _dragStartPosition);
        }

        public void DragEnd(MouseEventArgs e)
        {
            ViewModel.View.TitlePanel.Draggable = true;

            if (_dragState != DragState.Dragging)
                return;

            ObservableCollection<SGUIElement> canvasElements = ViewModel.View.UIElementsControl.Elements;
            IEnumerable<SGUIElement> draggedElements = canvasElements.Where(element => _dragBox.VisualRect.Contains(element.VisualRect));
            ViewModel.Commander.SelectUIElement.Execute(draggedElements);
            _dragState = DragState.None;
            _canvasShapes.Remove(_dragBox);
            _dragBox = null;
        }


        public event PropertyChangedEventHandler? PropertyChanged;
        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
            => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }
}
