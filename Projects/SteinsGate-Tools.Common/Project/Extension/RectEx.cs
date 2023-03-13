/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 7:28:22 AM
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
    public static class RectEx
    {
        // Rect의 Position만 문자열로 바꿈
        public static string ToPositionString(this Rect rect)
            => $"{(int)rect.X} {(int)rect.Y}";

        // Rect의 Size만 바꿈
        public static string ToSizeString(this Rect rect)
            => $"{(int)rect.Width} {(int)rect.Height}";

        // Rect전체를 바꿈
        public static string ToFullString(this Rect rect)
            => $"{(int)rect.X} {(int)rect.Y} {(int)rect.Width} {(int)rect.Height}";


        public static Rect From(double x, double y, double width, double height)
            => new (x, y, width, height);

        public static bool FromContain(double x, double y, double width, double height, Point pos)
        {
            if (pos.X >= x && pos.X <= x + width &&
                pos.Y >= y && pos.Y <= y + height)
                return true;

            return false;
        }

    }
}
