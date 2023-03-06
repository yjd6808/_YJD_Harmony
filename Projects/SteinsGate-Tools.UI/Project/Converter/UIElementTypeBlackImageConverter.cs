using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Windows.Media.Imaging;
using SGToolsUI.Model;
using SGToolsUI.Resource;

namespace SGToolsUI.Converter
{
    public class UIElementTypeBlackImageConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            SGUIElementType elementType = (SGUIElementType)value;

            switch (elementType)
            {
                case SGUIElementType.Group: return R.UIGroupIconImageSource;
                case SGUIElementType.Button: return R.UIButtonIconImageSource;
                case SGUIElementType.CheckBox: return R.UICheckBoxIconImageSource;
                case SGUIElementType.EditBox: return R.UIEditBoxIconImageSource;
                case SGUIElementType.Label: return R.UILabelIconImageSource;
            }

            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }
    }
}
