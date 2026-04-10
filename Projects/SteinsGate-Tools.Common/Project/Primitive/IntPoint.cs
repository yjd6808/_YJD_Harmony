/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:59:35 PM
 *
 */

using System;
using System.ComponentModel;
using System.Windows;

using SGToolsCommon.Primitive.Converter;

namespace SGToolsCommon.Primitive
{
    [TypeConverter(typeof(IntPointConverter))]
    public struct IntPoint
    {
        public static readonly IntPoint Zero = new();

        public int X { get; set; }
        public int Y { get; set; }

        //////////////////////////////////////////////////////////////////////////////////
        public IntPoint()
        {
            X = 0;
            Y = 0;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public IntPoint(int _x, int _y)
        {
            X = _x;
            Y = _y;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static implicit operator Point(IntPoint _p)
        {
            return new Point(_p.X, _p.Y);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static implicit operator IntPoint(Point _p)
        {
            return new IntPoint((int)_p.X, (int)_p.Y);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static IntPoint Add(IntPoint _lhs, IntPoint _rhs)
        {
            _lhs.X += _rhs.X;
            _lhs.Y += _rhs.Y;
            return _lhs;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static IntVector Subtract(IntPoint _lhs, IntPoint _rhs)
        {
            IntVector v = new IntVector();
            v.X = _lhs.X - _rhs.X;
            v.Y = _lhs.Y - _rhs.Y;
            return v;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static IntPoint Subtract(IntPoint _lhs, IntVector _rhs)
        {
            IntPoint v = new IntPoint();
            v.X = _lhs.X - _rhs.X;
            v.Y = _lhs.Y - _rhs.Y;
            return v;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static bool operator==(IntPoint _lhs, IntPoint _rhs)
        {
            return _lhs.X == _rhs.X && _lhs.Y == _rhs.Y;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static bool operator !=(IntPoint _lhs, IntPoint _rhs)
        {
            return !(_lhs == _rhs);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public Point ToPoint()
        {
            return new Point(X, Y);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public double Distance(IntPoint _other)
        {
            return Math.Sqrt(Math.Pow(X - _other.X, 2) + Math.Pow(Y - _other.Y, 2));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override bool Equals(object? _obj)
        {
            if (_obj == null) return false;
            if (!(_obj is IntPoint)) return false;
            return this == (IntPoint)_obj;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override int GetHashCode()
            => base.GetHashCode();

        //////////////////////////////////////////////////////////////////////////////////
        public override string ToString()
        {
            return $"{X},{Y}";
        }
    }
}
