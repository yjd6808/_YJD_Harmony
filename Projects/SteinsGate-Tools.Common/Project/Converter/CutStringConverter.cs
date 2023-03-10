/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 12:15:11 PM
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

namespace SGToolsCommon.Converter
{
    public class CutStringConverter : IValueConverter
    {
        public static readonly CutStringConverter Instance = new CutStringConverter();

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var s = ((string)value);
            int maxLength = (int)parameter;

            if (s.Length <= maxLength)
                return s;

            return s.Substring(0, maxLength);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
