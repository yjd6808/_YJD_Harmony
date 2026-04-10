/*
 * https://stackoverflow.com/questions/20494740/collapseall-items-of-a-treeview-within-wpf-application
 * 생성일: 3/3/2023 1:22:48 PM
 *
 */

using System.Linq;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;

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
            public static TreeViewItem GetTreeViewItem(this ItemsControl _treeView, object _item)
            {
                if (_treeView.ItemContainerGenerator.ContainerFromItem(_item) is TreeViewItem container)
                    return container;

                if (_treeView.ItemContainerGenerator.Status == GeneratorStatus.ContainersGenerated)
                {
                    foreach (object childItem in _treeView.Items)
                    {
                        if (_treeView.ItemContainerGenerator.ContainerFromItem(childItem) is TreeViewItem childContainer)
                        {
                            TreeViewItem targetContainer = childContainer.GetTreeViewItem(_item);
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
                        .Select(i => GetTreeViewItem(_parent, i, _isExpanded)))
                        SetExpansion(item, _isExpanded);
            }

            //////////////////////////////////////////////////////////////////////////////////
            private static TreeViewItem GetTreeViewItem(ItemsControl _parent, object _item, bool _isExpanded)
            {
                if (_item is TreeViewItem tvi)
                    return tvi;

                TreeViewItem result = ContainerFromItem(_parent, _item);
                if (result == null && _isExpanded)
                {
                    _parent.UpdateLayout();
                    result = ContainerFromItem(_parent, _item);
                }
                return result;
            }

            //////////////////////////////////////////////////////////////////////////////////
            private static TreeViewItem ContainerFromItem(ItemsControl _parent, object _item) =>
                (TreeViewItem)_parent.ItemContainerGenerator.ContainerFromItem(_item);
        }
    }
}
