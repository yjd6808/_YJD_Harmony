/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 12:15:11 PM
 *
 */

using System;
using System.Globalization;
using System.Windows.Data;

namespace SGToolsCommon.Converter
{
    public class CutStringConverter : IValueConverter
    {
        public static readonly CutStringConverter Instance = new CutStringConverter();

        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            string s = (string)_value;
            int maxLength = (int)_parameter;

            if (s.Length <= maxLength)
                return s;

            return s.Substring(0, maxLength);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }
}
