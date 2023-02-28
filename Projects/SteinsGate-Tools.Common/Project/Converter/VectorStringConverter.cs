/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 12:14:03 AM
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
    public class VectorStringConverter : MarkupExtension, IValueConverter
    {
        

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            Vector v = (Vector)value;
            return $"{v.X:F0} {v.Y:F0}";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            return this;
        }
    }
}
