// jdyun 24/04/29(월) - 오프신청

using System;
using System.Globalization;
using System.Windows.Data;

namespace SGToolsCommon.Customize.Converter
{
    public class DateToStringConverter : IValueConverter
    {
        public static readonly DateToStringConverter Instance = new();

        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_parameter is not string)
                throw new Exception("문자열을 인자로 전달해주세요.");

            if (_value is not DateTime)
                throw new Exception("데이트타임 개체가 아닙니다.");

            DateTime value = (DateTime)_value;
            return value.ToString((string)_parameter);
        }

        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException("구현이 안되었어요");
        }
    }
}
