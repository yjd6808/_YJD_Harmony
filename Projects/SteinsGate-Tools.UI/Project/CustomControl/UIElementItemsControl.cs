/*
 * 작성자: 윤정도
 */

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
using System.Reflection.Metadata.Ecma335;
using System.Runtime.CompilerServices;
using System.Security.RightsManagement;
using System.Windows.Data;
using MoreLinq;
using SGToolsCommon.Extension;
using SGToolsUI.View;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;
using SGToolsUI.ViewModel;
using System.Xml.Linq;
using Accessibility;
using MoreLinq.Extensions;
using SGToolsCommon.CustomStyle;
using SGToolsCommon.Model;
using SGToolsCommon.Primitive;
using SGToolsUI.Extension;
using SGToolsUI.Command.MainViewCommand;
using Xceed.Wpf.AvalonDock.Controls;
using SGToolsCommon.Sga;
using SGToolsUI.Model.Main;

namespace SGToolsUI.CustomControl
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
        public bool IsManipulationMode => _manipulationMode != Positioning.Center;
        public MainViewModel ViewModel { get; private set; }
        public Canvas CanvasPanel => _canvasPanel;
        public ItemsPresenter Presenter => _canvasPresenter;

        
        private Canvas _canvasPanel;
        private ItemsPresenter _canvasPresenter;
        private bool _isShiftMove;
        private DragState _dragState;
        private ShiftKeyMoving _shiftKeyMoving = ShiftKeyMoving.None;
        private Point _dragMoveStartPosition;
        private List<MovingElement> _movingElements;
        private SGUIElement _prevSelectElement;                     // 이전에 마우스 포인터를 찍었을때 선택한 엘리먼트

        // 매니퓰레이션관련
        private SGUIElement _manipulationTarget;
        private Positioning _manipulationMode = Positioning.Center; // Center는 아무 상태도 아닐때를 말한다.
        private Positioning _prevManipulationMode;
        private Size _manipulationStartTargetSize;
        private Point _manipulationStartPosition;
        private Point _manipulationStartTargetPosition;
        public delegate bool ManipulatorMethod(Point pos, Vector move, out Point manipulatedPosition, out Size manipulatedSize);
        private List<ManipulatorMethod> _manipulators = new ();


        private ContextMenu _contextMenu;
        private MenuItem _attributeMenuItem;
        private MenuItem _deleteMenuItem;
        private MenuItem _restoreSizeMenuItem;

        // ======================================================================
        //             초기화
        // ======================================================================
        public UIElementItemsControl()
        {
            Loaded += OnLoaded;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            InitializeViewModel();
            InitializePanel();
            InitializeContextMenu();
            InitializeManipulators();
        }

        

        private void InitializeContextMenu()
        {
            if (DesignerProperties.GetIsInDesignMode(this))
                return;

            _contextMenu = new ContextMenu();
            _attributeMenuItem = new MenuItem();
            _attributeMenuItem.Style = (Style)Application.Current.FindResource(CustomStyleKey.MenuItemSparkKey);
            _attributeMenuItem.Header = "속성";
            _attributeMenuItem.Command = ViewModel.Commander.SelectPropertyGridElement;

            _deleteMenuItem = new MenuItem();
            _deleteMenuItem.Style = (Style)Application.Current.FindResource(CustomStyleKey.MenuItemDeleteKey);
            _deleteMenuItem.Header = "삭제";
            _deleteMenuItem.Command = ViewModel.Commander.DeleteUIElement;

            // SGUISprite에만 적용될 메뉴
            _restoreSizeMenuItem = new MenuItem();
            _restoreSizeMenuItem.Header = "크기 복구";
            _restoreSizeMenuItem.Command = new RestoreSizeCommand();


            _contextMenu.Items.Add(_attributeMenuItem);
            _contextMenu.Items.Add(_deleteMenuItem);

            _canvasPanel.ContextMenu = _contextMenu;
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

        private void InitializeManipulators()
        {
            _manipulators.Add(ManipulateTopLeft);
            _manipulators.Add(ManipulateTop);
            _manipulators.Add(ManipulateTopRight);
            _manipulators.Add(ManipulateLeft);
            _manipulators.Add(ManipulateCenter);                  // 인덱스로 접근하기 위함
            _manipulators.Add(ManipulateRight);
            _manipulators.Add(ManipulateBottomLeft);
            _manipulators.Add(ManipulateBottom);
            _manipulators.Add(ManipulateBottomRight);
        }

        


        // ======================================================================
        //             터널링 이벤트
        // ======================================================================

        protected override void OnMouseDown(MouseButtonEventArgs e)
        {
            base.OnMouseDown(e);
            Point pos = e.GetPosition(this).Zoom(ViewModel.ZoomState);
            MouseButton btn = e.ChangedButton;

            OpenContextMenu(pos, btn);
            OnMouseDownEventMode(pos);
            OnMouseDownManipulation(pos);
            MoveBegin(pos);
        }

        


        protected override void OnPreviewMouseMove(MouseEventArgs e)
        {
            base.OnPreviewMouseMove(e);

            Point pos = e.GetPosition(this).Zoom(ViewModel.ZoomState);

            OnMouseMoveEventMode(pos);
            OnMouseMoveManipulation(e);
            MoveMove(pos);
        }

   


        protected override void OnPreviewMouseUp(MouseButtonEventArgs e)
        {
            base.OnPreviewMouseUp(e);

            Point pos = e.GetPosition(this).Zoom(ViewModel.ZoomState);
            MouseButton btn = e.ChangedButton;

            OnMouseUpEventMode(pos, btn);
            OnMouseUpManipulation();
            MoveEnd(pos);
        }


        // ======================================================================
        //             엘리먼트 선택/드래깅
        // ======================================================================


        private void MoveBegin(Point pos)
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
            if (_contextMenu.IsOpen)
                return;

            // 이벤트 모드일때도 선택/드래그 금지
            if (ViewModel.IsEventMode)
                return;

            _isShiftMove = ViewModel.KeyState.IsShiftPressed;
            _dragMoveStartPosition = pos;

            ObservableCollection<SGUIElement> pickedElements = ViewModel.GroupMaster.PickedElements;
            IEnumerable<SGUIElement> pickedSelectedElements = pickedElements.Where(element => element.Selected);


            // 마우스를 클릭한 지점에 선택된 원소가 있는 경우 마우스를 따라 움직일 수 있도록 한다.
            // if (!alt && !ctrl && pickedElements.Count > 0 && pickedSelectedElements.LastOrDefault(element => element.ContainPoint(pos)) != null)
            if (!alt && !ctrl && pickedElements.Count > 0 && pickedSelectedElements.LastOrDefault(element => element.ContainPoint(pos)) != null)
            {
                ViewModel.View.CanvasShapesControl.IsDraggable = false;
                ViewModel.View.TitlePanel.Draggable = false;

                // 각 엘리먼트의 시작위치를 기록해놓는다.
                _movingElements = ViewModel.GroupMaster.PickedSelectedElements.Select(element => new MovingElement(element, element.VisualPosition)).ToList();
                _dragState = DragState.Wait;
            }
            
        }

        public void MoveMove(Point pos)
        {
            if (_dragState == DragState.None)
                return;

            if (_movingElements == null)
                return;

            // 드래그 시작 후 마우스가 움직인 벡터만큼 다른 엘리먼트들도 벡터만큼 움직여준다.
            Vector move = Point.Subtract(_dragMoveStartPosition, pos);

            if (_dragState == DragState.Wait)
            {
                if (move.Length <= Constant.DragActivateDistance)
                    return;

                _dragState = DragState.Dragging;
                return;
            }

            // dragState == Dragging
            if (_isShiftMove && _shiftKeyMoving == ShiftKeyMoving.None)
            {
                if (Math.Abs(move.X) > Math.Abs(move.Y))
                    _shiftKeyMoving = ShiftKeyMoving.Horizontal;
                else
                    _shiftKeyMoving = ShiftKeyMoving.Vertical;
                _isShiftMove = false;
            }

            if (_shiftKeyMoving == ShiftKeyMoving.Vertical)
                move.X = 0;
            else if (_shiftKeyMoving == ShiftKeyMoving.Horizontal)
                move.Y = 0;

            _movingElements.ForEach(m => m.Element.VisualPosition = Point.Subtract(m.StartPosition, move));
        }

        public void MoveEnd(Point p)
        {
            _isShiftMove = false;
            _shiftKeyMoving = ShiftKeyMoving.None; 
            ViewModel.View.CanvasShapesControl.IsDraggable = true;
            ViewModel.View.TitlePanel.Draggable = true;
            _movingElements = null;

            // 드래그 중이었다면 아래의 선택기능은 수행하지 않는다.
            if (_dragState == DragState.Dragging) {
                _dragState = DragState.None;
                return; 
            }

            _dragState = DragState.None;
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
            if (_contextMenu.IsOpen)
                return;

            // 이벤트 모드일때도 선택/드래그 금지
            if (ViewModel.IsEventMode)
                return;

            

            ObservableCollection<SGUIElement> pickedElements = ViewModel.GroupMaster.PickedElements;

            // 알트키를 눌린 경우 겹친 위치의 원소들을 순차적으로 선택할 수 있도록한다.
            if (alt)
            {
                // 픽된 원소들중 마우스를 클릭한 지점에 겹쳐져있는 모든 엘리먼트를 가져온다.
                // 이때 깊은 자식부터 가져와야하므로 뒤짚어줘야함 (역방향 반복자를 구현했기때문에 효율 굳)
                IEnumerable<SGUIElement> candidates = pickedElements.Reversed()
                    .Where(element => element.CanvasSelectable && element.ContainPoint(_dragMoveStartPosition));

                IEnumerator<SGUIElement> enumerator = candidates.GetEnumerator();

                if (!candidates.Any())
                    return;

                bool find = false;
                SGUIElement findElement = null;

                // 알트키를 누른 경우 이전에 선택한 엘리먼트 다음 원소를 가져온다.
                while (true)
                {
                    bool success = enumerator.MoveNext();

                    if (success == false && find == false)
                        break;

                    if (find)
                    {
                        findElement = success ?
                            enumerator.Current :
                            candidates.First(); // 마지막원소를 찾은 경우, 처음 원소를 가져온다.

                        break;
                    }


                    if (enumerator.Current == _prevSelectElement)
                        find = true;
                }

                // 아예 찾지를 못한 경우
                if (find == false)
                    findElement = candidates.First();

                if (findElement == _prevSelectElement)
                    findElement.Selected = false;
                else
                {
                    _prevSelectElement = findElement;
                    ViewModel.Commander.SelectUIElement.Execute(findElement);
                }



            }
            else
            {
                _prevSelectElement = pickedElements.Reversed()
                    .FirstOrDefault(element => element.CanvasSelectable && element.ContainPoint(_dragMoveStartPosition));

                if (_prevSelectElement != null)
                    ViewModel.Commander.SelectUIElement.Execute(_prevSelectElement);
            }
        }

        // ======================================================================
        //             이벤트 모드
        // ======================================================================

        private bool TryGetPickedGroupEventMode(out SGUIGroup pickedGroup)
        {
            pickedGroup = null;

            if (ViewModel.IsEventMode == false)
                return false;

            pickedGroup = ViewModel.GroupMaster.PickedGroup;
            if (pickedGroup == null)
                return false;

            return true;
        }

        private void OnMouseDownEventMode(Point pos)
        {
            if (!TryGetPickedGroupEventMode(out SGUIGroup pickedGroup))
                return;

            pickedGroup.OnMouseDown(pos);
        }

        private void OnMouseMoveEventMode(Point pos)
        {
            if (!TryGetPickedGroupEventMode(out SGUIGroup pickedGroup))
                return;

            pickedGroup.OnMouseMove(pos);
        }

        private void OnMouseUpEventMode(Point pos, MouseButton btn)
        {
            if (!TryGetPickedGroupEventMode(out SGUIGroup pickedGroup))
                return;

            pickedGroup.OnMouseUp(pos);
        }


        // ======================================================================
        //             컨텍스트 메뉴
        // ======================================================================
        private void OpenContextMenu(Point pos, MouseButton btn)
        {
            if (btn != MouseButton.Right)
                return;

            if (ViewModel.IsEventMode)
                return;

            ObservableCollection<SGUIElement> pickedElements = ViewModel.GroupMaster.PickedElements;
            SGUIElement lastSelectedElement = pickedElements.LastOrDefault(element => element.Selected && element.ContainPoint(pos));

            if (lastSelectedElement == null)
                return;

            _contextMenu.Items.Remove(_restoreSizeMenuItem);

            if (lastSelectedElement is ISizeRestorable)
            {
                _contextMenu.Items.Add(_restoreSizeMenuItem);
                _restoreSizeMenuItem.CommandParameter = lastSelectedElement;
            }

            _attributeMenuItem.CommandParameter = lastSelectedElement;
            _contextMenu.IsOpen = true;



        }



        // ======================================================================
        //             엘리먼트 조작
        // ======================================================================


        /*
         * 주작은 어떻게 할 것인가?
         *
         * 1. 선택된 엘리먼트의 가장자리를 지나갈때 조작가능 커서 모양으로 바뀌어야한다.
         * 2. "그랩"이 가능하다는걸 표시하기 위해 가상의 사각형이 8개 필요하다. => (Corner)모서리 4개, (Edge)변 4개
         *    이 사각형에 커서가 포함되면 커서 모양을 알맞게 변경한다.
         *    좌우 변: Cursors.SizeWE →             
         *    상하 변: Cursors.SizeNS ↓
         *    우상좌하 모서리: Cursors.SizeNESW ↘
         *    좌상우하 모서리: Cursors.SizeNWSE ↙
         * 3. 모서리, 변에 있는 가상의 사각형에 커서가 포함된 상태에서 해당 위치에서 마우스를 누른 경우
         *    ManipulationMode에 진입하도록 한다.
         *    이때는 선택된 엘리먼트라도 드래깅, 셀렉션을 하지 못하도록 막는다.
         *    마우스를 때면 다시 가능토록 한다.
         */

        Positioning CheckManipulatable(Point mousePos, out SGUIElement manipulationTarget)
        {
            manipulationTarget = null;
            const double thickHalf = 2.0;
            const double thick = 4.0;           // 그랩 손잡이 두께

            foreach (var element in ViewModel.GroupMaster.PickedElements.Reversed())    // 앞에 있는 녀석부터 검사
            {
                if (!element.Selected) continue;
                if (!element.Manipulatable) continue;

                double x = element.VisualPosition.X;
                double y = element.VisualPosition.Y;
                double width = element.VisualSize.Width;
                double height = element.VisualSize.Height;
                manipulationTarget = element;

                // ↑ 방향의 변 가상 사각형
                if (RectEx.FromContain(x + thickHalf, y - thickHalf, width - thick, thick, mousePos))
                    return Positioning.Top;

                // ↓ 방향의 변 가상 사각형
                if (RectEx.FromContain(x + thickHalf, y + height - thickHalf, width - thick, thick, mousePos))
                    return Positioning.Bottom;

                // → 방향의 변 가상 사각형
                if (RectEx.FromContain(x + width - thickHalf, y + thickHalf, thick, height - thick, mousePos))
                    return Positioning.Right;

                // ← 방향의 변 가상 사각형
                if (RectEx.FromContain(x - thickHalf, y + thickHalf, thick, height - thick, mousePos))
                    return Positioning.Left;

                // ↖ 방향의 모서리 가상 사각형
                if (RectEx.FromContain(x - thickHalf, y - thickHalf, thick, thick, mousePos))
                    return Positioning.TopLeft;

                // ↘ 방향의 모서리 가상 사각형
                if (RectEx.FromContain(x + width - thickHalf, y + height - thickHalf, thick, thick, mousePos))
                    return Positioning.BottomRight;

                // ↗ 방향의 모서리 가상 사각형
                if (RectEx.FromContain(x + width - thickHalf, y - thickHalf, thick, thick, mousePos))
                    return Positioning.TopRight;

                // ↙ 방향의 모서리 가상 사각형
                if (RectEx.FromContain(x - thickHalf, y + height - thickHalf, thick, thick, mousePos))
                    return Positioning.BottomLeft;
            }


            return Positioning.Center;
        }

        Cursor GetSizeCursor(Positioning positioning)
        {
            switch (positioning)
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

            return null;
        }


        private void OnMouseDownManipulation(Point pos)
        {
            _manipulationMode = CheckManipulatable(pos, out SGUIElement manipulationTarget);

            if (_manipulationMode == Positioning.Center)
                return;

            _manipulationStartPosition = pos;
            _manipulationTarget = manipulationTarget;
            _manipulationStartTargetSize = manipulationTarget.VisualSize;
            _manipulationStartTargetPosition = manipulationTarget.VisualPosition;
        }


        public void OnMouseMoveManipulation(MouseEventArgs e)
        {
            if (ViewModel.DragState.State != DragState.None)
                return;

            Manipulate(e);
            ManipulationCheck(e);
        }

        private void Manipulate(MouseEventArgs e)
        {
            if (!IsManipulationMode || _manipulationTarget == null)
                return;

            Point pos = e.GetPosition(this).Zoom(ViewModel.ZoomState);
            Vector move = Point.Subtract(_manipulationStartPosition, pos);

            if (!_manipulators[(int)_manipulationMode](pos, move, out Point manipulatedPosition, out Size manipulatedSize))
                return;

            _manipulationTarget.VisualPosition = manipulatedPosition;
            _manipulationTarget.VisualSize = manipulatedSize;
        }

        private void ManipulationCheck(MouseEventArgs e)
        {
            Point pos = e.GetPosition(this).Zoom(ViewModel.ZoomState);

            if (IsManipulationMode)
                return;

            Positioning positioning = CheckManipulatable(pos, out SGUIElement _);

            if (positioning != _prevManipulationMode)
            {
                _prevManipulationMode = positioning;
                Mouse.OverrideCursor = GetSizeCursor(positioning);
            }
        }

        private bool ManipulateLeft(Point pos, Vector move, out Point manipulatedPoint, out Size manipulatedSize)
        {
            move.Y = 0; // 좌우 이동은 Y축 델타를 0으로만든다.
            double rightEdgeAxis = _manipulationStartTargetPosition.X + _manipulationStartTargetSize.Width;

            manipulatedPoint = _manipulationStartTargetPosition;
            manipulatedSize = _manipulationStartTargetSize;

            if (manipulatedPoint.X <= rightEdgeAxis)
                manipulatedPoint.X = _manipulationStartTargetPosition.X - move.X;

            // 좌에서 우로갈때 너비가 음수가 되지 않도록 해야한다.
            if (_manipulationStartTargetSize.Width + move.X < 5)
                return false;

            manipulatedSize.Width = _manipulationStartTargetSize.Width + move.X;
            return true;
        }

        private bool ManipulateRight(Point pos, Vector move, out Point manipulatedPoint, out Size manipulatedSize)
        {
            move.Y = 0; // 좌우 이동은 Y축 델타를 0으로만든다.
            manipulatedPoint = _manipulationStartTargetPosition;
            manipulatedSize = _manipulationStartTargetSize;

            // 우에서 좌로갈때 너비가 음수가 되지 않도록 해야한다.
            if (_manipulationStartTargetSize.Width - move.X < 5)
                return false;

            manipulatedSize.Width = _manipulationStartTargetSize.Width - move.X;
            return true;
        }

        private bool ManipulateTop(Point pos, Vector move, out Point manipulatedPoint, out Size manipulatedSize)
        {
            move.X = 0; // 상하 이동은 X축 델타를 0으로만든다.
            double bottomEdgeAxis = _manipulationStartTargetPosition.Y + _manipulationStartTargetSize.Height;

            manipulatedPoint = _manipulationStartTargetPosition;
            manipulatedSize = _manipulationStartTargetSize;

            if (manipulatedPoint.Y <= bottomEdgeAxis - 5)
                manipulatedPoint.Y = _manipulationStartTargetPosition.Y - move.Y;

            // 위에서 아래로갈때 높이가 음수가 되지 않도록 해야한다.
            if (_manipulationStartTargetSize.Height + move.Y < 5)
                return false;

            manipulatedSize.Height = _manipulationStartTargetSize.Height + move.Y;
            return true;
        }

        private bool ManipulateBottom(Point pos, Vector move, out Point manipulatedPoint, out Size manipulatedSize)
        {
            move.X = 0; // 상하 이동은 X축 델타를 0으로만든다.
            manipulatedPoint = _manipulationStartTargetPosition;
            manipulatedSize = _manipulationStartTargetSize;

            // 아래에서 위로갈때 높이가 음수가 되지 않도록 해야한다.
            if (_manipulationStartTargetSize.Height - move.Y < 5)
                return false;

            manipulatedSize.Height = _manipulationStartTargetSize.Height - move.Y;
            return true;
        }

        private bool ManipulateTopLeft(Point pos, Vector move, out Point manipulatedPoint, out Size manipulatedSize)
        {
            if (!ManipulateTop(pos, move, out Point topPoint, out Size topSize))
                return false;

            if (!ManipulateLeft(pos, move, out Point leftPoint, out Size leftSize))
                return false;

            manipulatedPoint = new Point(leftPoint.X, topPoint.Y);
            manipulatedSize = new Size(leftSize.Width, topSize.Height);
            return true;
        }

        private bool ManipulateTopRight(Point pos, Vector move, out Point manipulatedPoint, out Size manipulatedSize)
        {
            if (!ManipulateTop(pos, move, out Point topPoint, out Size topSize))
                return false;

            if (!ManipulateRight(pos, move, out Point rightPoint, out Size rightSize))
                return false;

            manipulatedPoint = new Point(rightPoint.X, topPoint.Y);
            manipulatedSize = new Size(rightSize.Width, topSize.Height);
            return true;
        }

        private bool ManipulateBottomLeft(Point pos, Vector move, out Point manipulatedPoint, out Size manipulatedSize)
        {
            if (!ManipulateBottom(pos, move, out Point bottomPoint, out Size bottomSize))
                return false;

            if (!ManipulateLeft(pos, move, out Point leftPoint, out Size leftSize))
                return false;

            manipulatedPoint = new Point(leftPoint.X, bottomPoint.Y);
            manipulatedSize = new Size(leftSize.Width, bottomSize.Height);
            return true;
        }

        private bool ManipulateBottomRight(Point pos, Vector move, out Point manipulatedPoint, out Size manipulatedSize)
        {
            if (!ManipulateBottom(pos, move, out Point bottomPoint, out Size bottomSize))
                return false;

            if (!ManipulateRight(pos, move, out Point rightPoint, out Size rightSize))
                return false;

            manipulatedPoint = new Point(rightPoint.X, bottomPoint.Y);
            manipulatedSize = new Size(rightSize.Width, bottomSize.Height);
            return true;

            return true;
        }

        private bool ManipulateCenter(Point pos, Vector move, out Point manipulatedposition, out Size manipulatedsize)
        {
            throw new Exception("매니퓰레이트 불가능한 모드입니다.");
        }


        public void OnMouseUpManipulation()
        {
            Mouse.OverrideCursor = Cursors.Arrow;
            _manipulationMode = Positioning.Center;
            _manipulationTarget = null;
        }


        public event PropertyChangedEventHandler? PropertyChanged;
        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        private struct MovingElement
        {
            public SGUIElement Element { get; }
            public Point StartPosition { get; }

            public MovingElement(SGUIElement element, Point startPosition)
            {
                Element = element;
                StartPosition = startPosition;
            }
        }

        private class RestoreSizeCommand : ICommand
        {
            public event EventHandler? CanExecuteChanged;
            public bool CanExecute(object? parameter) => true;

            public void Execute(object? parameter)
                => (parameter as ISizeRestorable).RestoreSize();
        }

        public void DragEnd(Point p, object data)
        {
            SgaSprite sprite = data as SgaSprite;

            if (sprite == null)
                return;

            Point pos = Mouse.GetPosition(this).Zoom(ViewModel.ZoomState);
            ObservableCollection<SGUIElement> pickedElements = ViewModel.GroupMaster.PickedElements;
            if (pickedElements.Count == 0)
            {
                MessageBoxEx.ShowTopMost("우선 그룹 또는 엘리먼트를 픽해주세요.");
                return;
            }

            // 놓은 지점에있는 그룹들중 가장 위에 그룹을 가져온다.
            SGUIGroup topLevelGroup = pickedElements.Where(element => element.IsGroup && element.ContainPoint(pos)).Cast<SGUIGroup>().LastOrDefault();
            if (topLevelGroup == null)
            {
                MessageBoxEx.ShowTopMost("해당위치에 그룹이 없습니다.\n그룹내에 드랍해야합니다.");
                return;
            }

            CreateElementView elementView = new CreateElementView(topLevelGroup, sprite, pos);
            elementView.ShowDialog();
        }

        public bool ContainPoint(Point p)
        {
            return VisualEx.ContainPoint(this, p);
        }
    }
}
