// jdyun 24/04/13(토)
using System;
using System.Globalization;
using System.Linq;
using System.Windows.Data;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Customize.Converter
{
    public class DoubleAddConverter : IMultiValueConverter
    {
        public static readonly DoubleAddConverter Instance = new();

        public object Convert(object[] _values, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_values.Length != 2)
                throw new Exception("파라미터가 2개 필요합니다.");

            if (!_values.All(_x => _x.IsNumericType()))
                throw new Exception("파라미터가 숫자 타입이어야 합니다.");

            double add = System.Convert.ToDouble(_values[0]) + System.Convert.ToDouble(_values[1]);
            return add;
        }

        public object[] ConvertBack(object _value, Type[] _targetTypes, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }
}
