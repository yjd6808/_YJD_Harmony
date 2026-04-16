// jdyun 25/01/28(화)

using System;
using System.Globalization;
using System.Linq;
using System.Windows.Data;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Customize.Converter
{
    public class BoolOperatorConverter : IMultiValueConverter
    {
        public static readonly BoolOperatorConverter Instance = new();

        public object? Convert(object[] _values, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_values.Length == 0)
                throw new Exception("소스가 전달되지 않음");
            if (!_values.All(_x => _x.IsBoolType()))
                throw new Exception("소스가 모두 불 타입이어야합니다.");
            string ops = string.Empty;
            if (_values.Length >= 2)
            {
                if (_parameter is not string o)
                    throw new Exception("파라미터로 연산 정보가 전달되지 않았습니다.");
                ops = o;
                if (ops.Length + 1 != _values.Length)
                    throw new Exception("연산정보가 일치하지 않습니다. 2개의 소스가 있을 때 1개의 문자 오퍼레이터가 필요합니다. 3개면 2개갰죠?");
                for (int i = 0; i < ops.Length; ++i)
                    if (ops[i] != '0' && ops[i] != '1')
                        throw new Exception("0 또는 1만 전달");
                // 0 = &&
                // 1 = ||
            }

            bool first = (bool)_values[0];
            for (int i = 1; i < _values.Length; ++i)
            {
                if (ops[i - 1] == '0')
                    first = first && (bool)_values[i];
                else if (ops[i - 1] == '1')
                    first = first || (bool)_values[i];
            }
            return first;
        }

        public object[] ConvertBack(object _value, Type[] _targetTypes, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }
}
