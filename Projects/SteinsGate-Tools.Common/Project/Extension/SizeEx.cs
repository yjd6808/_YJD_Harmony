/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 7:31:52 AM
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
    public static class SizeEx
    {
        public static string ToFullString(this Size size)
            => $"{(int)size.Width} {(int)size.Height}";

        public static Size ParseFullString(string s)
        {
            int[] sizeArr = new int[2];
            StringEx.ParseIntNumberN(s, sizeArr);
            return new Size(sizeArr[0], sizeArr[1]);
        }

        public static Size Add(Size size, Vector vec)
        {
            return new Size(size.Width + vec.X, size.Height + vec.Y);
        }

        public static Size Add(Size size, Size sizeOther)
        {
            return new Size(size.Width + sizeOther.Width, size.Height + sizeOther.Height);
        }

        public static Size Add(Size size, Point pt)
        {
            return new Size(size.Width + pt.X, size.Height + pt.Y);
        }
    }
}
