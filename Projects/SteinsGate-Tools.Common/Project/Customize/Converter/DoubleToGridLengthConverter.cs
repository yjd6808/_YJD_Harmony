// jdyun 24/04/13(토)
using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Customize.Converter
{
    public class DoubleToGridLengthConverter : IValueConverter
    {
        public static readonly DoubleToGridLengthConverter Instance = new();
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_parameter == null)
                throw new Exception("value가 null입니다.");

            if (!_parameter.IsNumericType())
                throw new Exception("파라미터가 숫자 타입이어야 합니다.");

            return new GridLength(System.Convert.ToDouble(_parameter));
        }

        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }
}
