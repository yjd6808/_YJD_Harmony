// jdyun 24/10/05(일)
using System.Windows.Controls;

namespace SGToolsCommon.Extension
{
    public static class TabControlEx
    {
        public static TabControl? GetTabControl(this TabItem _tabItem)
        {
            TabControl? tabControl = _tabItem.FindParentFrameworkElement<TabControl>();
            return tabControl;
        }

        public static int GetIndexFromTabControl(this TabItem _tabItem)
        {
            TabControl? tabControl = GetTabControl(_tabItem);
            if (tabControl == null)
                return -1;

            return tabControl.Items.IndexOf(_tabItem);
        }
    }
}
