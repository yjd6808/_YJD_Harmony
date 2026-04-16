// jdyun 24/10/04(토)
using System.Windows.Controls;

namespace SGToolsCommon.Extension
{
    public static class ComboBoxEx
    {
        public static TextBox? GetEditableTextBox(this ComboBox _cb)
        {
            return _cb.Template.FindName("PART_EditableTextBox", _cb) as TextBox;
        }

        public static ScrollViewer? GetDropDownScrollViewer(this ComboBox _cb)
        {
            return _cb.Template.FindName("DropDownScrollViewer", _cb) as ScrollViewer;
        }
    }
}
