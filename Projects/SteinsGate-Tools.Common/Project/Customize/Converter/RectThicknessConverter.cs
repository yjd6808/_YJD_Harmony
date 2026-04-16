/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 7:45:42 AM
 *
 */

using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

using SGToolsCommon.Primitive;

namespace SGToolsCommon.Customize.Converter
{
    public class RectThicknessConverter : IValueConverter
    {
        public static readonly RectThicknessConverter Instance = new();

        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            switch (_value)
            {
            case Rect rc:  return new Thickness(rc.Left, rc.Top, 0, 0);
            case IntRect irc: return new Thickness(irc.Left, irc.Top, 0, 0);
            default: throw new Exception("value가 rect 타입이 아닙니다,");
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_value is not Thickness)
                throw new Exception("value가 thickness 타입이 아닙니다,");

            Thickness r = (Thickness)_value;
            return new Rect(r.Left, r.Top, 0, 0);
        }
    }
}
