/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 11:32:17 AM
 *
 */

using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Media.Imaging;

namespace SGToolsCommon.Customize.Converter
{
    public class PackUrlImageSourceConverter : IValueConverter
    {
        public static readonly PackUrlImageSourceConverter Instance = new();

        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            return new BitmapImage(new Uri((string)_parameter));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException("구현이 안되었어요");
        }
    }
}
