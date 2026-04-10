/*
 * 작성자: 챗 GPT
 * 생성일: 3/19/2023 10:22:54 PM
 *
 */

using System;
using System.ComponentModel;
using System.Globalization;

namespace SGToolsCommon.Primitive.Converter
{
    public class IntSizeConverter : TypeConverter
    {
        //////////////////////////////////////////////////////////////////////////////////
        public override bool CanConvertFrom(ITypeDescriptorContext _context, Type _sourceType)
        {
            if (_sourceType == typeof(string))
                return true;

            return base.CanConvertFrom(_context, _sourceType);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override object ConvertFrom(ITypeDescriptorContext _context, CultureInfo _culture, object _value)
        {
            if (_value is string str)
            {
                if (str.Contains(','))
                {
                    // 1,,3 => 1과 3분할 옵션을 RemoveEmptyEntries 안주면
                    // 1,,3 => 1과 ""과 3으로 분할됨
                    string[] parts = str.Split(',', StringSplitOptions.RemoveEmptyEntries);
                    if (parts.Length == 2 && int.TryParse(parts[0], out int width) && int.TryParse(parts[1], out int height))
                        return new IntSize { Width = width, Height = height };
                }
                else if (str.Contains(' '))
                {
                    string[] parts = str.Split(' ', StringSplitOptions.RemoveEmptyEntries);
                    if (parts.Length == 2 && int.TryParse(parts[0], out int width) && int.TryParse(parts[1], out int height))
                        return new IntSize { Width = width, Height = height };
                }
            }

            return base.ConvertFrom(_context, _culture, _value);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override object ConvertTo(ITypeDescriptorContext _context, CultureInfo _culture, object _value, Type _destinationType)
        {
            if (_destinationType == typeof(string) && _value is IntSize size)
                return $"{size.Width}, {size.Width}";

            return base.ConvertTo(_context, _culture, _value, _destinationType);
        }
    }
}
