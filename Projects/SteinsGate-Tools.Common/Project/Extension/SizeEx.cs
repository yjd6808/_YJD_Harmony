/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 7:31:52 AM
 *
 */

using System.Windows;

using SGToolsCommon.Primitive;

namespace SGToolsCommon.Extension
{
    public static class SizeEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        public static string ToFullString(this IntSize _size)
            => $"{_size.Width} {_size.Height}";

        //////////////////////////////////////////////////////////////////////////////////
        public static IntSize ParseFullString(string _s)
        {
            int[] sizeArr = new int[2];
            StringEx.ParseIntNumberN(_s, sizeArr);
            return new IntSize(sizeArr[0], sizeArr[1]);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Size Add(Size _size, Vector _vec)
        {
            return new Size(_size.Width + _vec.X, _size.Height + _vec.Y);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Size Add(Size _size, Size _sizeOther)
        {
            return new Size(_size.Width + _sizeOther.Width, _size.Height + _sizeOther.Height);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Size Add(Size _size, Point _pt)
        {
            return new Size(_size.Width + _pt.X, _size.Height + _pt.Y);
        }
    }
}
