/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 10:33:30 AM
 *
 */


using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using SGToolsCommon.CustomControl;
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;
using SGToolsUI.Model;
using SGToolsUI.ViewModel;

namespace SGToolsUI.CustomControl
{
    public class DragPreventListBoxItem : ListBoxItem
    {
        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
        {
            return;
        }

        protected override void OnMouseLeftButtonUp(MouseButtonEventArgs e)
        {
            base.OnMouseLeftButtonDown(e);
        }
    }

    public class SgaSpriteListBox : ListBox
    {
        public MainViewModel ViewModel { get; private set; }
        public ScrollBar ScrollBar { get; private set; }

        public SgaSpriteListBox()
        {
            Loaded += OnLoaded;
        }

        protected override DependencyObject GetContainerForItemOverride()
        {
            return new DragPreventListBoxItem();
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

            ScrollBar = this.FindChild<ScrollBar>();
        }

        protected override void OnPreviewMouseLeftButtonDown(MouseButtonEventArgs e)
        {
            Point pos = e.GetPosition(this);
            var hit = this.HitTest<SgaSpriteListBox, ListBoxItem, SgaSprite>(pos);

            if (hit == null)
                return;

            ViewModel.DragState.OnDragBegin(this, e.GetPosition(ViewModel.View), hit.DataContext);
        }
    }
}
