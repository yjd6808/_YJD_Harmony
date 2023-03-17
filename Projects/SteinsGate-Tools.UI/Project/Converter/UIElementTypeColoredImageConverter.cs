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
    public class UIElementTypeColoredImageConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            SGUIElementType elementType = (SGUIElementType)value;

            switch (elementType)
            {
                case SGUIElementType.Group: return R.UIColoredGroupIconImageSource;
                case SGUIElementType.Button: return R.UIColoredButtonIconImageSource;
                case SGUIElementType.Label: return R.UIColoredLabelIconImageSource;
                case SGUIElementType.Sprite: return R.UIColoredSpriteIconImageSource;
                case SGUIElementType.CheckBox: return R.UIColoredCheckBoxIconImageSource;
                case SGUIElementType.EditBox: return R.UIColoredEditBoxIconImageSource;
                case SGUIElementType.ToggleButton: return R.UIColoredToggleButtonIconImageSource;
                case SGUIElementType.ProgressBar: return R.UIColoredProgressBarIconImageSource;
                case SGUIElementType.ScrollBar: return R.UIColoredScrollBarIconImageSource;
            }

            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }
    }
}
