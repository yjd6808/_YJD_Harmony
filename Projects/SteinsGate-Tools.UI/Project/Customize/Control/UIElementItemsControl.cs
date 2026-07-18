/*
 * 작성자: 윤정도
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using MoreLinq;
using SGToolsCommon.Customize.Style;
using SGToolsCommon.Extension;
using SGToolsCommon.Model;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.Extension;
using SGToolsUI.Model.Main;
using SGToolsUI.View;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Customize.Control
{
    // 쉬프트를 누르고 움직일 경우
    // 어떻게 움직일지
    public enum ShiftKeyMoving
    {
        None,
        Horizontal,
        Vertical
    }

    public class UIElementItemsControl : ItemsControl, INotifyPropertyChanged, IDataDragReceiver
    {
        public bool IsManipulationMode => manipulationMode_ != Positioning.Center;
        public MainViewModel ViewModel { get; private set; } = null!;
        public Canvas CanvasPanel => canvasPanel_;
        public ItemsPresenter Presenter => canvasPresenter_;

        private Canvas canvasPanel_ = null!;
        private ItemsPresenter canvasPresenter_ = null!;
        private bool isShiftMove_;
        private DragState dragState_;
        private ShiftKeyMoving shiftKeyMoving_ = ShiftKeyMoving.None;
        private IntPoint dragMoveStartPosition_;
        private List<MovingElement>? movingElements_;
        private SGUIElement? prevSelectElement_;  // 이전에 마우스 포인터를 찍었을때 선택한 엘리먼트

        // 매니퓰레이션관련
        private SGUIElement? manipulationTarget_;
        private Positioning manipulationMode_ = Positioning.Center;  // Center는 아무 상태도 아닐때를 말한다.
        private Positioning prevManipulationMode_;
        private IntSize manipulationStartTargetSize_;
        private IntPoint manipulationStartPosition_;
        private IntPoint manipulationStartTargetPosition_;
        public delegate bool ManipulatorMethod(IntPoint _pos, IntVector _move, out IntPoint _manipulatedPosition, out IntSize _manipulatedSize);
        private List<ManipulatorMethod> manipulators_ = new();

        private ContextMenu contextMenu_ = null!;
        private MenuItem attributeMenuItem_ = null!;
        private MenuItem deleteMenuItem_ = null!;
        private MenuItem restoreSizeMenuItem_ = null!;

        // ======================================================================
        //             초기화
        // ======================================================================

        //////////////////////////////////////////////////////////////////////////////////
        public UIElementItemsControl()
        {
            Loaded += OnLoaded;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnLoaded(object _sender, RoutedEventArgs _e)
        {
            InitializeViewModel();
            InitializePanel();
            InitializeContextMenu();
            InitializeManipulators();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void InitializeContextMenu()
        {
            if (DesignerProperties.GetIsInDesignMode(this))
                return;

            contextMenu_ = new ContextMenu();
            attributeMenuItem_ = new MenuItem();
            attributeMenuItem_.Style = (System.Windows.Style)Application.Current.FindResource(CustomStyleKey.MenuItemSparkKey);
            attributeMenuItem_.Header = "속성";
            attributeMenuItem_.Command = ViewModel.Commander.SelectPropertyGridElement;

            deleteMenuItem_ = new MenuItem();
            deleteMenuItem_.Style = (System.Windows.Style)Application.Current.FindResource(CustomStyleKey.MenuItemDeleteKey);
            deleteMenuItem_.Header = "삭제";
            deleteMenuItem_.Command = ViewModel.Commander.DeleteUIElement;

            // SGUISprite에만 적용될 메뉴
            restoreSizeMenuItem_ = new MenuItem();
            restoreSizeMenuItem_.Header = "크기 복구";
            restoreSizeMenuItem_.Command = new RestoreSizeCommand();

            contextMenu_.Items.Add(attributeMenuItem_);
            contextMenu_.Items.Add(deleteMenuItem_);

            canvasPanel_.ContextMenu = contextMenu_;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void InitializeViewModel()
        {
            ViewModel = (DataContext as MainViewModel)!;

            // 디자이너에서는 무조건 예외가 터지기땜에 막아줘야함.
            if (DesignerProperties.GetIsInDesignMode(this))
                return;

            if (ViewModel == null)
                throw new Exception("UIElementsControl에서 뷰모델 초기화 실패");
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
        private void InitializeManipulators()
        {
            manipulators_.Add(ManipulateTopLeft);
            manipulators_.Add(ManipulateTop);
            manipulators_.Add(ManipulateTopRight);
            manipulators_.Add(ManipulateLeft);
            manipulators_.Add(ManipulateCenter);  // 인덱스로 접근하기 위함
            manipulators_.Add(ManipulateRight);
            manipulators_.Add(ManipulateBottomLeft);
            manipulators_.Add(ManipulateBottom);
            manipulators_.Add(ManipulateBottomRight);
        }

        // ======================================================================
        //             터널링 이벤트
        // ======================================================================

        //////////////////////////////////////////////////////////////////////////////////
        protected override void OnMouseDown(MouseButtonEventArgs _e)
        {
            base.OnMouseDown(_e);
            IntPoint pos = _e.GetPosition(this).Zoom(ViewModel.ZoomState);
            MouseButton btn = _e.ChangedButton;

            OpenContextMenu(pos, btn);
            OnMouseDownEventMode(pos);
            OnMouseDownManipulation(pos);
            MoveBegin(pos);
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected override void OnPreviewMouseMove(MouseEventArgs _e)
        {
            base.OnPreviewMouseMove(_e);

            IntPoint pos = _e.GetPosition(this).Zoom(ViewModel.ZoomState);

            OnMouseMoveEventMode(pos);
            OnMouseMoveManipulation(_e);
            MoveMove(pos);
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected override void OnPreviewMouseUp(MouseButtonEventArgs _e)
        {
            base.OnPreviewMouseUp(_e);

            IntPoint pos = _e.GetPosition(this).Zoom(ViewModel.ZoomState);
            MouseButton btn = _e.ChangedButton;

            OnMouseUpEventMode(pos, btn);
            OnMouseUpManipulation();
            MoveEnd(pos);
        }

        // ======================================================================
        //             엘리먼트 선택/드래깅
        // ======================================================================

        //////////////////////////////////////////////////////////////////////////////////
        private void MoveBegin(IntPoint _pos)
        {
            bool alt = ViewModel.KeyState.IsAltPressed;
            bool ctrl = ViewModel.KeyState.IsCtrlPressed;
            bool space = ViewModel.KeyState.IsPressed(SGKey.Space);

            // 자유 드래그 모드 일때는 선택/드래그 되면 안됨
            if (space)
                return;

            // 매니퓰레이션 중일때는 선택/드래깅을 하지못하도록 한다.
            if (IsManipulationMode)
                return;

            // 컨텍스트 메뉴 열렸다가 캔버스를 누르고 닫을때 바닥찍으면 클릭으로 인식해버림, 한번 흘려주자.
            if (contextMenu_.IsOpen)
                return;

            // 이벤트 모드일때도 선택/드래그 금지
            if (ViewModel.IsEventMode)
                return;

            isShiftMove_ = ViewModel.KeyState.IsShiftPressed;
            dragMoveStartPosition_ = _pos;

            ObservableCollection<SGUIElement> pickedElements = ViewModel.RootGroup.PickedElements;
            IEnumerable<SGUIElement> pickedSelectedElements = pickedElements.Where(element => element.Selected);

            if (!alt && !ctrl && pickedElements.Count > 0 && pickedSelectedElements.LastOrDefault(element => element.ContainPoint(_pos)) != null)
            {
                ViewModel.View.CanvasShapesControl.IsDraggable = false;
                ViewModel.View.TitlePanel.Draggable = false;

                // 각 엘리먼트의 시작위치를 기록해놓는다.
                movingElements_ = ViewModel.RootGroup.PickedSelectedElements.Select(element => new MovingElement(element, element.VisualPosition)).ToList();
                dragState_ = DragState.Wait;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void MoveMove(IntPoint _pos)
        {
            if (dragState_ == DragState.None)
                return;

            if (movingElements_ == null)
                return;

            // 드래그 시작 후 마우스가 움직인 벡터만큼 다른 엘리먼트들도 벡터만큼 움직여준다.
            IntVector move = IntPoint.Subtract(dragMoveStartPosition_, _pos);

            if (dragState_ == DragState.Wait)
            {
                if (move.Length <= Constant.DragActivateDistance)
                    return;

                dragState_ = DragState.Dragging;
                return;
            }

            // dragState_ == Dragging
            if (isShiftMove_ && shiftKeyMoving_ == ShiftKeyMoving.None)
            {
                if (Math.Abs(move.X) > Math.Abs(move.Y))
                    shiftKeyMoving_ = ShiftKeyMoving.Horizontal;
                else
                    shiftKeyMoving_ = ShiftKeyMoving.Vertical;
                isShiftMove_ = false;
            }

            if (shiftKeyMoving_ == ShiftKeyMoving.Vertical)
                move.X = 0;
            else if (shiftKeyMoving_ == ShiftKeyMoving.Horizontal)
                move.Y = 0;

            movingElements_.ForEach(m => m.Element.VisualPosition = IntPoint.Subtract(m.StartPosition, move));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void MoveEnd(IntPoint _pos)
        {
            isShiftMove_ = false;
            shiftKeyMoving_ = ShiftKeyMoving.None;
            ViewModel.View.CanvasShapesControl.IsDraggable = true;
            ViewModel.View.TitlePanel.Draggable = true;
            movingElements_ = null;

            // 드래그 중이었다면 아래의 선택기능은 수행하지 않는다.
            if (dragState_ == DragState.Dragging)
            {
                dragState_ = DragState.None;
                return;
            }

            dragState_ = DragState.None;
            bool alt = ViewModel.KeyState.IsAltPressed;
            bool ctrl = ViewModel.KeyState.IsCtrlPressed;
            bool space = ViewModel.KeyState.IsPressed(SGKey.Space);

            // 자유 드래그 모드 일때는 선택/드래그 되면 안됨
            if (space)
                return;

            // 매니퓰레이션 중일때는 선택/드래깅을 하지못하도록 한다.
            if (IsManipulationMode)
                return;

            // 컨텍스트 메뉴 열렸다가 캔버스를 누르고 닫을때 바닥찍으면 클릭으로 인식해버림, 한번 흘려주자.
            if (contextMenu_.IsOpen)
                return;

            // 이벤트 모드일때도 선택/드래그 금지
            if (ViewModel.IsEventMode)
                return;

            ObservableCollection<SGUIElement> pickedElements = ViewModel.RootGroup.PickedElements;

            // 알트키를 눌린 경우 겹친 위치의 원소들을 순차적으로 선택할 수 있도록한다.
            if (alt)
            {
                IEnumerable<SGUIElement> candidates = pickedElements.Reversed()
                    .Where(element => element.CanvasSelectable && element.ContainPoint(dragMoveStartPosition_));

                IEnumerator<SGUIElement> enumerator = candidates.GetEnumerator();

                if (!candidates.Any())
                    return;

                bool find = false;
                SGUIElement? findElement = null;

                while (true)
                {
                    bool success = enumerator.MoveNext();

                    if (success == false && find == false)
                        break;

                    if (find)
                    {
                        findElement = success ?
                            enumerator.Current :
                            candidates.First();

                        break;
                    }

                    if (enumerator.Current == prevSelectElement_)
                        find = true;
                }

                // 아예 찾지를 못한 경우
                if (find == false)
                    findElement = candidates.First();

                if (findElement == prevSelectElement_)
                    findElement!.Selected = false;
                else
                {
                    prevSelectElement_ = findElement;
                    ViewModel.Commander.SelectUIElement.Execute(findElement);
                }
            }
            else
            {
                prevSelectElement_ = pickedElements.Reversed()
                    .FirstOrDefault(element => element.CanvasSelectable && element.ContainPoint(dragMoveStartPosition_));

                if (prevSelectElement_ != null)
                    ViewModel.Commander.SelectUIElement.Execute(prevSelectElement_);
            }
        }

        // ======================================================================
        //             이벤트 모드
        // ======================================================================

        //////////////////////////////////////////////////////////////////////////////////
        private bool TryGetPickedGroupEventMode(out SGUIGroup? _pickedGroup)
        {
            _pickedGroup = null;

            if (ViewModel.IsEventMode == false)
                return false;

            _pickedGroup = ViewModel.RootGroup.PickedGroup;
            if (_pickedGroup == null)
                return false;

            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnMouseDownEventMode(IntPoint _pos)
        {
            if (!TryGetPickedGroupEventMode(out SGUIGroup? pickedGroup))
                return;

            pickedGroup!.OnMouseDown(_pos);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnMouseMoveEventMode(IntPoint _pos)
        {
            if (!TryGetPickedGroupEventMode(out SGUIGroup? pickedGroup))
                return;

            pickedGroup!.OnMouseMove(_pos);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnMouseUpEventMode(IntPoint _pos, MouseButton _btn)
        {
            if (!TryGetPickedGroupEventMode(out SGUIGroup? pickedGroup))
                return;

            pickedGroup!.OnMouseUp(_pos);
        }

        // ======================================================================
        //             컨텍스트 메뉴
        // ======================================================================

        //////////////////////////////////////////////////////////////////////////////////
        private void OpenContextMenu(IntPoint _pos, MouseButton _btn)
        {
            if (_btn != MouseButton.Right)
                return;

            if (ViewModel.IsEventMode)
                return;

            ObservableCollection<SGUIElement> pickedElements = ViewModel.RootGroup.PickedElements;
            SGUIElement? lastSelectedElement = pickedElements.LastOrDefault(element => element.Selected && element.ContainPoint(_pos));

            if (lastSelectedElement == null)
                return;

            contextMenu_.Items.Remove(restoreSizeMenuItem_);

            if (lastSelectedElement is ISizeRestorable)
            {
                contextMenu_.Items.Add(restoreSizeMenuItem_);
                restoreSizeMenuItem_.CommandParameter = lastSelectedElement;
            }

            attributeMenuItem_.CommandParameter = lastSelectedElement;
            contextMenu_.IsOpen = true;
        }

        // ======================================================================
        //             엘리먼트 조작
        // ======================================================================

        //////////////////////////////////////////////////////////////////////////////////
        private Positioning CheckManipulatable(IntPoint _mousePos, out SGUIElement? _manipulationTarget)
        {
            _manipulationTarget = null;
            const double THICK_HALF = 2.0;
            const double THICK = 4.0;  // 그랩 손잡이 두께

            foreach (var element in ViewModel.RootGroup.PickedElements.Reversed())
            {
                if (!element.Selected) continue;
                if (!element.Manipulatable) continue;

                double x = element.VisualPosition.X;
                double y = element.VisualPosition.Y;
                double width = element.VisualSize.Width;
                double height = element.VisualSize.Height;
                _manipulationTarget = element;

                if (RectEx.FromContain(x + THICK_HALF, y - THICK_HALF, width - THICK, THICK, _mousePos))
                    return Positioning.Top;

                if (RectEx.FromContain(x + THICK_HALF, y + height - THICK_HALF, width - THICK, THICK, _mousePos))
                    return Positioning.Bottom;

                if (RectEx.FromContain(x + width - THICK_HALF, y + THICK_HALF, THICK, height - THICK, _mousePos))
                    return Positioning.Right;

                if (RectEx.FromContain(x - THICK_HALF, y + THICK_HALF, THICK, height - THICK, _mousePos))
                    return Positioning.Left;

                if (RectEx.FromContain(x - THICK_HALF, y - THICK_HALF, THICK, THICK, _mousePos))
                    return Positioning.TopLeft;

                if (RectEx.FromContain(x + width - THICK_HALF, y + height - THICK_HALF, THICK, THICK, _mousePos))
                    return Positioning.BottomRight;

                if (RectEx.FromContain(x + width - THICK_HALF, y - THICK_HALF, THICK, THICK, _mousePos))
                    return Positioning.TopRight;

                if (RectEx.FromContain(x - THICK_HALF, y + height - THICK_HALF, THICK, THICK, _mousePos))
                    return Positioning.BottomLeft;
            }

            return Positioning.Center;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private Cursor GetSizeCursor(Positioning _positioning)
        {
            switch (_positioning)
            {
                case Positioning.Center:
                    return Cursors.Arrow;
                case Positioning.Left:
                case Positioning.Right:
                    return Cursors.SizeWE;
                case Positioning.Bottom:
                case Positioning.Top:
                    return Cursors.SizeNS;
                case Positioning.BottomRight:
                case Positioning.TopLeft:
                    return Cursors.SizeNWSE;
                case Positioning.TopRight:
                case Positioning.BottomLeft:
                    return Cursors.SizeNESW;
            }

            return null!;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnMouseDownManipulation(IntPoint _pos)
        {
            manipulationMode_ = CheckManipulatable(_pos, out SGUIElement? manipulationTarget);

            if (manipulationMode_ == Positioning.Center)
                return;

            manipulationStartPosition_ = _pos;
            manipulationTarget_ = manipulationTarget;
            manipulationStartTargetSize_ = manipulationTarget!.VisualSize;
            manipulationStartTargetPosition_ = manipulationTarget!.VisualPosition;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void OnMouseMoveManipulation(MouseEventArgs _e)
        {
            if (ViewModel.DragState.State != DragState.None)
                return;

            if (ViewModel.View.CanvasShapesControl.IsHideSelection)
                return;

            Manipulate(_e);
            ManipulationCheck(_e);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void Manipulate(MouseEventArgs _e)
        {
            if (!IsManipulationMode || manipulationTarget_ == null)
                return;

            IntPoint pos = _e.GetPosition(this).Zoom(ViewModel.ZoomState);
            Vector move = IntPoint.Subtract(manipulationStartPosition_, pos);

            if (!manipulators_[(int)manipulationMode_](pos, move, out IntPoint manipulatedPosition, out IntSize manipulatedSize))
                return;

            manipulationTarget_.VisualPosition = manipulatedPosition;
            manipulationTarget_.VisualSize = manipulatedSize;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void ManipulationCheck(MouseEventArgs _e)
        {
            IntPoint pos = _e.GetPosition(this).Zoom(ViewModel.ZoomState);

            if (IsManipulationMode)
                return;

            Positioning positioning = CheckManipulatable(pos, out SGUIElement? _);

            if (positioning != prevManipulationMode_)
            {
                prevManipulationMode_ = positioning;
                Mouse.OverrideCursor = GetSizeCursor(positioning);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool ManipulateLeft(IntPoint _pos, IntVector _move, out IntPoint _manipulatedIntPoint, out IntSize _manipulatedSize)
        {
            _move.Y = 0;
            double rightEdgeAxis = manipulationStartTargetPosition_.X + manipulationStartTargetSize_.Width;

            _manipulatedIntPoint = manipulationStartTargetPosition_;
            _manipulatedSize = manipulationStartTargetSize_;

            if (_manipulatedIntPoint.X <= rightEdgeAxis)
                _manipulatedIntPoint.X = manipulationStartTargetPosition_.X - _move.X;

            if (manipulationStartTargetSize_.Width + _move.X < 5)
                return false;

            _manipulatedSize.Width = manipulationStartTargetSize_.Width + _move.X;
            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool ManipulateRight(IntPoint _pos, IntVector _move, out IntPoint _manipulatedIntPoint, out IntSize _manipulatedSize)
        {
            _move.Y = 0;
            _manipulatedIntPoint = manipulationStartTargetPosition_;
            _manipulatedSize = manipulationStartTargetSize_;

            if (manipulationStartTargetSize_.Width - _move.X < 5)
                return false;

            _manipulatedSize.Width = manipulationStartTargetSize_.Width - _move.X;
            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool ManipulateTop(IntPoint _pos, IntVector _move, out IntPoint _manipulatedIntPoint, out IntSize _manipulatedSize)
        {
            _move.X = 0;
            double bottomEdgeAxis = manipulationStartTargetPosition_.Y + manipulationStartTargetSize_.Height;

            _manipulatedIntPoint = manipulationStartTargetPosition_;
            _manipulatedSize = manipulationStartTargetSize_;

            if (_manipulatedIntPoint.Y <= bottomEdgeAxis - 5)
                _manipulatedIntPoint.Y = manipulationStartTargetPosition_.Y - _move.Y;

            if (manipulationStartTargetSize_.Height + _move.Y < 5)
                return false;

            _manipulatedSize.Height = manipulationStartTargetSize_.Height + _move.Y;
            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool ManipulateBottom(IntPoint _pos, IntVector _move, out IntPoint _manipulatedIntPoint, out IntSize _manipulatedSize)
        {
            _move.X = 0;
            _manipulatedIntPoint = manipulationStartTargetPosition_;
            _manipulatedSize = manipulationStartTargetSize_;

            if (manipulationStartTargetSize_.Height - _move.Y < 5)
                return false;

            _manipulatedSize.Height = manipulationStartTargetSize_.Height - _move.Y;
            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool ManipulateTopLeft(IntPoint _pos, IntVector _move, out IntPoint _manipulatedIntPoint, out IntSize _manipulatedSize)
        {
            _manipulatedIntPoint = new();
            _manipulatedSize = new();

            if (!ManipulateTop(_pos, _move, out IntPoint topIntPoint, out IntSize topSize))
                return false;

            if (!ManipulateLeft(_pos, _move, out IntPoint leftIntPoint, out IntSize leftSize))
                return false;

            _manipulatedIntPoint = new IntPoint(leftIntPoint.X, topIntPoint.Y);
            _manipulatedSize = new Size(leftSize.Width, topSize.Height);
            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool ManipulateTopRight(IntPoint _pos, IntVector _move, out IntPoint _manipulatedIntPoint, out IntSize _manipulatedSize)
        {
            _manipulatedIntPoint = new();
            _manipulatedSize = new();

            if (!ManipulateTop(_pos, _move, out IntPoint topIntPoint, out IntSize topSize))
                return false;

            if (!ManipulateRight(_pos, _move, out IntPoint rightIntPoint, out IntSize rightSize))
                return false;

            _manipulatedIntPoint = new IntPoint(rightIntPoint.X, topIntPoint.Y);
            _manipulatedSize = new Size(rightSize.Width, topSize.Height);
            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool ManipulateBottomLeft(IntPoint _pos, IntVector _move, out IntPoint _manipulatedIntPoint, out IntSize _manipulatedSize)
        {
            _manipulatedIntPoint = new();
            _manipulatedSize = new();

            if (!ManipulateBottom(_pos, _move, out IntPoint bottomIntPoint, out IntSize bottomSize))
                return false;

            if (!ManipulateLeft(_pos, _move, out IntPoint leftIntPoint, out IntSize leftSize))
                return false;

            _manipulatedIntPoint = new IntPoint(leftIntPoint.X, bottomIntPoint.Y);
            _manipulatedSize = new Size(leftSize.Width, bottomSize.Height);
            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool ManipulateBottomRight(IntPoint _pos, IntVector _move, out IntPoint _manipulatedIntPoint, out IntSize _manipulatedSize)
        {
            _manipulatedIntPoint = new();
            _manipulatedSize = new();

            if (!ManipulateBottom(_pos, _move, out IntPoint bottomIntPoint, out IntSize bottomSize))
                return false;

            if (!ManipulateRight(_pos, _move, out IntPoint rightIntPoint, out IntSize rightSize))
                return false;

            _manipulatedIntPoint = new IntPoint(rightIntPoint.X, bottomIntPoint.Y);
            _manipulatedSize = new Size(rightSize.Width, bottomSize.Height);
            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private bool ManipulateCenter(IntPoint _pos, IntVector _move, out IntPoint _manipulatedPosition, out IntSize _manipulatedSize)
        {
            throw new Exception("매니퓰레이트 불가능한 모드입니다.");
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void OnMouseUpManipulation()
        {
            Mouse.OverrideCursor = Cursors.Arrow;
            manipulationMode_ = Positioning.Center;
            manipulationTarget_ = null;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void DragEnd(IntPoint _p, object _data)
        {
            SgaSprite? sprite = _data as SgaSprite;

            if (sprite == null)
                return;

            IntPoint pos = Mouse.GetPosition(this).Zoom(ViewModel.ZoomState);
            ObservableCollection<SGUIElement> pickedElements = ViewModel.RootGroup.PickedElements;
            if (pickedElements.Count == 0)
            {
                MessageBoxEx.ShowTopMost("우선 그룹 또는 엘리먼트를 픽해주세요.");
                return;
            }

            // 놓은 지점에있는 그룹들중 가장 위에 그룹을 가져온다.
            SGUIGroup? topLevelGroup = pickedElements.Where(element => element.IsGroup && element.ContainPoint(pos)).Cast<SGUIGroup>().LastOrDefault();
            if (topLevelGroup == null)
            {
                MessageBoxEx.ShowTopMost("해당위치에 그룹이 없습니다.\n그룹내에 드랍해야합니다.");
                return;
            }

            CreateElementView elementView = new CreateElementView(topLevelGroup, sprite, pos);
            elementView.ShowDialog();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public bool ContainPoint(IntPoint _p)
            => VisualEx.ContainPoint(this, _p);

        public event PropertyChangedEventHandler? PropertyChanged;

        //////////////////////////////////////////////////////////////////////////////////
        protected virtual void OnPropertyChanged([CallerMemberName] string? _propertyName = null)
            => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(_propertyName));

        private struct MovingElement
        {
            public SGUIElement Element { get; }
            public IntPoint StartPosition { get; }

            //////////////////////////////////////////////////////////////////////////////////
            public MovingElement(SGUIElement _element, IntPoint _startPosition)
            {
                Element = _element;
                StartPosition = _startPosition;
            }
        }

        private class RestoreSizeCommand : ICommand
        {
#pragma warning disable CS0067
            public event EventHandler? CanExecuteChanged;
#pragma warning restore CS0067

            //////////////////////////////////////////////////////////////////////////////////
            public bool CanExecute(object? _parameter) => true;

            //////////////////////////////////////////////////////////////////////////////////
            public void Execute(object? _parameter)
                => (_parameter as ISizeRestorable)!.RestoreSize();
        }
    }
}