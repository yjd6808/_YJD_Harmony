using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Windows.Media.Imaging;
using SGToolsUI.Model;
using SGToolsUI.Resource;

namespace SGToolsUI.Converter
{
    // 이벤트 모드가 true이면 "이벤트모드"문자열 반환하는 용도
    public class EventModeToStringConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return (bool)value ? "이벤트 모드" : "일반 모드";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }
    }
}
