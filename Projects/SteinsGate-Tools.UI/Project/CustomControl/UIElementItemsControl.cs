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

namespace SGToolsUI.CustomControl
{
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
            MoveBegin(e);
        }

        
        protected override void OnMouseMove(MouseEventArgs e)
        {
            MoveMove(e);
        }

        
        protected override void OnMouseUp(MouseButtonEventArgs e)
        {
            MoveEnd(e);
        }


        // ======================================================================
        //             기능
        // ======================================================================


        private void MoveBegin(MouseButtonEventArgs e)
        {
            _moveStartPosition = e.GetPosition(this).Zoom(ViewModel.ZoomState);
            
            IEnumerable<SGUIElement> pickedSelectedElements = ViewModel.GroupMaster.PickedSelectedElements;

            // 1. 알트키를 누른경우 겹친 엘리먼트 뒤에있는 원소를 순차적으로 선택할 수 있도록 한다.
            // 2. 선택된 엘리먼트가 있더라도 움직일 수 없도록 한다.
            bool alt = ViewModel.KeyState.IsAltPressed;


            // 마우스를 클릭한 지점에 선택된 원소가 있는 경우 마우스를 따라 움직일 수 있도록 한다.
            if (!alt && pickedSelectedElements.FirstOrDefault(element => element.ContainPoint(_moveStartPosition)) != null)
            {
                ViewModel.View.CanvasShapesControl.IsDraggable = false;
                ViewModel.View.TitlePanel.Draggable = false;

                // 각 엘리먼트의 시작위치를 기록해놓는다.
                _movingElements = pickedSelectedElements.Select(element => new MovingElement(element, element.VisualPosition)).ToList();
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
            _movingElements.ForEach(m => m.Element.VisualPosition = Point.Subtract(m.StartPosition, move));
        }

        public void MoveEnd(MouseButtonEventArgs e)
        {
            if (_isElementsMove)
            {

                ViewModel.View.CanvasShapesControl.IsDraggable = true;
                ViewModel.View.TitlePanel.Draggable = true;

                _movingElements = null;
                _isElementsMove = false;
            }
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
