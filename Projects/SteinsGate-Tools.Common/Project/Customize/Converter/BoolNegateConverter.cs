/*
 * 작성자: 윤정도
 * 생성일: 1/28/2025 10:03:47 AM
 *
 */

using System;
using System.Globalization;
using System.Windows.Data;

namespace SGToolsCommon.Customize.Converter
{
    public class BoolNegateConverter : IValueConverter
    {
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_value is bool boolValue)
            {
                return !boolValue;
            }
            return _value;
        }

        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }
}
