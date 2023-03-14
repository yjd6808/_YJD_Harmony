/*
 * 작성자: 윤정도
 * 생성일: 3/14/2023 3:49:42 PM
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
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using SGToolsCommon.Primitive;

namespace SGToolsCommon.Converter
{
    public class SGHorizontalAlignmentWpfTextAlignmentConverter : IValueConverter
    {
        public static readonly SGHorizontalAlignmentWpfTextAlignmentConverter Instance = new();

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is HAlignment alignment)
                return (TextAlignment)alignment;

            throw new Exception("소스가 HAlignment 타입이 아닙니다.");
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }
    }
}
