/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 1:33:20 PM
 *
 */

using System;
using System.Globalization;
using System.Windows.Controls;
using System.Windows.Data;

namespace SGToolsCommon.CustomStyle
{
    internal class TreeViewLineConverter : IValueConverter
    {
        public static readonly TreeViewLineConverter Converter = new();

        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            TreeViewItem item = (TreeViewItem)_value;
            ItemsControl ic = ItemsControl.ItemsControlFromItemContainer(item);
            return ic.ItemContainerGenerator.IndexFromContainer(item) == ic.Items.Count - 1;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            return false;
        }
    }
}
