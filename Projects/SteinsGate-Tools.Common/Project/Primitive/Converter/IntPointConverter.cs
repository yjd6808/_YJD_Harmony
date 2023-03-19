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
    public class IntPointConverter : TypeConverter
    {
        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            if (sourceType == typeof(string))
                return true;

            return base.CanConvertFrom(context, sourceType);
        }

        public override object ConvertFrom(ITypeDescriptorContext context, CultureInfo culture, object value)
        {
            if (value is string str)
            {
                if (str.Contains(','))
                {
                    // 1,,3 => 1과 3분할 옵션을 RemoveEmptyEntries 안주면
                    // 1,,3 => 1과 ""과 3으로 분할됨
                    var parts = str.Split(',', StringSplitOptions.RemoveEmptyEntries);
                    if (parts.Length == 2 && int.TryParse(parts[0], out int x) && int.TryParse(parts[1], out int y))
                        return new IntPoint { X = x, Y = y };
                }
                else if (str.Contains(' '))
                {
                    var parts = str.Split(' ', StringSplitOptions.RemoveEmptyEntries);
                    if (parts.Length == 2 && int.TryParse(parts[0], out int x) && int.TryParse(parts[1], out int y))
                        return new IntPoint { X = x, Y = y };
                }
            }

            return base.ConvertFrom(context, culture, value);
        }

        public override object ConvertTo(ITypeDescriptorContext context, CultureInfo culture, object value, Type destinationType)
        {
            if (destinationType == typeof(string) && value is IntPoint point)
            {
                return $"{point.X}, {point.Y}";
            }
            return base.ConvertTo(context, culture, value, destinationType);
        }
    }
}
