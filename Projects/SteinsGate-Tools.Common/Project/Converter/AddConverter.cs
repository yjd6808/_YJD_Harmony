/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 12:56:58 AM
 *
 */

using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;
using System.Windows.Markup;

namespace SGToolsCommon.Converter
{
    public class AddConverter : MarkupExtension, IValueConverter
    {
        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            switch (Type.GetTypeCode(_targetType))
            {
                case TypeCode.Byte:
                case TypeCode.SByte:
                case TypeCode.UInt16:
                case TypeCode.UInt32:
                case TypeCode.UInt64:
                case TypeCode.Int16:
                case TypeCode.Int32:
                case TypeCode.Int64:
                case TypeCode.Decimal:
                    if (_parameter is string)
                        return (long)_value + long.Parse((string)_parameter);

                    return (long)_value + (long)_parameter;
                case TypeCode.Double:
                case TypeCode.Single:
                    if (_parameter is string)
                        return (double)_value + double.Parse((string)_parameter);

                    return (double)_value + (double)_parameter;
                default:
                {
                    Thickness v = (Thickness)_value;
                    if (_targetType == typeof(Thickness))
                    {
                        if (_parameter is Thickness thickness)
                        {
                            v.Left += thickness.Left;
                            v.Top += thickness.Top;
                            v.Right += thickness.Right;
                            v.Bottom += thickness.Bottom;
                            return v;
                        }

                        double parsed = double.Parse((string)_parameter);
                        v.Left += parsed;
                        v.Top += parsed;
                        v.Right += parsed;
                        v.Bottom += parsed;
                    }
                    break;
                }
            }

            throw new ArgumentException("Parameter를 TargetType에 맞게 변환할 수 없습니다.");
        }

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            return null;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override object ProvideValue(IServiceProvider _serviceProvider)
        {
            return this;
        }
    }
}
