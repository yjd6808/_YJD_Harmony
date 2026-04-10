/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:59:35 PM
 *
 */

using System.Windows;

namespace SGToolsCommon.Primitive
{
    public struct IntRect
    {
        public int X { get; set; }
        public int Y { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }
        public int Left => X;
        public int Top => Y;
        public int Right => X + Width;
        public int Bottom => Y + Height;
        public IntPoint Location
        {
            get => new IntPoint(X, Y);
            set { X = value.X; Y = value.Y; }
        }
        public IntSize Size
        {
            get => new IntSize(Width, Height);
            set { Width = value.Width; Height = value.Height; }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public IntRect()
        {
            X = 0;
            Y = 0;
            Width = 0;
            Height = 0;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public IntRect(int _x, int _y, int _width, int _height)
        {
            X = _x;
            Y = _y;
            Width = _width;
            Height = _height;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static implicit operator Rect(IntRect _rc)
        {
            return new Rect(_rc.X, _rc.Y, _rc.Width, _rc.Height);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static implicit operator IntRect(Rect _rc)
        {
            return new IntRect((int)_rc.X, (int)_rc.Y, (int)_rc.Width, (int)_rc.Height);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public IntRect ToRect()
        {
            return new Rect((int)X, (int)Y, (int)Width, (int)Height);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public bool Contains(IntRect _otherRect)
        {
            return (_otherRect.X >= X && _otherRect.X + _otherRect.Width <= X + Width &&
                    _otherRect.Y >= Y && _otherRect.Y + _otherRect.Height <= Y + Height);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override string ToString()
        {
            return $"{X} {Y} {Width} {Height}";
        }
    }
}
