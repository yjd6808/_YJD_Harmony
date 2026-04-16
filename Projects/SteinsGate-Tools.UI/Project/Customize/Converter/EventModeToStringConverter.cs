/*
 * 작성자: 윤정도
 *
 */

using System;
using System.Globalization;
using System.Windows.Data;

namespace SGToolsUI.Customize.Converter
{
    // 이벤트 모드가 true이면 "이벤트모드"문자열 반환하는 용도
    public class EventModeToStringConverter : IValueConverter
    {
        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
            => (bool)_value ? "이벤트 모드" : "일반 모드";

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
            => null!;
    }
}
