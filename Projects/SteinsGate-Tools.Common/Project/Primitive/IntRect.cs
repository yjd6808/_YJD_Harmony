/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:59:35 PM
 *
 */


using System.Windows;
using System.Windows.Media.Media3D;


namespace SGToolsCommon.Primitive
{
    public struct IntRect
    {
        public int X { get; set; }
        public int Y { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }

        public IntRect()
        {
            X = 0;
            Y = 0;
            Width = 0;
            Height = 0;
        }

        public IntRect(int x, int y, int width, int height)
        {
            X = x;
            Y = y;
            Width = width;
            Height = height;
        }

        public static implicit operator Rect(IntRect rc)
        {
            return new Rect(rc.X, rc.Y, rc.Width, rc.Height);
        }

        public static implicit operator IntRect(Rect rc)
        {
            return new IntRect((int)rc.X, (int)rc.Y, (int)rc.Width, (int)rc.Height);
        }

        public IntRect ToRect()
        {
            return new Rect((int)X, (int)Y, (int)Width, (int)Height);
        }

        public bool Contains(IntRect otherRect)
        {
            return (otherRect.X >= X && otherRect.X + otherRect.Width <= X + Width &&
                    otherRect.X >= X && otherRect.X + otherRect.Height <= X + Height);
        }

        public override string ToString()
        {
            return $"{X} {Y} {Width} {Height}";
        }
    }
}
