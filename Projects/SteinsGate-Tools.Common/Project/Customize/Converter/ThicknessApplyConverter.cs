// jdyun 25/01/28(화)
// 띡니스 특정 원소만 적용
// 비트플래그 방식
// 1: left
// 2: top
// 4: right
// 8: bottom
using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace SGToolsCommon.Customize.Converter
{
    public class ThicknessApplyConverter : IValueConverter
    {
        // 기본 방향
        public const int KEY_LEFT = 8;      // 0001
        public const int KEY_TOP = 4;       // 0010
        public const int KEY_RIGHT = 2;     // 0100
        public const int KEY_BOTTOM = 1;    // 1000

        public const int KEY_LEFT_TOP = KEY_LEFT | KEY_TOP;        // 1100
        public const int KEY_LEFT_RIGHT = KEY_LEFT | KEY_RIGHT;    // 1010
        public const int KEY_LEFT_BOTTOM = KEY_LEFT | KEY_BOTTOM;  // 1001
        public const int KEY_TOP_RIGHT = KEY_TOP | KEY_RIGHT;      // 0110
        public const int KEY_TOP_BOTTOM = KEY_TOP | KEY_BOTTOM;    // 0101
        public const int KEY_RIGHT_BOTTOM = KEY_RIGHT | KEY_BOTTOM; // 0011

        public const int KEY_LEFT_TOP_RIGHT = KEY_LEFT | KEY_TOP | KEY_RIGHT;      // 1110
        public const int KEY_LEFT_TOP_BOTTOM = KEY_LEFT | KEY_TOP | KEY_BOTTOM;    // 1101
        public const int KEY_LEFT_RIGHT_BOTTOM = KEY_LEFT | KEY_RIGHT | KEY_BOTTOM; // 1011
        public const int KEY_TOP_RIGHT_BOTTOM = KEY_TOP | KEY_RIGHT | KEY_BOTTOM;  // 0111

        public const int KEY_ALL = KEY_LEFT | KEY_TOP | KEY_RIGHT | KEY_BOTTOM; // 1111

        public static readonly ThicknessApplyConverter Instance = new();

        public object? Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_parameter is not int applyFlag)
                return _value;
            if (_value is not Thickness source)
                return _value;

            Thickness applied = source;

            if ((applyFlag & KEY_LEFT) == 0)
            {
                applied.Left = 0;
            }

            if ((applyFlag & KEY_TOP) == 0)
            {
                applied.Top = 0;
            }

            if ((applyFlag & KEY_RIGHT) == 0)
            {
                applied.Right = 0;
            }

            if ((applyFlag & KEY_BOTTOM) == 0)
            {
                applied.Bottom = 0;
            }

            return applied;
        }

        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }
}
