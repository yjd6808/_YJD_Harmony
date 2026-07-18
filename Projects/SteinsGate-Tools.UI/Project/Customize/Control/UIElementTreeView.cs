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
            var rootGroup = ViewModel.RootGroup;
            SGUIElement element = rootGroup.SelectedElement;
            WorkspaceTreeItem? wsItem = FindSelectedWorkspaceItem();

            if (element == null && wsItem == null)
            {
                if (rootGroup.XmlFilePath != null)
                {
                    switch (_key)
                    {
                        case SGKey.Delete:
                        case SGKey.X:
                        case SGKey.C:
                        case SGKey.V:
                            return;
                    }
                }
                return;
            }

            if (wsItem != null)
            {
                switch (_key)
                {
                    case SGKey.Up:
                    case SGKey.Down:
                        NavigateWorkspaceItem(wsItem, _key == SGKey.Up);
                        return;
                    case SGKey.Enter:
                    case SGKey.Space:
                        if (wsItem.IsDirectory)
                            wsItem.IsExpanded = !wsItem.IsExpanded;
                        return;
                    case SGKey.Delete:
                    case SGKey.X:
                    case SGKey.C:
                    case SGKey.V:
                        return;
                }
                return;
            }

            SGUIGroup? group = element as SGUIGroup;

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
                    if (ViewModel.RootGroup.HasSelectedElement && MessageBoxEx.ShowTopMost("정말로 삭제하시겠습니까?", "질문임", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.Yes)
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
        private void DeselectAllWorkspaceItems()
        {
            if (ViewModel.WorkspaceRoot == null)
                return;

            ForEachWorkspaceItem(ViewModel.WorkspaceRoot, item => item.Selected = false);
        }

        private void ForEachWorkspaceItem(WorkspaceTreeItem _parent, Action<WorkspaceTreeItem> _action)
        {
            _action(_parent);
            foreach (var child in _parent.Children)
            {
                if (child is WorkspaceTreeItem wsItem)
                    ForEachWorkspaceItem(wsItem, _action);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private WorkspaceTreeItem? FindSelectedWorkspaceItem()
        {
            if (ViewModel.WorkspaceRoot == null)
                return null;

            WorkspaceTreeItem? result = null;
            ForEachWorkspaceItem(ViewModel.WorkspaceRoot, item =>
            {
                if (item.Selected)
                    result = item;
            });
            return result;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void NavigateWorkspaceItem(WorkspaceTreeItem _current, bool _up)
        {
            var flatList = new List<WorkspaceTreeItem>();
            FlattenWorkspaceTree(ViewModel.WorkspaceRoot!, flatList);

            int index = flatList.IndexOf(_current);
            if (index < 0) return;

            int target = _up ? index - 1 : index + 1;
            if (target < 0 || target >= flatList.Count)
                return;

            WorkspaceTreeItem targetItem = flatList[target];
            DeselectAllWorkspaceItems();
            targetItem.Selected = true;
        }

        private void FlattenWorkspaceTree(WorkspaceTreeItem _parent, List<WorkspaceTreeItem> _list)
        {
            _list.Add(_parent);
            foreach (var child in _parent.Children)
            {
                if (child is WorkspaceTreeItem wsItem)
                    FlattenWorkspaceTree(wsItem, _list);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnPreviewMouseLeftButtonDown(object _sender, MouseButtonEventArgs _e)
        {
            IntPoint pos = _e.GetPosition(this);
            var hit = this.HitTest<UIElementTreeView, TreeViewItem, SGUIElement>(pos);
            if (hit != null)
            {
                SGUIElement? selected = hit.DataContext;

                if (selected == null)
                    return;

                if (_e.ClickCount == 1)
                {
                    SGUIElement? prevSelected = ViewModel.RootGroup.SelectedElement;

                    if (ViewModel.UIElementSelectMode == SelectMode.Keep && prevSelected != null &&
                        prevSelected != selected)
                    {
                        List<SGUIElement> betweenElements =
                            ViewModel.RootGroup.GetElementsBetween(prevSelected, selected, true);
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
