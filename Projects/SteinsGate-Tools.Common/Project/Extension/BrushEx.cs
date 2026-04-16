// jdyun 24/10/06(일)
using System;
using System.Collections.Generic;
using System.Windows.Media;

namespace SGToolsCommon.Extension
{
    public static class BrushEx
    {
        private static readonly Dictionary<uint, Brush> cahedBrushes_ = new();

        public static Brush FromHex(uint _hexVal)
        {
            if (cahedBrushes_.TryGetValue(_hexVal, out Brush? brush))
                return brush;

            byte r = (byte)((_hexVal >> 24) & 0xFF);
            byte g = (byte)((_hexVal >> 16) & 0xFF);
            byte b = (byte)((_hexVal >> 8) & 0xFF);
            byte a = (byte)(_hexVal & 0xFF);

            brush = new SolidColorBrush(Color.FromArgb(a, r, g, b));
            cahedBrushes_.Add(_hexVal, brush);
            return brush;
        }

        public static Brush? FromHexString(string _hexString)
        {
            throw new NotImplementedException();
        }

        public static Brush FromColor(Color _color)
        {
            uint hexVal = 0;
            hexVal |= (uint)_color.R << 24;
            hexVal |= (uint)_color.G << 16;
            hexVal |= (uint)_color.B << 8;
            hexVal |= (uint)_color.A;
            if (cahedBrushes_.TryGetValue(hexVal, out Brush? brush))
                return brush;

            brush = new SolidColorBrush(_color);
            cahedBrushes_.Add(hexVal, brush);
            return brush;
        }
    }
}
