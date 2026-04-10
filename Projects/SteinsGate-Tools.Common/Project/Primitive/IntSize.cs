/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:59:35 PM
 *
 */

using System.ComponentModel;
using System.Windows;

using SGToolsCommon.Primitive.Converter;

namespace SGToolsCommon.Primitive
{
    [TypeConverter(typeof(IntSizeConverter))]
    public struct IntSize
    {
        public int Width { get; set; }
        public int Height { get; set; }

        //////////////////////////////////////////////////////////////////////////////////
        public IntSize()
        {
            Width = 0;
            Height = 0;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public IntSize(int _width, int _height)
        {
            Width = _width;
            Height = _height;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public IntSize(double _width, double _height)
        {
            Width = (int)_width;
            Height = (int)_height;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static implicit operator Size(IntSize _p)
        {
            return new Size(_p.Width, _p.Height);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static implicit operator IntSize(Size _p)
        {
            return new IntSize((int)_p.Width, (int)_p.Height);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public Size ToSize()
        {
            return new Size(Width, Height);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override string ToString()
        {
            return $"{Width},{Height}";
        }
    }
}
