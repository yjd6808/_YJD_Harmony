using System;

namespace SGToolsCommon.Base
{
    /// <summary>
    /// 제네릭 범위 구조체. IsOverlapped, IsContain, Cast 등 범위 연산을 제공한다.
    /// </summary>
    public struct Range<T> where T : struct, IComparable<T>
    {
        public T Begin { get; private set; }
        public T End { get; private set; }

        public Range(T _begin, T _end) : this()
        {
            SetRange(_begin, _end);
        }

        public T Length()
        {
            dynamic begin = Begin;
            dynamic end = End;
            return end - begin;
        }

        public bool IsZero()
        {
            return Begin.CompareTo(default) == 0 && End.CompareTo(default) == 0;
        }

        public bool IsValidRange()
        {
            return End.CompareTo(Begin) >= 0;
        }

        public void SetRange(T _begin, T _end)
        {
            Begin = _begin;
            End = _end;

            if (!IsValidRange())
                throw new ArgumentException("Invalid range: End must be greater than or equal to Begin.");
        }

        public bool IsOverlapped(Range<T> _range)
        {
            if (Begin.CompareTo(_range.End) > 0)
                return false;
            if (End.CompareTo(_range.Begin) < 0)
                return false;
            return true;
        }

        public bool IsContain(T _value)
        {
            return _value.CompareTo(Begin) >= 0 && _value.CompareTo(End) <= 0;
        }

        public bool IsContain(Range<T> _range)
        {
            if (!_range.IsValidRange())
                throw new ArgumentException("Invalid range to check containment.");
            return _range.Begin.CompareTo(Begin) >= 0 && _range.End.CompareTo(End) <= 0;
        }

        public Range<U> Cast<U>() where U : struct, IComparable<U>
        {
            return new Range<U>(
                (U)Convert.ChangeType(Begin, typeof(U)),
                (U)Convert.ChangeType(End, typeof(U)));
        }

        public override string ToString() => $"{Begin}~{End}";

        public string ToStringHex() => $"0x{Begin:x} ~ 0x{End:x}";
    }
}
