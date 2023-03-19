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
    public struct IntVector
    {
        public static readonly IntVector Zero = new();

        public int X { get; set; }
        public int Y { get; set; }

        public IntVector()
        {
            X = 0;
            Y = 0;
        }

        public IntVector(int x, int y)
        {
            X = x;
            Y = y;
        }

        public static implicit operator Vector(IntVector p)
        {
            return new Vector(p.X, p.Y);
        }

        public static implicit operator IntVector(Vector p)
        {
            return new IntVector((int)p.X, (int)p.Y);
        }

        public Vector ToPoint()
        {
            return new Vector(X, Y);
        }

        public override string ToString()
        {
            return $"{X} {Y}";
        }

        public static IntVector Add(IntVector lhs, IntVector rhs)
        {
            lhs.X += rhs.X;
            lhs.Y += rhs.Y;
            return lhs;
        }

        public double Length => Math.Sqrt(Math.Pow(X, 2) + Math.Pow(Y, 2));
    }
}
