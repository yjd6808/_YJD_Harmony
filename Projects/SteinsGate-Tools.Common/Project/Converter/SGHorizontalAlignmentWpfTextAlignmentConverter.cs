/*
 * 작성자: 윤정도
 * 생성일: 3/14/2023 3:49:42 PM
 *
 */

using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

using SGToolsCommon.Primitive;

namespace SGToolsCommon.Converter
{
    public class SGHorizontalAlignmentWpfTextAlignmentConverter : IValueConverter
    {
        public static readonly SGHorizontalAlignmentWpfTextAlignmentConverter Instance = new();

        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_value is HAlignment alignment)
            {
                switch (alignment)
                {
                    case HAlignment.Right: return TextAlignment.Right;
                    case HAlignment.Center: return TextAlignment.Center;
                    case HAlignment.Left: return TextAlignment.Left;
                }
            }

            throw new Exception("소스가 HAlignment 타입이 아닙니다.");
        }

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            return null;
        }
    }
}
