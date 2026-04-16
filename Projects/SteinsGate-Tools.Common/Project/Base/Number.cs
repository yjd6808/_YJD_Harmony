using System;

namespace SGToolsCommon.Base
{
    public enum NumberType
    {
        None,
        Int32,
        Int64,
        Int32U,
        Int64U,
        Single,
        Double,
    }

    public enum NumberStyle
    {
        None,
        Binary,
        Hex,
        Decimal,
    }

    public interface INumber
    {
        public NumberType Type { get; }
        public int ToInt32();
        public uint ToUInt32();
        public long ToInt64();
        public ulong ToUInt64();
        public float ToSingle();
        public double ToDouble();
    }

    /// <summary>
    /// 타입 박싱 없는 제네릭 숫자 래퍼.
    /// </summary>
    public class Number<T> : INumber
    {
        private T value_ = default!;
        private NumberType type_ = NumberType.None;

        public T Value => value_;
        public NumberType Type => type_;

        public Number() {}
        public Number(T _value)
        {
            Set(_value);
        }

        public void Set(T _value)
        {
            value_ = _value;

            if (typeof(T) == typeof(int))
                type_ = NumberType.Int32;
            else if (typeof(T) == typeof(uint))
                type_ = NumberType.Int32U;
            else if (typeof(T) == typeof(long))
                type_ = NumberType.Int64;
            else if (typeof(T) == typeof(ulong))
                type_ = NumberType.Int64U;
            else if (typeof(T) == typeof(float))
                type_ = NumberType.Single;
            else if (typeof(T) == typeof(double))
                type_ = NumberType.Double;
            else
                throw new Exception("지원하지 않는 숫자 타입");
        }

        public int ToInt32()
        {
            try { return Convert.ToInt32(value_); }
            catch { return 0; }
        }

        public uint ToUInt32()
        {
            try { return Convert.ToUInt32(value_); }
            catch { return 0; }
        }

        public long ToInt64()
        {
            try { return Convert.ToInt64(value_); }
            catch { return 0; }
        }

        public ulong ToUInt64()
        {
            try { return Convert.ToUInt64(value_); }
            catch { return 0; }
        }

        public float ToSingle()
        {
            try { return Convert.ToSingle(value_); }
            catch { return 0; }
        }

        public double ToDouble()
        {
            try { return Convert.ToDouble(value_); }
            catch { return 0; }
        }

        public override string ToString()
        {
            return value_?.ToString() ?? string.Empty;
        }
    }
}
