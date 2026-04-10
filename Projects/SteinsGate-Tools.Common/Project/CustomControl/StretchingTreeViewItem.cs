/*
 * 스택 복붙
 * https://stackoverflow.com/questions/47258955/how-to-stretch-treeviewitem-width-to-fill-parent
 * 생성일: 3/1/2023 3:17:27 PM
 *
 */

using System.Windows;
using System.Windows.Controls;

namespace SGToolsCommon.CustomControl
{
    public class StretchingTreeViewItem : TreeViewItem
    {
        //////////////////////////////////////////////////////////////////////////////////
        public StretchingTreeViewItem()
        {
            this.Loaded += new RoutedEventHandler(StretchingTreeViewItem_Loaded);
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected override DependencyObject GetContainerForItemOverride()
        {
            return new StretchingTreeViewItem();
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected override bool IsItemItsOwnContainerOverride(object _item)
        {
            return _item is StretchingTreeViewItem;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void StretchingTreeViewItem_Loaded(object _sender, RoutedEventArgs _e)
        {
            if (this.VisualChildrenCount > 0)
            {
                Grid grid = this.GetVisualChild(0) as Grid;
                if (grid != null && grid.ColumnDefinitions.Count == 3)
                {
                    grid.ColumnDefinitions.RemoveAt(2);
                    grid.ColumnDefinitions[1].Width = new GridLength(1, GridUnitType.Star);
                }
            }
        }
    }
}
