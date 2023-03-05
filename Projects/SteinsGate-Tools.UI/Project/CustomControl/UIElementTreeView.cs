/*
 * 작성자: 윤정도
 * 생성일: 3/5/2023 2:22:45 AM
 *
 */

using System;
using System.Collections.Generic;
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
using SGToolsCommon.CustomControl;
using SGToolsCommon.Extension;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.Model;
using SGToolsUI.ViewModel;

namespace SGToolsUI.CustomControl
{
    public class UIElementTreeView : TreeView
    {
        public MainViewModel ViewModel { get; private set; }
        public ScrollViewer ScrollViewer { get; private set; }

        public UIElementTreeView()
        {
            Loaded += OnLoaded;
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

        protected override void OnPreviewMouseLeftButtonDown(MouseButtonEventArgs e)
        {
            HitTestResult hit = VisualTreeHelper.HitTest(this, Mouse.GetPosition(this));

            if (hit.VisualHit == null)
                return;

            var item = hit.VisualHit.FindParent<TreeViewItem>();

            if (item == null)
                return;

            if (item.DataContext is not SGUIElement)
                throw new Exception("선택한 트리뷰 아이템의 데이터컨텍스트가 설정되어있지 않습니다.");


            SGUIElement? selected = item.DataContext as SGUIElement;
            SGUIElement? prevSelected = ViewModel.GroupMaster.SelectedElement;

            if (ViewModel.UIElementSelectMode == SelectMode.Keep && prevSelected != null)
            {
                List<SGUIElement> betweenElements = ViewModel.GroupMaster.GetElementsBetween(prevSelected, selected);
                ViewModel.Commander.SelectUIElement.Execute(betweenElements);
                return;
            }

            ViewModel.Commander.SelectUIElement.Execute(selected);
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

        protected override void OnSelectedItemChanged(RoutedPropertyChangedEventArgs<object> e)
        {
            base.OnSelectedItemChanged(e);
        }

        protected override void OnMouseDoubleClick(MouseButtonEventArgs e)
        {
            SGUIElement selected = SelectedItem as SGUIElement;

            if (selected == null)
                return;

            ViewModel.Commander.PickUIElement.Execute(selected);
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
            => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }
}
