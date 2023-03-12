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
using MoreLinq;
using SGToolsCommon.Extension;
using SGToolsUI.Model;
using SGToolsUI.View;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;
using SGToolsUI.ViewModel;
using System.Xml.Linq;
using Accessibility;
using SGToolsUI.Extension;
using SGToolsUI.Command.MainViewCommand;

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

    public class UIElementItemsControl : ItemsControl, INotifyPropertyChanged
    {
        public bool IsMove
        {
            get => _isElementsMove;
            set
            {
                _isElementsMove = value;
                MoveEnd(null);
                OnPropertyChanged();
            }
        }

        public MainViewModel ViewModel { get; private set; }
        public Canvas CanvasPanel => _canvasPanel;
        public ItemsPresenter Presenter => _canvasPresenter;

        private Canvas _canvasPanel;
        private ItemsPresenter _canvasPresenter;
        private bool _isElementsMove;
        private bool _isShiftMove;
        private ShiftKeyMoving _shiftKeyMoving = ShiftKeyMoving.None;
        private Point _moveStartPosition;
        private List<MovingElement> _movingElements;
        private SGUIElement _prevSelectElement;                     // 이전에 마우스 포인터를 찍었을때 선택한 엘리먼트

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



        // ======================================================================
        //             터널링 이벤트
        // ======================================================================

        protected override void OnMouseDown(MouseButtonEventArgs e)
        {
            OnMouseDownEventMode(e);
            MoveBegin(e);
        }

      
        protected override void OnMouseMove(MouseEventArgs e)
        {
            OnMouseMoveEventMode(e);
            MoveMove(e);
        }

  
        protected override void OnMouseUp(MouseButtonEventArgs e)
        {
            OnMouseUpEventMode(e);
            MoveEnd(e);
        }

      

        // ======================================================================
        //             기능
        // ======================================================================


        // 그룹은 제일 처음부터 그룹이 드
        private IEnumerable<SGUIElement> PickedElementsForCanvasSelection
        {
            get
            {
                SGUIGroupMaster master =  ViewModel.GroupMaster;
                SGUIElement element = master.PickedElement;

                if (element == null)
                    yield break;
                ObservableCollection<SGUIElement> pickedElements = master.PickedElements;

                if (element.IsGroup)
                {
                    // 부모그룹이 픽된 경우 부모 그룹이 제일 앞이므로, 
                    for (int i = pickedElements.Count - 2; i >= 0; --i)
                        yield return pickedElements[i];

                    yield return pickedElements[pickedElements.Count - 1];
                }
                else
                {
                    // 단순하게 
                    for (int i = 0; i < pickedElements.Count; ++i) 
                        yield return pickedElements[i];
                }
            }
        }

        private void MoveBegin(MouseButtonEventArgs e)
        {
            bool alt = ViewModel.KeyState.IsAltPressed;
            bool ctrl = ViewModel.KeyState.IsCtrlPressed;
            bool space = ViewModel.KeyState.IsPressed(SGKey.Space);

            _isShiftMove = ViewModel.KeyState.IsShiftPressed;
            _moveStartPosition = e.GetPosition(this).Zoom(ViewModel.ZoomState);
            
            ObservableCollection<SGUIElement> pickedElements = ViewModel.GroupMaster.PickedElements;
            
            // 1. 알트키를 누른경우 겹친 엘리먼트 뒤에있는 원소를 순차적으로 선택할 수 있도록 한다.
            // 2. 선택된 엘리먼트가 있더라도 움직일 수 없도록 한다.
            
            if (space)
                return;

            if (ViewModel.IsEventMode)
                return;

            // 마우스를 클릭한 지점에 선택된 원소가 있는 경우 마우스를 따라 움직일 수 있도록 한다.
            if (!alt && !ctrl && pickedElements.Count > 0 && pickedElements.Last().Selected && pickedElements.Last().ContainPoint(_moveStartPosition))
            {
                ViewModel.View.CanvasShapesControl.IsDraggable = false;
                ViewModel.View.TitlePanel.Draggable = false;

                // 각 엘리먼트의 시작위치를 기록해놓는다.
                _movingElements = ViewModel.GroupMaster.PickedSelectedElements.Select(element => new MovingElement(element, element.VisualPosition)).ToList();
                _isElementsMove = true;
                return;
            }


            // 알트키를 눌린 경우 겹친 위치의 원소들을 순차적으로 선택할 수 있도록한다.
            if (alt)
            {
                // 픽된 원소들중 마우스를 클릭한 지점에 겹쳐져있는 모든 엘리먼트를 가져온다.
                // 이때 깊은 자식부터 가져와야하므로 뒤짚어줘야함 (역방향 반복자를 구현했기때문에 효율 굳)
                IEnumerable<SGUIElement> candidates = ViewModel.GroupMaster
                    .PickedElements
                    .Reversed()
                    .Where(element => element.CanvasSelectable && element.ContainPoint(_moveStartPosition));

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
                _prevSelectElement = ViewModel.GroupMaster
                    .PickedElements
                    .Reversed()
                    .FirstOrDefault(element => element.CanvasSelectable && element.ContainPoint(_moveStartPosition));

                if (_prevSelectElement != null)
                    ViewModel.Commander.SelectUIElement.Execute(_prevSelectElement);
            }
        }

        public void MoveMove(MouseEventArgs e)
        {
            if (!_isElementsMove)
                return;

            if (_movingElements == null)
                return;

            // 드래그 시작 후 마우스가 움직인 벡터만큼 다른 엘리먼트들도 벡터만큼 움직여준다.
            Point pos = e.GetPosition(this).Zoom(ViewModel.ZoomState);
            Vector move = Point.Subtract(_moveStartPosition, pos);

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

        public void MoveEnd(MouseButtonEventArgs e)
        {
            _isShiftMove = false;
            _shiftKeyMoving = ShiftKeyMoving.None;

            if (_isElementsMove)
            {

                ViewModel.View.CanvasShapesControl.IsDraggable = true;
                ViewModel.View.TitlePanel.Draggable = true;

                _movingElements = null;
                _isElementsMove = false;
            }
        }


        private void OnMouseDownEventMode(MouseButtonEventArgs mouseButtonEventArgs)
        {

        }
        private void OnMouseMoveEventMode(MouseEventArgs mouseEventArgs)
        {
        }
        private void OnMouseUpEventMode(MouseButtonEventArgs mouseButtonEventArgs)
        {
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

    }
}
