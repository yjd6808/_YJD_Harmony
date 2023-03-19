/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:59:35 PM
 *
 */


using System;
using System.Windows;
using Vanara.PInvoke;


namespace SGToolsCommon.Primitive
{
    public struct IntPoint
    {
        public static readonly IntPoint Zero = new();

        public int X { get; set; }
        public int Y { get; set; }

        public IntPoint()
        {
            X = 0;
            Y = 0;
        }

        public IntPoint(int x, int y)
        {
            X = x;
            Y = y;
        }

        public static implicit operator Point(IntPoint p)
        {
            return new Point(p.X, p.Y);
        }

        public static implicit operator IntPoint(Point p)
        {
            return new IntPoint((int)p.X, (int)p.Y);
        }

        public Point ToPoint()
        {
            return new Point(X, Y);
        }

        public override string ToString()
        {
            return $"{X} {Y}";
        }

        public static IntPoint Add(IntPoint lhs, IntPoint rhs)
        {
            lhs.X += rhs.X;
            lhs.Y += rhs.Y;
            return lhs;
        }

        public static IntVector Subtract(IntPoint lhs, IntPoint rhs)
        {
            IntVector v = new IntVector();
            v.X = lhs.X - rhs.X;
            v.Y = lhs.Y - rhs.Y;
            return v;
        }

        public static IntPoint Subtract(IntPoint lhs, IntVector rhs)
        {
            IntPoint v = new IntPoint();
            v.X = lhs.X - rhs.X;
            v.Y = lhs.Y - rhs.Y;
            return v;
        }

        public static bool operator==(IntPoint lhs, IntPoint rhs)
        {
            return lhs.X == rhs.X && lhs.Y == rhs.Y;
        }

        public static bool operator !=(IntPoint lhs, IntPoint rhs)
        {
            return !(lhs == rhs);
        }

    public double Distance(IntPoint other)
        {
            return Math.Sqrt(Math.Pow(X - other.X, 2) + Math.Pow(Y - other.Y, 2));
        }
    }
}
