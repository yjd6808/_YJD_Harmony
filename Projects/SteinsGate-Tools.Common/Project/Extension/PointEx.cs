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
using SGToolsCommon.Primitive;

namespace SGToolsCommon.Extension
{
    public static partial class PointEx
    {
        public static readonly Point Zero = new (0, 0);

        public static Point ToPoint(this Vector vector)
        {
            return new global::System.Windows.Point(vector.X, vector.Y);
        }

        public static Vector ToVector(this Point point)
        {
            return new Vector(point.X, point.Y);
        }

        public static string ToFullString(this Vector vector)
        {
            return $"{vector.X} {vector.Y:F0}";
        }

        public static string ToFullString(this Point point)
        {
            return $"{(int)point.X} {(int)point.Y}";
        }

        public static double Distance(this Point point, Point other)
        {
            return Point.Subtract(point, other).Length;
        }
        public static Point ParseFullString(string s)
        {
            int[] sizeArr = new int[2];
            StringEx.ParseIntNumberN(s, sizeArr);
            return new Point(sizeArr[0], sizeArr[1]);
        }

       

    }


}
