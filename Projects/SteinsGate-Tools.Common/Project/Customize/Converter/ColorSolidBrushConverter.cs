/*
 * 작성자: 윤정도
 * 생성일: 3/15/2023 6:45:07 PM
 *
 */

using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Media;

namespace SGToolsCommon.Customize.Converter
{
    public class ColorSolidBrushConverter : IValueConverter
    {
        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_value is not Color color)
                throw new Exception("value가 Color 타입이 아닙니다.");
            return new SolidColorBrush(color);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            return null!;
        }
    }
}
