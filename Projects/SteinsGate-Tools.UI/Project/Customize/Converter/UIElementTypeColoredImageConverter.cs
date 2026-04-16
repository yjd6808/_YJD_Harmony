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
    public class UIElementTypeColoredImageConverter : IValueConverter
    {
        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            SGUIElementType elementType = (SGUIElementType)_value;

            switch (elementType)
            {
                case SGUIElementType.Group:         return R.UIColoredGroupIconImageSource;
                case SGUIElementType.Button:        return R.UIColoredButtonIconImageSource;
                case SGUIElementType.Label:         return R.UIColoredLabelIconImageSource;
                case SGUIElementType.Sprite:        return R.UIColoredSpriteIconImageSource;
                case SGUIElementType.CheckBox:      return R.UIColoredCheckBoxIconImageSource;
                case SGUIElementType.EditBox:       return R.UIColoredEditBoxIconImageSource;
                case SGUIElementType.ToggleButton:  return R.UIColoredToggleButtonIconImageSource;
                case SGUIElementType.ProgressBar:   return R.UIColoredProgressBarIconImageSource;
                case SGUIElementType.ScrollBar:     return R.UIColoredScrollBarIconImageSource;
                case SGUIElementType.Static:        return R.UIColoredStaticIconImageSource;
            }

            return null!;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
            => null!;
    }
}
