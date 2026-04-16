// jdyun 24/11/10(일)
using System;
using System.Globalization;
using System.Windows.Data;

namespace SGToolsCommon.Customize.Converter
{
    public class StringConcatConverter : IMultiValueConverter
    {
        public static readonly StringConcatConverter Instance = new();

        public object Convert(object[] _values, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_values == null)
                return string.Empty;

            string s = string.Empty;
            foreach (object v in _values)
            {
                s += v;
            }
            return s;
        }

        public object[] ConvertBack(object _value, Type[] _targetTypes, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }
}
