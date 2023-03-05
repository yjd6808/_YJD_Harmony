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
using System.Xml.Linq;

namespace SGToolsUI.CustomControl
{
    public class UIElementItemsControl : ItemsControl, INotifyPropertyChanged
    {
        public bool IsMove
        {
            get => _isMove;
            set
            {
                _isMove = value;
                MoveEnd(null);
                OnPropertyChanged();
            }
        }

        public MainViewModel ViewModel { get; private set; }
        public Canvas CanvasPanel => _canvasPanel;
        public ItemsPresenter Presenter => _canvasPresenter;

        private Canvas _canvasPanel;
        private ItemsPresenter _canvasPresenter;
        private bool _isMove;
        private Point _moveStartPosition;
        private List<MovingElement> _movingElements;

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

            _moveStartPosition = e.GetPosition(this);
            
            IEnumerable<SGUIElement> pickedSelectedElements = ViewModel.GroupMaster.PickedSelectedElements;

            if (pickedSelectedElements.FirstOrDefault(element => element.VisualRect.Contains(_moveStartPosition)) != null)
            {
                ViewModel.View.CanvasShapesControl.IsDraggable = false;
                ViewModel.View.TitlePanel.Draggable = false;

                // 각 엘리먼트의 시작위치를 기록해놓는다.
                _movingElements = pickedSelectedElements.Select(element => new MovingElement(element, element.VisualPosition)).ToList();
                _isMove = true;
                return;
            }

        }

        public void MoveMove(MouseEventArgs e)
        {
            if (!_isMove)
                return;

            if (_movingElements == null)
                return;

            Point pos = e.GetPosition(this);
            Vector move = Point.Subtract(_moveStartPosition, pos);
            _movingElements.ForEach(m => m.Element.VisualPosition = Point.Subtract(m.StartPosition, move));
        }

        public void MoveEnd(MouseButtonEventArgs e)
        {
            ViewModel.View.CanvasShapesControl.IsDraggable = true;
            ViewModel.View.TitlePanel.Draggable = true;

            _movingElements = null;
            _isMove = false;
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
