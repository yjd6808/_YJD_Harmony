/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 7:46:26 AM
 *
 * 이름 긴 패키지들 짧게 변경해서 보여주기위한 컨바터
 */

using System;
using System.Globalization;
using System.Windows.Data;

using SGToolsCommon.Extension;

namespace SGToolsCommon.Converter
{
    public class StringUnescapeConverter : IValueConverter
    {
        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            return ((string)_value).Unescape();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }
}
