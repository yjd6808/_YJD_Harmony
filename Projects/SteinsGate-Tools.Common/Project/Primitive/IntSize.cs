/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:59:35 PM
 *
 */


using SGToolsCommon.Primitive.Converter;
using System.ComponentModel;
using System.Windows;


namespace SGToolsCommon.Primitive
{

    [TypeConverter(typeof(IntSizeConverter))]
    public struct IntSize
    {
        public int Width { get; set; }
        public int Height { get; set; }

        public IntSize()
        {
            Width = 0;
            Height = 0;
        }

        public IntSize(int width, int height)
        {
            Width = width;
            Height = height;
        }

        public IntSize(double width, double height)
        {
            Width = (int)width;
            Height = (int)height;
        }

        public static implicit operator Size(IntSize p)
        {
            return new Size(p.Width, p.Height);
        }

        public static implicit operator IntSize(Size p)
        {
            return new IntSize((int)p.Width, (int)p.Height);
        }

        public Size ToSize()
        {
            return new Size(Width, Height);
        }

        public override string ToString()
        {
            return $"{Width},{Height}";
        }
    }
}
