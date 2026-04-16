/*
 * 작성자: 윤정도
 * 생성일: 3/5/2023 2:22:45 AM
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using SGToolsCommon.Extension;
using SGToolsCommon.Model;
using SGToolsCommon.Primitive;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Customize.Control
{
    public class UIElementTreeView : TreeView, IDataDragReceiver, IKeyboardInputReceiver
    {
        public MainViewModel ViewModel { get; private set; } = null!;
        public ScrollViewer ScrollViewer { get; private set; } = null!;

        //////////////////////////////////////////////////////////////////////////////////
        public UIElementTreeView()
        {
            Loaded += OnLoaded;
            PreviewMouseLeftButtonDown += OnPreviewMouseLeftButtonDown;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnLoaded(object _sender, RoutedEventArgs _e)
        {
            InitializeViewModel();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void InitializeViewModel()
        {
            ViewModel = (DataContext as MainViewModel)!;

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

        //////////////////////////////////////////////////////////////////////////////////
        public void OnKeyDown(SGKey _key)
        {
            var commandCenter = ViewModel.Commander;
            var groupMaster = ViewModel.GroupMaster;
            SGUIElement element = groupMaster.SelectedElement;

            if (element == null)
                return;

            SGUIGroup parent = element.Parent;
            SGUIGroup? group = null;
            if (element is SGUIGroup)
                group = (SGUIGroup)element;

            switch (_key)
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
        private void OnPreviewMouseLeftButtonDown(object _sender, MouseButtonEventArgs _e)
        {
            IntPoint pos = _e.GetPosition(this);
            var hit = this.HitTest<UIElementTreeView, TreeViewItem, SGUIElement>(pos);

            if (hit == null)
                return;

            SGUIElement? selected = hit.DataContext;

            if (selected == null)
                return;

            if (_e.ClickCount == 1)
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
            else if (_e.ClickCount > 1)
            {
                _e.Handled = true;
                ViewModel.Commander.PickUIElement.Execute(selected);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected override void OnMouseMove(MouseEventArgs _e)
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected override void OnMouseLeftButtonUp(MouseButtonEventArgs _e)
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected override void OnMouseWheel(MouseWheelEventArgs _e)
        {
            if (_e.Delta > 0)
                ScrollViewer.LineUp();
            else if (_e.Delta < 0)
                ScrollViewer.LineDown();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void DragEnd(IntPoint _p, object _data)
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public bool ContainPoint(IntPoint _p)
            => SGToolsCommon.Extension.VisualEx.ContainPoint(this, _p);
    }
}
