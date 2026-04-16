// jdyun 25/02/02(화)
using System;
using System.Globalization;
using System.Windows.Data;

namespace SGToolsCommon.Customize.Converter
{
    public class StringDecorateConverter : IValueConverter
    {
        public object? Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            string s = string.Empty;
            if (_value is null)
                s = "(null)";
            else
                s = _value.ToString() ?? string.Empty;

            if (_parameter is null)
            {
                // 데코레이션 할건데 어떻게 데코레이션 할지 전달 안해줌. 그럼 그냥 반환
                return s;
            }

            string? d = _parameter.ToString();
            return string.Format(d ?? "{0}", s);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
