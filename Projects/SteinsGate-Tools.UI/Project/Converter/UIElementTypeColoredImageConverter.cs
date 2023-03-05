using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Windows.Media.Imaging;
using SGToolsUI.Model;

namespace SGToolsUI.Converter
{
    public class UIElementTypeColoredImageConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            SGUIElementType elementType = (SGUIElementType)value;

            switch (elementType)
            {
                case SGUIElementType.Group: return Constant.UIColoredGroupIconImageSource;
                case SGUIElementType.Button: return Constant.UIColoredButtonIconImageSource;
                case SGUIElementType.CheckBox: return Constant.UIColoredCheckBoxIconImageSource;
                case SGUIElementType.EditBox: return Constant.UIColoredEditBoxIconImageSource;
                case SGUIElementType.Label: return Constant.UIColoredLabelIconImageSource;
            }

            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }
    }
}
