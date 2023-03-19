/*
 * 작성자: 윤정도
 * 생성일: 3/5/2023 2:22:45 AM
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Diagnostics.Metrics;
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
using SGToolsCommon.CustomControl;
using SGToolsCommon.Extension;
using SGToolsCommon.Model;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.CustomControl
{


    public class UIElementTreeView : TreeView, IDataDragReceiver, IKeyboardInputReceiver
    {

        public MainViewModel ViewModel { get; private set; }
        public ScrollViewer ScrollViewer { get; private set; }

        public UIElementTreeView()
        {
            Loaded += OnLoaded;
            PreviewMouseLeftButtonDown += OnPreviewMouseLeftButtonDown;
            /*
             * TreeViewItem Loaded 추가를 위해 내가 시도한 방법들
             * 1. TreeView와 TreeViewItem을 상속받은 클래스를 정의한 후 GetContainerForItemOverride 함수를 오버라이딩해서
             *    TreeViewItemn 생성 후 Loaded 이벤트에 콜백이벤트 추가해줬는데
             *    이렇게 하니까 Depth가 2이상인 TreeViewItem에서 Loaded 이벤트가 호출이 안됨
             *
             *    public class TreeViewItemImpl : TreeViewItem
             *    {
             *        protected override DependencyObject GetContainerForItemOverride()
             *        {
             *            var treeViewItem = new TreeViewItem();
             *            treeViewItem.Loaded += OnTreeViewItemLoaded;
             *            return treeViewItem;
             *        }
             *    
             *        // 확장 누르면 로딩함 ㅡㅡ;
             *        public static void OnTreeViewItemLoaded(object sender, RoutedEventArgs e)
             *        {
             *            TreeViewItem item = sender as TreeViewItem;
             *            if (item == null)
             *                throw new Exception("이럴 수 없어요 어떻게 트리뷰 아이템 아닐 수 있죠?");
             *    
             *            SGUIElement element = item.DataContext as SGUIElement;
             *            if (element == null)
             *                throw new Exception("로드된 트리뷰 아이템에 데이터 컨텍스트가 설정되어있지 않습니다.");
             *            element.OnTreeViewItemLoaded(item);
             *        }
             *    }
             *
             * 2. 컨테이너 스타일을 코드로 작성할려고 했는데. 코드가 가독성도 떨어지고 별로같음
             *    ItemContainerStyle = new Style(typeof(TreeViewItem));
             *    ItemContainerStyle.BasedOn = (Style)Application.Current.FindResource("WinformTreeViewItem");
             *    ItemContainerStyle.Setters.Add(new EventSetter(TreeViewItem.LoadedEvent, new RoutedEventHandler(OnTreeViewItemLoaded)));
             *
             * 3. CustomStyle.UIElementTreeView 리소스파일과 연결할 클래스 UIElementTreeViewStyle.cs를 만든 후 연결해줌
             *    이게 제일 깔끔한 것 같다.
             */

        }


        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            InitializeViewModel();
        }


        private void InitializeViewModel()
        {
            ViewModel = DataContext as MainViewModel;

            // 디자이너에서는 무조건 예외가 터지기땜에 막아줘야함.
            if (DesignerProperties.GetIsInDesignMode(this))
                return;

            if (ViewModel == null)
                throw new Exception("UIElementTreeView에서 뷰모델 초기화 실패");
            ScrollViewer = ViewModel.View.UIElementTreeViewScrollViewer;
        }

        // ======================================================================
        //             이벤트
        // ======================================================================

        public void OnKeyDown(SGKey key)
        {
            var commandCenter = ViewModel.Commander;
            var groupMaster = ViewModel.GroupMaster;
            SGUIElement element = groupMaster.SelectedElement;

            if (element == null)
                return;

            SGUIGroup parent = element.Parent;
            SGUIGroup group = null;
            if (element is SGUIGroup)
                group = (SGUIGroup)element;

            switch (key)
            {
                case SGKey.Up:
                    SGUIElement prev = element.Previous;
                    if (prev != null) commandCenter.SelectUIElement.Execute(prev);
                    break;
                case SGKey.Down:
                    SGUIElement next = element.Next;
                    if (next != null) commandCenter.SelectUIElement.Execute(next);
                    break;
                case SGKey.Enter:
                case SGKey.Space:
                {
                    if (group == null)
                        break;

                    group.Item.IsExpanded = !group.Item.IsExpanded;
                    break;
                }
                case SGKey.Delete:
                {
                    if (ViewModel.GroupMaster.HasSelectedElement && MessageBoxEx.ShowTopMost("정말로 삭제하시겠습니까?", "질문임", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.Yes)
                        ViewModel.Commander.DeleteUIElement.Execute(null);
                    break;
                }
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

        public void OnKeyUp(SGKey key)
        {

        }

        private void OnPreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Point pos = e.GetPosition(this);
            var hit = this.HitTest<UIElementTreeView, TreeViewItem, SGUIElement>(pos);

            if (hit == null)
                return;

            SGUIElement? selected = hit.DataContext;

            if (selected == null)
                return;

            if (e.ClickCount == 1)
            {
                SGUIElement? prevSelected = ViewModel.GroupMaster.SelectedElement;

                if (ViewModel.UIElementSelectMode == SelectMode.Keep && prevSelected != null &&
                    prevSelected != selected)
                {
                    List<SGUIElement> betweenElements =
                        ViewModel.GroupMaster.GetElementsBetween(prevSelected, selected, true);
                    ViewModel.Commander.SelectUIElement.Execute(betweenElements);
                    return;
                }

                ViewModel.Commander.SelectUIElement.Execute(selected);
            }
            else if (e.ClickCount > 1)
            {
                e.Handled = true;

                ViewModel.Commander.UnpickUIElement.Execute(null);
                selected.Picked = true;


                // https://stackoverflow.com/questions/6037883/how-to-disable-double-click-behaviour-in-a-wpf-treeview
                // 마우스 더블클릭으로 아이템 확장 안되도록 만듬.
                // 이게 엄청 거슬리네
                // 터널링 이벤트가 자식 이벤트로 클릭이 전파되지 않도록하는걸로 보인다.
            }
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
        }

        protected override void OnMouseLeftButtonUp(MouseButtonEventArgs e)
        {
        }

        protected override void OnMouseWheel(MouseWheelEventArgs e)
        {
            if (e.Delta > 0)
                ScrollViewer.LineUp();
            else if (e.Delta < 0)
                ScrollViewer.LineDown();
        }

     

        public void DragEnd(Point p, object data)
        {

        }

        public bool ContainPoint(Point p) => SGToolsCommon.Extension.VisualEx.ContainPoint(this, p);

    }
}
