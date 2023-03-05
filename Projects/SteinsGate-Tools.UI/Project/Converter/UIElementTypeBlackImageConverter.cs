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
    public class UIElementTypeBlackImageConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            SGUIElementType elementType = (SGUIElementType)value;

            switch (elementType)
            {
                case SGUIElementType.Group: return Constant.UIGroupIconImageSource;
                case SGUIElementType.Button: return Constant.UIButtonIconImageSource;
                case SGUIElementType.CheckBox: return Constant.UICheckBoxIconImageSource;
                case SGUIElementType.EditBox: return Constant.UIEditBoxIconImageSource;
                case SGUIElementType.Label: return Constant.UILabelIconImageSource;
            }

            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }
    }
}
