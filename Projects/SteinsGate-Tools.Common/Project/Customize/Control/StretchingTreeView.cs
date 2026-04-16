/*
 * 스택 복붙
 * https://stackoverflow.com/questions/47258955/how-to-stretch-treeviewitem-width-to-fill-parent
 * 생성일: 3/1/2023 3:18:01 PM
 *
 */

using System.Windows;
using System.Windows.Controls;

namespace SGToolsCommon.Customize.Control
{
    public class StretchingTreeView : TreeView
    {
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
    }
}
