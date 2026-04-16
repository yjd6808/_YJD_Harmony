// jdyun 24/04/13(토)
using System;
using System.Globalization;
using System.Linq;
using System.Windows;
using System.Windows.Data;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Customize.Converter
{
    public class DoubleToMarginConverter : IMultiValueConverter
    {
        public static readonly DoubleToMarginConverter Instance = new();

        public object? Convert(object[] _values, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_values.Length != 1 && _values.Length != 4 && _values.Length != 5)
                throw new Exception("파라미터가 1개 또는 4개 또는 5개 필요합니다.");

            if (_values.Length == 1)
            {
                if (!_values.All(_x => _x.IsNumericType()))
                    throw new Exception("파라미터가 숫자 타입이어야 합니다.");

                double f = System.Convert.ToDouble(_values[0]);
                return new Thickness(f, f, f, f);
            }
            if (_values.Length == 4)
            {
                if (!_values.All(_x => _x.IsNumericType()))
                    throw new Exception("파라미터가 숫자 타입이어야 합니다.");

                double f1 = System.Convert.ToDouble(_values[0]);
                double f2 = System.Convert.ToDouble(_values[1]);
                double f3 = System.Convert.ToDouble(_values[2]);
                double f4 = System.Convert.ToDouble(_values[3]);
                return new Thickness(f1, f2, f3, f4);
            }
            if (_values.Length == 5)
            {
                if (!_values[0].IsNumericType() ||
                    !_values[1].IsNumericType() ||
                    !_values[2].IsNumericType() ||
                    !_values[3].IsNumericType())
                    throw new Exception("1, 2, 3, 4 파라미터가 숫자 타입이어야 합니다.");

                if (_values[4] is not Thickness)
                    throw new Exception("5 파라미터가 Thickness 타입이어야 합니다.");

                double f1 = System.Convert.ToDouble(_values[0]);
                double f2 = System.Convert.ToDouble(_values[1]);
                double f3 = System.Convert.ToDouble(_values[2]);
                double f4 = System.Convert.ToDouble(_values[3]);
                Thickness r = new Thickness(f1, f2, f3, f4);
                Thickness o = (Thickness)_values[4];

                r.Bottom += o.Bottom;
                r.Top += o.Top;
                r.Left += o.Left;
                r.Right += o.Right;
                return r;
            }

            return null;
        }

        public object[] ConvertBack(object _value, Type[] _targetTypes, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }
}
