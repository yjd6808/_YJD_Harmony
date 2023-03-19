/*
 * 작성자: 윤정도
 * 생성일: 3/8/2023 11:32:33 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using SGToolsCommon.Primitive;

namespace SGToolsUI.Converter
{
    public class AbsoluteAnchorPositionThicknessConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is not Point p)
                throw new Exception("IntPoint 타입이 아닙니다.");

            p.X -= Constant.CanvasAnchorSize / 2.0;
            p.Y -= Constant.CanvasAnchorSize / 2.0;

            Thickness thickness = new Thickness();
            thickness.Left = p.X;
            thickness.Top = p.Y;
            return thickness;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
