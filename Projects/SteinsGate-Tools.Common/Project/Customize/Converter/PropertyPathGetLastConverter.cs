// jdyun 24/08/30 프로퍼티 패쓰 라스트 아이템 가져옴

using System;
using System.Globalization;
using System.Windows.Data;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Customize.Converter
{
    public class PropertyPathGetLastConverter : IValueConverter
    {
        public static readonly PropertyPathGetLastConverter Instance = new();

        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_value is not string str)
                throw new Exception("문자열 개체가 아닙니다.");

            string[] props = str.SplitPropertyPath();
            if (props.Length == 0)
                return "";
            return props[props.Length - 1];
        }

        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException("구현이 안되었어요");
        }
    }
}
