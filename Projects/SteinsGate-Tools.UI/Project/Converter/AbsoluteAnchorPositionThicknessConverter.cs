/*
 * 작성자: 윤정도
 * 생성일: 3/8/2023 11:32:33 AM
 *
 */

using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;
using SGToolsCommon.Primitive;

namespace SGToolsUI.Converter
{
    public class AbsoluteAnchorPositionThicknessConverter : IValueConverter
    {
        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            Point pt = new Point();

            if (_value is Point)
                pt = (Point)_value;
            else if (_value is IntPoint)
                pt = (IntPoint)_value;

            pt.X -= Constant.CanvasAnchorSize / 2.0;
            pt.Y -= Constant.CanvasAnchorSize / 2.0;

            Thickness thickness = new Thickness();
            thickness.Left = pt.X;
            thickness.Top = pt.Y;
            return thickness;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new NotImplementedException();
        }
    }
}
