/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 7:45:42 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
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
using SGToolsCommon.Primitive;

namespace SGToolsCommon.Converter
{
    public class RectThicknessConverter : IValueConverter
    {
        public static readonly RectThicknessConverter Instance = new ();

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            switch (value)
            {
            case Rect rc:  return new Thickness(rc.Left, rc.Top, 0, 0);
            case IntRect irc: return new Thickness(irc.Left, irc.Top, 0, 0);
            default: throw new Exception("value가 rect 타입이 아닙니다,");
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is not Thickness)
                throw new Exception("value가 thickness 타입이 아닙니다,");

            Thickness r = (Thickness)value;
            return new Rect(r.Left, r.Top, 0, 0);
        }
    }
}
