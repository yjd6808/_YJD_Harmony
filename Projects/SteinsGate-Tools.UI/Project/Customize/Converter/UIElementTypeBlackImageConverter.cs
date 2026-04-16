/*
 * 작성자: 윤정도
 *
 */

using System;
using System.Globalization;
using System.Windows.Data;
using SGToolsUI.Model.Main;
using SGToolsUI.Resource;

namespace SGToolsUI.Customize.Converter
{
    public class UIElementTypeBlackImageConverter : IValueConverter
    {
        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            SGUIElementType elementType = (SGUIElementType)_value;

            switch (elementType)
            {
                case SGUIElementType.Group:         return R.UIGroupIconImageSource;
                case SGUIElementType.Button:        return R.UIButtonIconImageSource;
                case SGUIElementType.Label:         return R.UILabelIconImageSource;
                case SGUIElementType.Sprite:        return R.UISpriteIconImageSource;
                case SGUIElementType.CheckBox:      return R.UICheckBoxIconImageSource;
                case SGUIElementType.EditBox:       return R.UIEditBoxIconImageSource;
                case SGUIElementType.ToggleButton:  return R.UIToggleButtonIconImageSource;
                case SGUIElementType.ProgressBar:   return R.UIProgressBarIconImageSource;
                case SGUIElementType.ScrollBar:     return R.UIScrollBarIconImageSource;
                case SGUIElementType.Static:        return R.UIStaticIconImageSource;
            }

            return null!;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
            => null!;
    }
}
