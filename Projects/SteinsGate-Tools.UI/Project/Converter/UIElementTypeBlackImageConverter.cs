using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Windows.Media.Imaging;
using SGToolsUI.Model.Main;
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
                case SGUIElementType.Label: return R.UILabelIconImageSource;
                case SGUIElementType.Sprite: return R.UISpriteIconImageSource;
                case SGUIElementType.CheckBox: return R.UICheckBoxIconImageSource;
                case SGUIElementType.EditBox: return R.UIEditBoxIconImageSource;
                case SGUIElementType.ToggleButton: return R.UIToggleButtonIconImageSource;
                case SGUIElementType.ProgressBar: return R.UIProgressBarIconImageSource;
                case SGUIElementType.ScrollBar: return R.UIScrollBarIconImageSource;
            }

            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }
    }
}
