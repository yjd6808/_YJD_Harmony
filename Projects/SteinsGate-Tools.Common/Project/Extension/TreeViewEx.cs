/*
 * https://stackoverflow.com/questions/20494740/collapseall-items-of-a-treeview-within-wpf-application
 * 생성일: 3/3/2023 1:22:48 PM
 *
 */

using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Media;

namespace SGToolsCommon.Extension
{
    namespace System.Windows.Controls
    {
        public static class TreeViewExtensions
        {
            //////////////////////////////////////////////////////////////////////////////////
            public static void SetExpansion(this TreeView _treeView, bool _isExpanded) =>
                SetExpansion((ItemsControl)_treeView, _isExpanded);

            //////////////////////////////////////////////////////////////////////////////////
            // 바인딩된 원소로부터 TreeViewItem을 얻음
            public static TreeViewItem? GetTreeViewItem(this ItemsControl _treeView, object _item)
            {
                if (_treeView.ItemContainerGenerator.ContainerFromItem(_item) is TreeViewItem container)
                    return container;

                if (_treeView.ItemContainerGenerator.Status == GeneratorStatus.ContainersGenerated)
                {
                    foreach (object childItem in _treeView.Items)
                    {
                        if (_treeView.ItemContainerGenerator.ContainerFromItem(childItem) is TreeViewItem childContainer)
                        {
                            TreeViewItem? targetContainer = childContainer.GetTreeViewItem(_item);
                            if (targetContainer != null)
                                return targetContainer;
                        }
                    }
                }

                return null;
            }

            //////////////////////////////////////////////////////////////////////////////////
            private static void SetExpansion(ItemsControl _parent, bool _isExpanded)
            {
                if (_parent is TreeViewItem tvi)
                    tvi.IsExpanded = _isExpanded;

                if (_parent.HasItems)
                    foreach (TreeViewItem item in _parent.Items.Cast<object>()
                        .Select(i => GetTreeViewItem(_parent, i, _isExpanded))
                        .Where(i => i != null)
                        .Cast<TreeViewItem>())
                        SetExpansion(item, _isExpanded);
            }

            //////////////////////////////////////////////////////////////////////////////////
            private static TreeViewItem? GetTreeViewItem(ItemsControl _parent, object _item, bool _isExpanded)
            {
                if (_item is TreeViewItem tvi)
                    return tvi;

                TreeViewItem? result = ContainerFromItem(_parent, _item);
                if (result == null && _isExpanded)
                {
                    _parent.UpdateLayout();
                    result = ContainerFromItem(_parent, _item);
                }
                return result;
            }

            //////////////////////////////////////////////////////////////////////////////////
            private static TreeViewItem? ContainerFromItem(ItemsControl _parent, object _item) =>
                (TreeViewItem?)_parent.ItemContainerGenerator.ContainerFromItem(_item);
        }
    }

    public static class TreeViewEx
    {
        public static void CollapseAll(this TreeView _treeView, Func<TreeViewItem, bool>? _predicate = null)
        {
            foreach (var item in _treeView.Items)
            {
                if (_treeView.ItemContainerGenerator.ContainerFromItem(item) is TreeViewItem treeViewItem)
                {
                    if (_predicate != null && !_predicate(treeViewItem))
                        continue;

                    CollapseTreeViewItem(treeViewItem);
                }
            }
        }

        public static void CollapseAll(this TreeViewItem _treeViewItem, Func<TreeViewItem, bool>? _predicate = null)
        {
            foreach (var item in _treeViewItem.Items)
            {
                if (_treeViewItem.ItemContainerGenerator.ContainerFromItem(item) is TreeViewItem treeViewItem)
                {
                    if (_predicate != null && !_predicate(treeViewItem))
                        continue;

                    CollapseTreeViewItem(treeViewItem);
                }
            }
        }

