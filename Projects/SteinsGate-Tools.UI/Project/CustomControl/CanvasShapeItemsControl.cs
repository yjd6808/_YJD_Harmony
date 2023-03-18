/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 3:10:12 PM
 *
 */

using SGToolsCommon.Extension;
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
using SGToolsCommon.Model;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.Extension;
using SGToolsUI.Model.Main;

namespace SGToolsUI.CustomControl
{


    public class CanvasShapeItemsControl : ItemsControl, INotifyPropertyChanged, IKeyboardInputReceiver
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

        public bool IsAnchorVisible
        {
            get => _isAnchorVisible;
            set
            {
                if (_isAnchorVisible == value)
                    return;

                _isAnchorVisible = value;

                if (value)
                    _canvasShapes.Add(_anchor);
                else
                    _canvasShapes.Remove(_anchor);


                OnPropertyChanged();
            }
        }

        public bool IsDraggable
        {
            get => _isDraggable;
            set
            {
                _isDraggable = value;
                DragEnd(null);
            }
        }

        public bool IsHideSelection
        {
            get => _isHideSelection;
            set
            {
                if (_isHideSelection == value)
                    return;

                _isHideSelection = value;
                double opacity = _isHideSelection ? 0 : 1;
                _selectionPool.ForEach(selection => selection.Selection.Opacity = opacity);
                _selectionMap.Values.ForEach(selection => selection.Selection.Opacity = opacity);
            }
        }

        // 헬퍼 프로퍼티.
        public ObservableCollection<SGUIElement> PickedElements => ViewModel.GroupMaster.PickedElements;


        public MainViewModel ViewModel { get; private set; }
        public CanvasRect Viewport => _viewPort;

        
        public CanvasGrid Grid => _grid;
        public Canvas CanvasPanel => _canvasPanel;
        public ItemsPresenter Presenter => _canvasPresenter;
        public DragState DragState => _dragState;
        public bool HasSelection => _canvasShapes.FirstOrDefault(shape => shape is CanvasSelection) != null;
        


        private ObservableCollection<CanvasShape> _canvasShapes = new();
        private LinkedList<CanvasSelection> _selectionPool = new();
        private Dictionary<SGUIElement, CanvasSelection> _selectionMap = new();
        private CanvasGrid _grid;
        private CanvasRect _viewPort;
        private CanvasAnchor _anchor;
        private Canvas _canvasPanel;
        private ItemsPresenter _canvasPresenter;
        private bool _isGridVisible;
        private bool _isViewportVisible;
        private bool _isAnchorVisible;

