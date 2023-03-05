/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 12:39:38 AM
 *
 */

using System;
using System.Collections.Generic;
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

namespace SGToolsCommon.Extension
{
    public static class PointEx
    {
        public static Point ToPoint(this Vector vector)
        {
            return new Point(vector.X, vector.Y);
        }

        public static Vector ToVector(this Point point)
        {
            return new Vector(point.X, point.Y);
        }

        public static string ToSpacedString(this Vector vector)
        {
            return $"{vector.X:F0} {vector.Y:F0}";
        }

        public static string ToSpacedString(this Point point)
        {
            return $"{point.X:F0} {point.Y:F0}";
        }

        public static double Distance(this Point point, Point other)
        {
            return Point.Subtract(point, other).Length;
        }
    }


}
