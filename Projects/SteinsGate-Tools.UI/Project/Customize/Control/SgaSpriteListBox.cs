/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 10:33:30 AM
 *
 */

using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Customize.Control
{
    public class DragPreventListBoxItem : ListBoxItem
    {
        //////////////////////////////////////////////////////////////////////////////////
        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs _e)
        {
            return;
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected override void OnMouseLeftButtonUp(MouseButtonEventArgs _e)
        {
            base.OnMouseLeftButtonDown(_e);
        }
    }

    public class SgaSpriteListBox : ListBox
    {
        public MainViewModel? ViewModel { get; private set; }
        public ScrollBar? ScrollBar { get; private set; }

        //////////////////////////////////////////////////////////////////////////////////
        public SgaSpriteListBox()
        {
            Loaded += OnLoaded;
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected override DependencyObject GetContainerForItemOverride()
            => new DragPreventListBoxItem();

        //////////////////////////////////////////////////////////////////////////////////
        private void OnLoaded(object _sender, RoutedEventArgs _e)
        {
            InitializeViewModel();
        }

        //////////////////////////////////////////////////////////////////////////////////
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

        //////////////////////////////////////////////////////////////////////////////////
        protected override void OnPreviewMouseLeftButtonDown(MouseButtonEventArgs _e)
        {
            IntPoint pos = _e.GetPosition(this);
            var hit = this.HitTest<SgaSpriteListBox, ListBoxItem, SgaSprite>(pos);

            if (hit == null)
                return;

            ViewModel!.DragState.OnDragBegin(this, _e.GetPosition(ViewModel!.View), hit.DataContext!);
        }
    }
}