        private CanvasRect _dragBox;
        private DragState _dragState = DragState.None;
        private Point _dragStartPosition;
        private bool _isDraggable = true;
        private bool _isHideSelection = false;

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
            initializeAnchor();
            initializeSelectionPool();
        }

        

        private void InitializeViewPort() 
            => _viewPort = new CanvasRect(
                new Rect(0, 0, Constant.ResolutionWidth, Constant.ResolutionHeight), 
                1, Brushes.DodgerBlue
            );
        private void InitializeGrid() 
            => _grid = new CanvasGrid(100, 1, Brushes.White);

        private void initializeAnchor()
        {
            _anchor = new CanvasAnchor(
                new Rect(0, 0, Constant.CanvasAnchorSize, Constant.CanvasAnchorSize),
                2, Brushes.Black, Brushes.Orange
            );

            if (DesignerProperties.GetIsInDesignMode(this))
                return;

            _anchor.Target = ViewModel.GroupMaster;
        }
            

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
        //             버블링 이벤트
        //             MainView.xaml파일상 ZOrder를 더 높게 배치했기 때문에
        //             이때 겹쳐진 UIElementsItemsControl로 이벤트 전파가 안되기 땜에 강제로 이벤트 발생을 해줘야함
        // ======================================================================

        public void OnKeyDown(SGKey key)
        {
            if (key == SGKey.Z)
                IsHideSelection = true;

            if (!ViewModel.KeyState.IsModifierKeyPressed)
            {
                if (ViewModel.KeyState.IsPressed(SGKey.X))
                {
                    ViewModel.GroupMaster.DeselectAll();
                    ViewModel.IsEventMode = !ViewModel.IsEventMode;

                    if (ViewModel.IsEventMode == false)
                        ViewModel.GroupMaster.PickedElementsDisabled = false;
                }

                else if (ViewModel.KeyState.IsPressed(SGKey.C) && ViewModel.IsEventMode)
                {
                    ViewModel.GroupMaster.PickedElementsDisabled = !ViewModel.GroupMaster.PickedElementsDisabled;
                }
                else if (key == SGKey.Delete)
                {
                    if (ViewModel.GroupMaster.HasSelectedElement && MessageBoxEx.ShowTopMost("정말로 삭제하시겠습니까?", "질문임", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.Yes)
                        ViewModel.Commander.DeleteUIElement.Execute(null);
                }
                else if (key == SGKey.Left)
                {
                    ViewModel.GroupMaster
                        .PickedSelectedElements
                        .ForEach(ps => ps.VisualPosition = Point.Add(ps.VisualPosition, new Vector(Constant.CanvasElementWithKeyboardDeltaX * -1, 0)) );
                }
                else if (key == SGKey.Right)
                {
                    ViewModel.GroupMaster
                        .PickedSelectedElements
                        .ForEach(ps => ps.VisualPosition = Point.Add(ps.VisualPosition, new Vector(Constant.CanvasElementWithKeyboardDeltaX, 0)) );
                }
                else if (key == SGKey.Up)
                {
                    ViewModel.GroupMaster
                        .PickedSelectedElements
                        .ForEach(ps => ps.VisualPosition = Point.Add(ps.VisualPosition, new Vector(0, Constant.CanvasElementWithKeyboardDeltaY * -1)) );
                }
                else if (key == SGKey.Down)
                {
                    ViewModel.GroupMaster
                        .PickedSelectedElements
                        .ForEach(ps => ps.VisualPosition = Point.Add(ps.VisualPosition, new Vector(0, Constant.CanvasElementWithKeyboardDeltaY)) );
                }


            }

            if (ViewModel.KeyState.IsCtrlPressed)
            {
                switch (key)
                {
                case SGKey.X:
                    if (ViewModel.KeyState.IsCtrlPressed)
                        ViewModel.Commander.ClipboardOperateUIElement.Execute(ClipboardOperate.Cut);
                    break;
                case SGKey.C:
                    if (ViewModel.KeyState.IsCtrlPressed)
                        ViewModel.Commander.ClipboardOperateUIElement.Execute(ClipboardOperate.Copy);
                    break;
                case SGKey.V:
                    if (ViewModel.KeyState.IsCtrlPressed)
                        ViewModel.Commander.ClipboardOperateUIElement.Execute(ClipboardOperate.Paste);
                    break;
                }
            }
        }

        public void OnKeyUp(SGKey key)
        {
            if (key == SGKey.Z)
                IsHideSelection = false;
        }


        
        protected override void OnMouseDown(MouseButtonEventArgs e)
        {
            DragBegin(e);

            UIElementItemsControl source = ViewModel.View.UIElementsControl;
            source.RaiseEvent(new MouseButtonEventArgs(e.MouseDevice, e.Timestamp, e.ChangedButton)
            {
                RoutedEvent = e.RoutedEvent, 
                Source = source
            });
        }

        protected override void OnPreviewMouseMove(MouseEventArgs e)
        {
            DragMove(e);

            UIElementItemsControl source = ViewModel.View.UIElementsControl;
            source.RaiseEvent(new MouseEventArgs(e.MouseDevice, e.Timestamp, e.StylusDevice)
            {
                RoutedEvent = e.RoutedEvent,
                Source = source
            });
        }

        protected override void OnPreviewMouseUp(MouseButtonEventArgs e)
        {
            DragEnd(e);

            UIElementItemsControl source = ViewModel.View.UIElementsControl;
            source.RaiseEvent(new MouseButtonEventArgs(e.MouseDevice, e.Timestamp, e.ChangedButton)
            {
                RoutedEvent = e.RoutedEvent,
                Source = source
            });
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
                if (shape == _anchor) continue;

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
            bool hasAnchor = false;

            foreach (CanvasShape shape in _canvasShapes)
            {
                if (shape == _grid) hasGrid = true;
                else if (shape == _viewPort) hasViewPort = true;
                else if (shape == _dragBox) hasDragBox = true;
                else if (shape == _anchor) hasAnchor = true;
            }

            if (!hasViewPort && _isViewportVisible)
                _canvasShapes.Add(_viewPort);

            if (!hasGrid && _isGridVisible)
                _canvasShapes.Add(_grid);

            if (!hasDragBox && _dragBox != null)
                _canvasShapes.Add(_dragBox);

            if (!hasAnchor && _anchor != null)
                _canvasShapes.Add(_anchor);


            OnPropertyChanged(nameof(CanvasShapes));
        }


        public void AdjustAnchor(SGUIGroup group = null)
        {
            // 뷰가 초기화되기도 전에 호출하는 경우
            if (_anchor == null)
                return;

            if (group == null)
                _anchor.Target = ViewModel.GroupMaster;
            else
                _anchor.Target = group;
        }

        public void ArrangeSelection(SGUIElement element)
        {
            var canvasSelection = PopSelection();
            canvasSelection.Selection.Width = element.VisualSize.Width;
            canvasSelection.Selection.Height = element.VisualSize.Height;
            canvasSelection.Element = element;
            _canvasShapes.Add(canvasSelection);
            _selectionMap.Add(element, canvasSelection);

            Binding selectionWidthBinding = new Binding("VisualSize.Width");
            selectionWidthBinding.Source = element;
            selectionWidthBinding.Mode = BindingMode.OneWay;
            canvasSelection.Selection.SetBinding(ItemsControl.WidthProperty, selectionWidthBinding);

            Binding selectionHeightBinding = new Binding("VisualSize.Height");
            selectionHeightBinding.Source = element;
            selectionHeightBinding.Mode = BindingMode.OneWay;
            canvasSelection.Selection.SetBinding(ItemsControl.HeightProperty, selectionHeightBinding);


            System.Diagnostics.Debug.WriteLine($"셀렉션맵: {_selectionMap.Count} ★ 캔버스쉐이프 {_canvasShapes.Count}");
        }

        public void ReleaseSelection(SGUIElement element)
        {
            if (!_selectionMap.ContainsKey(element))
                return;

             CanvasSelection selection = _selectionMap[element];
             selection.Element = null;
            if (!_selectionMap.Remove(element))
                throw new Exception("셀렉션 맵에서 삭제 실패");

            if (!_canvasShapes.Remove(selection))
                throw new Exception("캔버스 쉐잎에서 셀렉션 삭제 실패");

            PushSelection(selection);
        }

        public void DragBegin(MouseButtonEventArgs e)
        {
            if (!_isDraggable)
                return;

            

            if (_dragState != DragState.None)
            {
                DragEnd(e);
                return;
            }

            Point pos = e.GetPosition(this).Zoom(ViewModel.ZoomState);
            _dragState = DragState.Wait;
            _dragStartPosition = pos;
            ViewModel.View.TitlePanel.Draggable = false;
        }

        public void DragMove(MouseEventArgs e)
        {
            if (ViewModel.View.UIElementsControl.IsManipulationMode)
            {
                DragEnd(e);
                return;
            }

            Point pos = e.GetPosition(this).Zoom(ViewModel.ZoomState);

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
            _dragState = DragState.None;
            ViewModel.View.TitlePanel.Draggable = true;

            if (e == null)
                return;

            if (_dragBox == null)
                return;

            Point pos = e.GetPosition(this).Zoom(ViewModel.ZoomState);

            if (pos.Distance(_dragStartPosition) < Constant.DragActivateDistance)
                return;

            IEnumerable<SGUIElement> selectedElements = PickedElements.Where(element => _dragBox.VisualRect.Contains(element.VisualRect));

            if (selectedElements.Any())
                ViewModel.Commander.SelectUIElement.Execute(selectedElements);

            _canvasShapes.Remove(_dragBox);
            _dragBox = null;
        }


        public event PropertyChangedEventHandler? PropertyChanged;
        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
            => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }
}
