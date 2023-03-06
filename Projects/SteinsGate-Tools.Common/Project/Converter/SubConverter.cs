/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 12:56:58 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace SGToolsCommon.Converter
{
    public class SubConverter : MarkupExtension, IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {

            switch (Type.GetTypeCode(targetType))
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
                    if (parameter is string)
                        return (long)value - long.Parse((string)parameter);

                    return (long)value - (long)parameter;
                case TypeCode.Double:
                case TypeCode.Single:
                    if (parameter is string)
                        return (double)value - double.Parse((string)parameter);

                    return (double)value - (double)parameter;
                default:
                {
                    Thickness v = (Thickness)value;
                    if (targetType == typeof(Thickness))
                    {
                        if (parameter is Thickness thickness)
                        {
                            v.Left -= thickness.Left;
                            v.Top -= thickness.Top;
                            v.Right -= thickness.Right;
                            v.Bottom -= thickness.Bottom;
                            return v;
                        }

                        double parsed = double.Parse((string)parameter);
                        v.Left -= parsed;
                        v.Top -= parsed;
                        v.Right -= parsed;
                        v.Bottom -= parsed;
                    }
                    break;
                }
            }

            throw new ArgumentException("Parameter를 TargetType에 맞게 변환할 수 없습니다.");
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch (Type.GetTypeCode(targetType))
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
                    return (long)value - long.Parse((string)parameter);
                case TypeCode.Double:
                case TypeCode.Single:
                    return (double)value - double.Parse((string)parameter);
                default:
                    throw new InvalidCastException("숫자 타입이 아닙니다.");
            }
        }

        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            return this;
        }
    }
}
