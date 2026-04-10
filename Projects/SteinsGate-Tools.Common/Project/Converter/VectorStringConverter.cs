/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 12:14:03 AM
 *
 */

using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;
using System.Windows.Markup;

namespace SGToolsCommon.Converter
{
    public class VectorStringConverter : MarkupExtension, IValueConverter
    {
        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            Vector v = (Vector)_value;
            return $"{v.X:F0} {v.Y:F0}";
        }

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override object ProvideValue(IServiceProvider _serviceProvider)
        {
            return this;
        }
    }
}
