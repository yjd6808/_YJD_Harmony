/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 7:46:26 AM
 *
 * 이름 긴 패키지들 짧게 변경해서 보여주기위한 컨바터
 */

using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
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
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;

namespace SGToolsCommon.Converter
{
    public class StringUnescapeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return ((string)value).Unescape();
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
