/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 12:39:38 AM
 *
 */

using System.Windows;

using SGToolsCommon.Primitive;

namespace SGToolsCommon.Extension
{
    public static partial class PointEx
    {
        public static readonly Point Zero = new(0, 0);

        //////////////////////////////////////////////////////////////////////////////////
        public static Point ToPoint(this Vector _vector)
        {
            return new Point(_vector.X, _vector.Y);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Vector ToVector(this Point _point)
        {
            return new Vector(_point.X, _point.Y);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static string ToFullString(this Vector _vector)
        {
            return $"{_vector.X:F0} {_vector.Y:F0}";
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static string ToFullString(this Point _point)
        {
            return $"{(int)_point.X} {(int)_point.Y}";
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static double Distance(this Point _point, Point _other)
        {
            return Point.Subtract(_point, _other).Length;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Point ParseFullString(string _s)
        {
            int[] sizeArr = new int[2];
            StringEx.ParseIntNumberN(_s, sizeArr);
            return new Point(sizeArr[0], sizeArr[1]);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static IntPoint ToIntPoint(this IntVector _vector)
        {
            return new IntPoint(_vector.X, _vector.Y);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static IntVector ToIntVector(this IntPoint _point)
        {
            return new IntVector(_point.X, _point.Y);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static string ToFullString(this IntVector _vector)
        {
            return $"{_vector.X} {_vector.Y}";
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static string ToFullString(this IntPoint _point)
        {
            return $"{_point.X} {_point.Y}";
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static double Distance(this IntPoint _point, IntPoint _other)
        {
            return _point.Distance(_other);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static IntPoint ParseFullStringIntPoint(string _s)
        {
            int[] sizeArr = new int[2];
            StringEx.ParseIntNumberN(_s, sizeArr);
            return new IntPoint(sizeArr[0], sizeArr[1]);
        }
    }
}
