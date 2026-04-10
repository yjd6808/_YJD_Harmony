/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:54:52 PM
 *
 */

using System;
using System.Windows.Media;

namespace SGToolsCommon.Extension
{
    public static class ColorEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        public static string ToFullString4B(this Color _color)
            => $"{_color.R} {_color.G} {_color.B} {_color.A}";

        //////////////////////////////////////////////////////////////////////////////////
        public static Color ParseFullString4B(string _fullstr)
        {
            int[] n = new int[4];
            StringEx.ParseIntNumberN(_fullstr, n);
            return Color.FromArgb((Byte)n[3], (Byte)n[0], (Byte)n[1], (Byte)n[2]);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Color AddAllWithoutAlpha(this Color _me, byte _all)
        {
            _me.R = _me.R + _all > byte.MaxValue ? byte.MaxValue : (byte)(_me.R + _all);
            _me.G = _me.G + _all > byte.MaxValue ? byte.MaxValue : (byte)(_me.G + _all);
            _me.B = _me.B + _all > byte.MaxValue ? byte.MaxValue : (byte)(_me.B + _all);
            return _me;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Color Add(this Color _me, byte _r, byte _g, byte _b, byte _a)
        {
            _me.R = _me.R + _r > byte.MaxValue ? byte.MaxValue : (byte)(_me.R + _r);
            _me.G = _me.G + _g > byte.MaxValue ? byte.MaxValue : (byte)(_me.G + _g);
            _me.B = _me.B + _b > byte.MaxValue ? byte.MaxValue : (byte)(_me.B + _b);
            _me.A = _me.A + _a > byte.MaxValue ? byte.MaxValue : (byte)(_me.A + _a);
            return _me;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Color Add(this Color _me, byte _r, byte _g, byte _b)
        {
            _me.R = _me.R + _r > byte.MaxValue ? byte.MaxValue : (byte)(_me.R + _r);
            _me.G = _me.G + _g > byte.MaxValue ? byte.MaxValue : (byte)(_me.G + _g);
            _me.B = _me.B + _b > byte.MaxValue ? byte.MaxValue : (byte)(_me.B + _b);
            return _me;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Color AddA(this Color _me, byte _a)
        {
            _me.A = _me.A + _a > byte.MaxValue ? byte.MaxValue : (byte)(_me.A + _a);
            return _me;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Color AddR(this Color _me, byte _r)
        {
            _me.R = _me.R + _r > byte.MaxValue ? byte.MaxValue : (byte)(_me.R + _r);
            return _me;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Color AddG(this Color _me, byte _g)
        {
            _me.G = _me.G + _g > byte.MaxValue ? byte.MaxValue : (byte)(_me.G + _g);
            return _me;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static Color AddB(this Color _me, byte _b)
        {
            _me.B = _me.B + _b > byte.MaxValue ? byte.MaxValue : (byte)(_me.B + _b);
            return _me;
        }
    }
}
