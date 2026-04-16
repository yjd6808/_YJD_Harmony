/*
 * 작성자: 윤정도
 * 생성일: 1/31/2025 5:03:47 PM
 *
 */

using System;
using System.Diagnostics;
using System.Globalization;
using System.Windows.Data;

namespace SGToolsCommon.Customize.Converter
{
    public class BoolOpacityConverter : IValueConverter
    {
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_value is not bool s)
            {
                Debug.Assert(false);
                return 0.0;
            }

            double trueOpacity = 1.0;
            double falseOpacity = 0.0;

            if (_parameter is not string param)
            {
                return s ? trueOpacity : falseOpacity;
            }

            string[] splts = param.Split('|');
            if (splts.Length != 2)
            {
                // 인자 2개 전달 필요
                Debug.Assert(false);
                return s ? trueOpacity : falseOpacity;
            }

            try
            {
                trueOpacity = double.Parse(splts[0]);
                falseOpacity = double.Parse(splts[1]);
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
            return s ? trueOpacity : falseOpacity;
        }

        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }
}
