/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 7:28:22 AM
 *
 */

using System.Windows;

namespace SGToolsCommon.Extension
{
    public static class RectEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        // Rect의 Position만 문자열로 바꿈
        public static string ToPositionString(this Rect _rect)
            => $"{(int)_rect.X} {(int)_rect.Y}";

        //////////////////////////////////////////////////////////////////////////////////
        // Rect의 Size만 바꿈
        public static string ToSizeString(this Rect _rect)
            => $"{(int)_rect.Width} {(int)_rect.Height}";

        //////////////////////////////////////////////////////////////////////////////////
        // Rect전체를 바꿈
        public static string ToFullString(this Rect _rect)
            => $"{(int)_rect.X} {(int)_rect.Y} {(int)_rect.Width} {(int)_rect.Height}";

        //////////////////////////////////////////////////////////////////////////////////
        public static Rect From(double _x, double _y, double _width, double _height)
            => new(_x, _y, _width, _height);

        //////////////////////////////////////////////////////////////////////////////////
        public static bool FromContain(double _x, double _y, double _width, double _height, Point _pos)
        {
            if (_pos.X >= _x && _pos.X <= _x + _width &&
                _pos.Y >= _y && _pos.Y <= _y + _height)
                return true;

            return false;
        }
    }
}
