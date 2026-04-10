/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:59:35 PM
 *
 */

using System;
using System.Windows;

namespace SGToolsCommon.Primitive
{
    public struct IntVector
    {
        public static readonly IntVector Zero = new();

        public int X { get; set; }
        public int Y { get; set; }

        public double Length => Math.Sqrt(Math.Pow(X, 2) + Math.Pow(Y, 2));

        //////////////////////////////////////////////////////////////////////////////////
        public IntVector()
        {
            X = 0;
            Y = 0;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public IntVector(int _x, int _y)
        {
            X = _x;
            Y = _y;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static implicit operator Vector(IntVector _p)
        {
            return new Vector(_p.X, _p.Y);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static implicit operator IntVector(Vector _p)
        {
            return new IntVector((int)_p.X, (int)_p.Y);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static IntVector Add(IntVector _lhs, IntVector _rhs)
        {
            _lhs.X += _rhs.X;
            _lhs.Y += _rhs.Y;
            return _lhs;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public Vector ToPoint()
        {
            return new Vector(X, Y);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override string ToString()
        {
            return $"{X} {Y}";
        }
    }
}
