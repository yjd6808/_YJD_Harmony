/*
 * 작성자: 윤정도
 * 생성일: 3/14/2023 3:30:34 PM
 *
 */

using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

using SGToolsCommon.Primitive;

namespace SGToolsCommon.Converter
{
    public class SGAlignmentWpfAlignmentConverter : IValueConverter
    {
        public static readonly SGAlignmentWpfAlignmentConverter Instance = new();

        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_value is HAlignment)
                return (HorizontalAlignment)_value;
            if (_value is VAlignment)
                return (VerticalAlignment)_value;

            throw new Exception("value가 HAlignment 또는 VAlignment 타입이 아닙니다.");
        }

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }
}
