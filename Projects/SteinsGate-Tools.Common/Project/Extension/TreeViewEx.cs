/*
 * https://stackoverflow.com/questions/20494740/collapseall-items-of-a-treeview-within-wpf-application
 * 생성일: 3/3/2023 1:22:48 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
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

namespace SGToolsCommon.Extension
{
    namespace System.Windows.Controls
    {
        public static class TreeViewExtensions
        {
            public static void SetExpansion(this TreeView treeView, bool isExpanded) =>
              SetExpansion((ItemsControl)treeView, isExpanded);

            static void SetExpansion(ItemsControl parent, bool isExpanded)
            {
                if (parent is TreeViewItem tvi)
                    tvi.IsExpanded = isExpanded;

                if (parent.HasItems)
                    foreach (var item in parent.Items.Cast<object>()
                  .Select(i => GetTreeViewItem(parent, i, isExpanded)))
                        SetExpansion(item, isExpanded);
            }

            static TreeViewItem GetTreeViewItem(
              ItemsControl parent, object item, bool isExpanded)
            {
                if (item is TreeViewItem tvi)
                    return tvi;

                var result = ContainerFromItem(parent, item);
                if (result == null && isExpanded)
                {
                    parent.UpdateLayout();
                    result = ContainerFromItem(parent, item);
                }
                return result;
            }

            static TreeViewItem ContainerFromItem(ItemsControl parent, object item) =>
              (TreeViewItem)parent.ItemContainerGenerator.ContainerFromItem(item);
        }
    }
}
