/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 3:10:12 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;

using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;
using SGToolsCommon.Extension;
using SGToolsCommon.Model;
using SGToolsCommon.Primitive;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.Extension;
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Customize.Control
{
    public class CanvasShapeItemsControl : ItemsControl, INotifyPropertyChanged, IKeyboardInputReceiver
    {
        public ObservableCollection<CanvasShape> CanvasShapes
        {
            get => canvasShapes_;
            set
            {
                canvasShapes_ = value;
                OnPropertyChanged();
            }
        }

        public bool IsViewportVisible
        {
            get => isViewportVisible_;
            set
            {
                if (isViewportVisible_ == value)
                    return;

                isViewportVisible_ = value;

                if (value)
                    canvasShapes_.Add(viewPort_);
                else
                    canvasShapes_.Remove(viewPort_);

                OnPropertyChanged();
            }
        }

        public bool IsGridVisible
        {
            get => isGridVisible_;
            set
            {
                if (isGridVisible_ == value)
                    return;

                isGridVisible_ = value;

                if (value)
                    canvasShapes_.Add(grid_);
                else
                    canvasShapes_.Remove(grid_);

                OnPropertyChanged();
            }
        }

        public bool IsAnchorVisible
        {
            get => isAnchorVisible_;
            set
            {
                if (isAnchorVisible_ == value)
                    return;

                isAnchorVisible_ = value;

                if (value)
                    canvasShapes_.Add(anchor_);
                else
                    canvasShapes_.Remove(anchor_);

                OnPropertyChanged();
            }
        }

        public bool IsDraggable
        {
            get => isDraggable_;
            set
            {
                isDraggable_ = value;
                DragEnd(null!);
            }
        }

        public bool IsHideSelection
        {
            get => isHideSelection_;
            set
            {
                if (isHideSelection_ == value)
                    return;

                isHideSelection_ = value;
                double opacity = isHideSelection_ ? 0 : 1;
                selectionPool_.ForEach(selection => selection.Selection.Opacity = opacity);
                selectionMap_.Values.ForEach(selection => selection.Selection.Opacity = opacity);
            }
        }

        public bool IsHideStatic
        {
            get => isHideStatic_;
            set
            {
                if (isHideStatic_ == value)
                    return;

                isHideStatic_ = value;

                ViewModel.RootGroup.PickedElements.ForEach(element =>
                {
                    if (element is SGUIStatic staticElement)
                        staticElement.IsVisible = !value;
                });
            }
        }

        // 헬퍼 프로퍼티.
        public ObservableCollection<SGUIElement> PickedElements => ViewModel.RootGroup.PickedElements;

        public MainViewModel ViewModel { get; private set; } = null!;
        public CanvasRect Viewport => viewPort_;
        public CanvasGrid Grid => grid_;
        public Canvas CanvasPanel => canvasPanel_;
        public ItemsPresenter Presenter => canvasPresenter_;
        public DragState DragState => dragState_;
        public bool HasSelection => canvasShapes_.FirstOrDefault(shape => shape is CanvasSelection) != null;

        private ObservableCollection<CanvasShape> canvasShapes_ = new();
        private LinkedList<CanvasSelection> selectionPool_ = new();
        private Dictionary<SGUIElement, CanvasSelection> selectionMap_ = new();
        private CanvasGrid grid_ = null!;
        private CanvasRect viewPort_ = null!;
        private CanvasAnchor anchor_ = null!;
        private Canvas canvasPanel_ = null!;
        private ItemsPresenter canvasPresenter_ = null!;
        private bool isGridVisible_;
        private bool isViewportVisible_;
        private bool isAnchorVisible_;

        private CanvasRect? dragBox_;
        private DragState dragState_ = DragState.None;
        private IntPoint dragStartPosition_;
        private bool isDraggable_ = true;
        private bool isHideSelection_ = false;
        private bool isHideStatic_ = false;

        //////////////////////////////////////////////////////////////////////////////////
        public CanvasShapeItemsControl()
        {
            Loaded += OnLoaded;
        }

        // ======================================================================
        //             이니셜라이즈
        // ======================================================================

        //////////////////////////////////////////////////////////////////////////////////
        private void OnLoaded(object _sender, RoutedEventArgs _e)
        {
            InitializeViewModel();
            InitializePanel();
            InitializeGrid();
            InitializeViewPort();
            InitializeAnchor();
            InitializeSelectionPool();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void InitializeViewPort()
            => viewPort_ = new CanvasRect(
                new Rect(0, 0, Constant.ResolutionWidth, Constant.ResolutionHeight),
                1, Brushes.DodgerBlue
            );

        //////////////////////////////////////////////////////////////////////////////////
        private void InitializeGrid()
            => grid_ = new CanvasGrid(100, 1, Brushes.White);

        //////////////////////////////////////////////////////////////////////////////////
        private void InitializeAnchor()
        {
            anchor_ = new CanvasAnchor(
                new Rect(0, 0, Constant.CanvasAnchorSize, Constant.CanvasAnchorSize),
                2, Brushes.Black, Brushes.Orange
            );

            if (DesignerProperties.GetIsInDesignMode(this))
                return;

            anchor_.Target = ViewModel.RootGroup;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void InitializeViewModel()
        {
            ViewModel = (DataContext as MainViewModel)!;

            // 디자이너에서는 무조건 예외가 터지기땜에 막아줘야함.
            if (DesignerProperties.GetIsInDesignMode(this))
                return;

            if (ViewModel == null)
                throw new Exception("CanvasShapesControl에서 뷰모델 초기화 실패");
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void InitializePanel()
        {
            canvasPresenter_ = this.FindChild<ItemsPresenter>()!;
            canvasPanel_ = canvasPresenter_.FindChild<Canvas>()!;

            if (canvasPresenter_ == null)
                throw new Exception("캔버스 프레젠터를 찾지 못했슴당.");

            if (canvasPanel_ == null)
                throw new Exception("캔버스 패널을 찾지 못했슴당.");
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void InitializeSelectionPool()
        {
            for (int i = 0; i < 200; ++i)
            {
                var canvasSelection = new CanvasSelection(new ItemsControl());
                canvasSelection.Selection.Style = (System.Windows.Style)Application.Current.FindResource("Selection");
                selectionPool_.AddLast(canvasSelection);
            }
        }

        // ======================================================================
        //             버블링 이벤트
        //             MainView.xaml파일상 ZOrder를 더 높게 배치했기 때문에
        //             이때 겹쳐진 UIElementsItemsControl로 이벤트 전파가 안되기 땜에 강제로 이벤트 발생을 해줘야함
        // ======================================================================

        //////////////////////////////////////////////////////////////////////////////////
        public void OnKeyDown(SGKey _key)
        {
            if (!ViewModel.KeyState.IsModifierKeyPressed)
            {
                if (_key == SGKey.Z)
                    IsHideSelection = !IsHideSelection;
                else if (_key == SGKey.S)
                    IsHideStatic = !IsHideStatic;
                else if (ViewModel.KeyState.IsPressed(SGKey.X))
                {
                    ViewModel.RootGroup.DeselectAll();
                    ViewModel.IsEventMode = !ViewModel.IsEventMode;

                    if (ViewModel.IsEventMode == false)
                        ViewModel.RootGroup.PickedElementsDisabled = false;
                }
                else if (ViewModel.KeyState.IsPressed(SGKey.C) && ViewModel.IsEventMode)
                {
                    ViewModel.RootGroup.PickedElementsDisabled = !ViewModel.RootGroup.PickedElementsDisabled;
                }
                else if (_key == SGKey.Delete)
                {
                    if (ViewModel.RootGroup.HasSelectedElement && MessageBoxEx.ShowTopMost("정말로 삭제하시겠습니까?", "질문임", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.Yes)
                        ViewModel.Commander.DeleteUIElement.Execute(null);
                }
                else if (_key == SGKey.Left)
                {
                    ViewModel.RootGroup
                        .PickedSelectedElements
                        .ForEach(ps => ps.VisualPosition = IntPoint.Add(ps.VisualPosition, new IntPoint(Constant.CanvasElementWithKeyboardDeltaX * -1, 0)));
                }
                else if (_key == SGKey.Right)
                {
                    ViewModel.RootGroup
                        .PickedSelectedElements
                        .ForEach(ps => ps.VisualPosition = IntPoint.Add(ps.VisualPosition, new IntPoint(Constant.CanvasElementWithKeyboardDeltaX, 0)));
                }
                else if (_key == SGKey.Up)
                {
                    ViewModel.RootGroup
                        .PickedSelectedElements
                        .ForEach(ps => ps.VisualPosition = IntPoint.Add(ps.VisualPosition, new IntPoint(0, Constant.CanvasElementWithKeyboardDeltaY * -1)));
                }
                else if (_key == SGKey.Down)
                {
                    ViewModel.RootGroup
                        .PickedSelectedElements
                        .ForEach(ps => ps.VisualPosition = IntPoint.Add(ps.VisualPosition, new IntPoint(0, Constant.CanvasElementWithKeyboardDeltaY)));
                }
            }

            if (ViewModel.KeyState.IsCtrlPressed)
            {
                switch (_key)
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

        //////////////////////////////////////////////////////////////////////////////////
        public void OnKeyUp(SGKey _key)
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void OnLostFocus()
        {
            this.FocusClear();
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected override void OnMouseDown(MouseButtonEventArgs _e)
        {
            DragBegin(_e);

            UIElementItemsControl source = ViewModel.View.UIElementsControl;
            source.RaiseEvent(new MouseButtonEventArgs(_e.MouseDevice, _e.Timestamp, _e.ChangedButton)
            {
                RoutedEvent = _e.RoutedEvent,
                Source = source
            });
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected override void OnPreviewMouseMove(MouseEventArgs _e)
        {
            DragMove(_e);

            UIElementItemsControl source = ViewModel.View.UIElementsControl;
            source.RaiseEvent(new MouseEventArgs(_e.MouseDevice, _e.Timestamp, _e.StylusDevice)
            {
                RoutedEvent = _e.RoutedEvent,
                Source = source
            });
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected override void OnPreviewMouseUp(MouseButtonEventArgs _e)
        {
            DragEnd(_e);

            UIElementItemsControl source = ViewModel.View.UIElementsControl;
            source.RaiseEvent(new MouseButtonEventArgs(_e.MouseDevice, _e.Timestamp, _e.ChangedButton)
            {
                RoutedEvent = _e.RoutedEvent,
                Source = source
            });
        }

        // ======================================================================
        //             기능
        // ======================================================================

        //////////////////////////////////////////////////////////////////////////////////
        public void ReleaseAllSelection()
        {
            selectionMap_.Clear();

            ObservableCollection<CanvasShape> newShapes = new(new List<CanvasShape>(canvasShapes_.Count + 10));

            foreach (CanvasShape shape in canvasShapes_)
            {
                if (shape == grid_) continue;
                if (shape == viewPort_) continue;
                if (shape == dragBox_) continue;
                if (shape == anchor_) continue;

                if (!shape.IsSelection)
                {
                    newShapes.Add(shape);
                    continue;
                }

                PushSelection((shape as CanvasSelection)!);
            }

            ExchangeShapes(newShapes);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public CanvasSelection PopSelection()
        {
            if (selectionPool_.Count == 0)
            {
                var canvasSelection = new CanvasSelection(new ItemsControl());
                canvasSelection.Selection.Style = (System.Windows.Style)Application.Current.FindResource("Selection");
                return canvasSelection;
            }

            CanvasSelection popped = selectionPool_.First!.Value;
            selectionPool_.RemoveFirst();
            return popped;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void PushSelection(CanvasSelection _selection)
            => selectionPool_.AddLast(_selection);

        //////////////////////////////////////////////////////////////////////////////////
        private void ExchangeShapes(IEnumerable<CanvasShape>? _shapes)
        {
            // shape null 전달시 빈 컬렉션 만듬
            canvasShapes_ = new ObservableCollection<CanvasShape>(_shapes ?? new List<CanvasShape>(100));

            // 이전 컬렉션에서 뷰포트나 그리드 포함되어 있을 경우 중복포함 방지
            bool hasViewPort = false;
            bool hasGrid = false;
            bool hasDragBox = false;
            bool hasAnchor = false;

            foreach (CanvasShape shape in canvasShapes_)
            {
                if (shape == grid_) hasGrid = true;
                else if (shape == viewPort_) hasViewPort = true;
                else if (shape == dragBox_) hasDragBox = true;
                else if (shape == anchor_) hasAnchor = true;
            }

            if (!hasViewPort && isViewportVisible_)
                canvasShapes_.Add(viewPort_);

            if (!hasGrid && isGridVisible_)
                canvasShapes_.Add(grid_);

            if (!hasDragBox && dragBox_ != null)
                canvasShapes_.Add(dragBox_);

            if (!hasAnchor && anchor_ != null)
                canvasShapes_.Add(anchor_);

            OnPropertyChanged(nameof(CanvasShapes));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void AdjustAnchor(SGUIGroup? _group = null)
        {
            // 뷰가 초기화되기도 전에 호출하는 경우
            if (anchor_ == null)
                return;

            if (_group == null)
                anchor_.Target = ViewModel.RootGroup;
            else
                anchor_.Target = _group;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void ArrangeSelection(SGUIElement _element)
        {
            var canvasSelection = PopSelection();
            canvasSelection.Selection.Width = _element.VisualSize.Width;
            canvasSelection.Selection.Height = _element.VisualSize.Height;
            canvasSelection.Element = _element;

            if (selectionMap_.ContainsKey(_element))
            {
                Debug.Assert(false, "이미 해당 엘리먼트에 셀렉션이 할당되어있는데 다시 셀렉션이 할당이 시도되고 있습니다.");
                PushSelection(canvasSelection);
                return;
            }

            selectionMap_.Add(_element, canvasSelection);
            canvasShapes_.Add(canvasSelection);

            Binding selectionWidthBinding = new Binding("VisualSize.Width");
            selectionWidthBinding.Source = _element;
            selectionWidthBinding.Mode = BindingMode.OneWay;
            canvasSelection.Selection.SetBinding(ItemsControl.WidthProperty, selectionWidthBinding);

            Binding selectionHeightBinding = new Binding("VisualSize.Height");
            selectionHeightBinding.Source = _element;
            selectionHeightBinding.Mode = BindingMode.OneWay;
            canvasSelection.Selection.SetBinding(ItemsControl.HeightProperty, selectionHeightBinding);

            Debug.WriteLine($"셀렉션맵: {selectionMap_.Count} ★ 캔버스쉐이프 {canvasShapes_.Count}");
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void ReleaseSelection(SGUIElement _element)
        {
            if (!selectionMap_.ContainsKey(_element))
                return;

            CanvasSelection selection = selectionMap_[_element];
            selection.Element = null;

            if (!selectionMap_.Remove(_element))
                throw new Exception("셀렉션 맵에서 삭제 실패");

            if (!canvasShapes_.Remove(selection))
                throw new Exception("캔버스 쉐잎에서 셀렉션 삭제 실패");

            PushSelection(selection);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void DragBegin(MouseButtonEventArgs _e)
        {
            if (!isDraggable_)
                return;

            if (dragState_ != DragState.None)
            {
                DragEnd(_e);
                return;
            }

            IntPoint pos = _e.GetPosition(this).Zoom(ViewModel.ZoomState);
            dragState_ = DragState.Wait;
            dragStartPosition_ = pos;
            ViewModel.View.TitlePanel.Draggable = false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void DragMove(MouseEventArgs _e)
        {
            if (ViewModel.View.UIElementsControl.IsManipulationMode)
            {
                DragEnd(_e);
                return;
            }

            IntPoint pos = _e.GetPosition(this).Zoom(ViewModel.ZoomState);

            if (dragState_ == DragState.Wait)
            {
                double distance = pos.Distance(dragStartPosition_);
                if (distance < Constant.DragActivateDistance)
                    return;

                Rect dragBoxRect = new Rect(pos, dragStartPosition_);
                dragBox_ = new CanvasRect(dragBoxRect, 2, Brushes.Black);
                dragState_ = DragState.Dragging;
                canvasShapes_.Add(dragBox_);
            }
            else if (dragState_ == DragState.Dragging)
                dragBox_!.VisualRect = new Rect(pos, dragStartPosition_);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void DragEnd(MouseEventArgs? _e)
        {
            dragState_ = DragState.None;
            ViewModel.View.TitlePanel.Draggable = true;

            if (_e == null)
                return;

            if (dragBox_ == null)
                return;

            IntPoint pos = _e.GetPosition(this).Zoom(ViewModel.ZoomState);

            if (pos.Distance(dragStartPosition_) < Constant.DragActivateDistance)
                return;

            IEnumerable<SGUIElement> selectedElements = PickedElements.Where(element => dragBox_.VisualRect.Contains(element.VisualRect));

            if (selectedElements.Any())
                ViewModel.Commander.SelectUIElement.Execute(selectedElements);

            canvasShapes_.Remove(dragBox_);
            dragBox_ = null;
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        //////////////////////////////////////////////////////////////////////////////////
        protected virtual void OnPropertyChanged([CallerMemberName] string? _propertyName = null)
            => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(_propertyName));
    }
}
