/*
 * 작성자: 윤정도
 * 생성일: 10/1/2024 6:03:47 PM
 *
 */

using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace SGToolsCommon.Customize.Converter
{
    public class BoolVisibilityConverter : IValueConverter
    {
        public const string KEY_HIDDEN = "1";
        public static readonly BoolVisibilityConverter Instance = new();
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            if (_value is bool boolValue)
            {
                bool hidden = false;
                string param = _parameter as string ?? string.Empty;

                if (param == KEY_HIDDEN)
                {
                    hidden = true;
                }

                if (hidden) return boolValue ? Visibility.Visible : Visibility.Hidden;
                return boolValue ? Visibility.Visible : Visibility.Collapsed;
            }
            return Visibility.Collapsed;
        }

        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }
}