        public static void CollapseTreeViewItem(TreeViewItem _treeViewItem, Func<TreeViewItem, bool>? _predicate = null)
        {
            _treeViewItem.IsExpanded = false;

            foreach (var item in _treeViewItem.Items)
            {
                if (_treeViewItem.ItemContainerGenerator.ContainerFromItem(item) is TreeViewItem childItem)
                {
                    if (_predicate != null && !_predicate(_treeViewItem))
                        continue;

                    CollapseTreeViewItem(childItem);
                }
            }
        }

        public static void ExpandAll(this TreeView _treeView)
        {
            foreach (var item in _treeView.Items)
            {
                if (_treeView.ItemContainerGenerator.ContainerFromItem(item) is TreeViewItem treeViewItem)
                {
                    ExpandTreeViewItem(treeViewItem);
                }
            }
        }

        public static void ExpandTreeViewItem(TreeViewItem _treeViewItem)
        {
            _treeViewItem.IsExpanded = true;

            foreach (var item in _treeViewItem.Items)
            {
                if (_treeViewItem.ItemContainerGenerator.ContainerFromItem(item) is TreeViewItem childItem)
                {
                    ExpandTreeViewItem(childItem);
                }
            }
        }

        public static TreeViewItem? GetSelectedTreeViewItem(this TreeView _treeView)
        {
            return _treeView.ItemContainerGenerator.ContainerFromIndex(_treeView.Items.CurrentPosition) as TreeViewItem;
        }

        // depth 기반 트리뷰 foreach 구현
        public static void ForEachDepth<T>(this ItemsControl _tvControl, int _maxDepth, Func<T, TreeViewItem, int, bool> _depthPredicate, int _depth = 0)
        {
            if (_depth > _maxDepth)
                return;

            foreach (var item in _tvControl.Items)
            {
                if (item is not T t)
                    continue;

                TreeViewItem? tvItem = _tvControl.ItemContainerGenerator.ContainerFromItem(item) as TreeViewItem;
                if (tvItem == null)
                    continue;

                if (_depthPredicate(t, tvItem, _depth))
                {
                    ForEachDepth(tvItem, _maxDepth, _depthPredicate, _depth + 1);
                }
            }
        }

        // depth 기반 트리뷰 foreach 구현
        public static void ForEach<T>(this ItemsControl _tvControl, Func<T, TreeViewItem, int, bool> _depthPredicate, int _depth = 0)
        {
            foreach (var item in _tvControl.Items)
            {
                if (item is not T t)
                    continue;

                TreeViewItem? tvItem = _tvControl.ItemContainerGenerator.ContainerFromItem(item) as TreeViewItem;
                if (tvItem == null)
                    return;

                if (_depthPredicate(t, tvItem, _depth))
                {
                    ForEach(tvItem, _depthPredicate, _depth + 1);
                }
            }
        }

        // 최상위 부모 TreeView를 가져온다.
        public static TreeView? GetParentTreeView(this TreeViewItem _item)
        {
            DependencyObject? parent = VisualTreeHelper.GetParent(_item);
            while (!(parent is TreeView))
            {
                parent = VisualTreeHelper.GetParent(parent);
            }

            TreeView? treeView = parent as TreeView;
            return treeView;
        }

        // 직속 부모를 가져온다.
        public static ItemsControl? GetParentItemsControl(this TreeViewItem _item)
        {
            DependencyObject? parent = VisualTreeHelper.GetParent(_item);
            while (!(parent is TreeViewItem || parent is TreeView))
            {
                parent = VisualTreeHelper.GetParent(parent);
            }

            return parent as ItemsControl;
        }

        // 직속 부모가 TreeViewItem인 경우 가져온다.
        public static TreeViewItem? GetParentTreeViewItem(this TreeViewItem _item)
        {
            DependencyObject? parent = VisualTreeHelper.GetParent(_item);
            while (!(parent is TreeViewItem))
            {
                parent = VisualTreeHelper.GetParent(parent);
            }

            TreeViewItem? tri = parent as TreeViewItem;
            return tri;
        }
    }
}
