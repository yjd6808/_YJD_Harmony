// jdyun 24/04/16(화)
using System;
using System.Globalization;
using System.Windows.Data;

namespace SGToolsCommon.Customize.Converter
{
    public class StringFormatConverter : IMultiValueConverter
    {
        public object Convert(object[] _values, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_values.Length <= 1)
                throw new Exception("또잉? 2개이상 인자를 전달해주세요.");

            if (_values[0] is not string)
                throw new Exception("또잉? 1번째 인자가 문자열이 아닌데용?");

            if (_values.Length == 2)
                return String.Format((string)_values[0], _values[1]);
            if (_values.Length == 3)
                return String.Format((string)_values[0], _values[1], _values[2]);
            if (_values.Length == 4)
                return String.Format((string)_values[0], _values[1], _values[2], _values[3]);

            throw new Exception("또잉.. 너무 많은 인자를 전달했어요.. 추가로 if문을 작성해주세요.");
        }

        public object[] ConvertBack(object _value, Type[] _targetTypes, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }
}
