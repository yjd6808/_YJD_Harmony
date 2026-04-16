// jdyun 24/09/07(토)

using System;
using System.Globalization;
using System.Windows.Data;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Customize.Converter
{
    public class HexConverter : IValueConverter
    {
        public static readonly HexConverter Instance = new();

        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            long l = _value.ToLong();
            return $"0x{l:x8}";
        }

        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException("구현이 안되었어요");
        }
    }
}
