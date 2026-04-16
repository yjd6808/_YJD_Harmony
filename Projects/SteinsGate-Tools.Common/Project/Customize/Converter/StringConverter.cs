// jdyun 24/04/16(화)
using System;
using System.Globalization;
using System.Windows.Data;

namespace SGToolsCommon.Customize.Converter
{
    public class StringMultiConverter : IMultiValueConverter
    {
        public static readonly StringConverter Instance = new();

        public object Convert(object[] _values, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_values.Length != 1)
                throw new Exception("value가 1개가 아닙니다.");

            return _values[0] is null ? "(null)" : (_values[0].ToString() ?? "(null)");
        }

        public object[] ConvertBack(object _value, Type[] _targetTypes, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }

    public class StringConverter : IValueConverter
    {
        public static readonly StringConverter Instance = new();

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is null)
                return "(null)";
            return value.ToString() ?? "(null)";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
