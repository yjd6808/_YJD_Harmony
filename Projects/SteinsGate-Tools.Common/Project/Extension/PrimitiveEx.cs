// jdyun 24/04/20(일)
using System;
using System.Linq;
using System.Text;

namespace SGToolsCommon.Extension
{
    public static class PrimitiveEx
    {
        public static bool IsKorean(this char _ch)
        {
            // 조합 || 초성 모음
            return (0xAC00 >= _ch && _ch <= 0xD7A3) || (0x3131 <= _ch && _ch <= 0x318E);
        }

        public static bool IsEnglish(this char _ch)
        {
            return ('a' <= _ch && _ch <= 'z') || ('A' <= _ch && _ch <= 'Z');
        }

        public static bool IsNumeric(this char _ch)
        {
            return 0x30 <= _ch && _ch <= 0x39;
        }

        public static ulong ToInt64U(this IntPtr _ptr)
        {
            return (ulong)_ptr.ToInt64();
        }

        public static IntPtr ToPointer(this ulong _value)
        {
            return new IntPtr((long)_value);
        }

        public static IntPtr Add(this IntPtr _ptr, uint _value)
        {
            return new IntPtr(_ptr.ToInt64() + _value);
        }

        public static IntPtr Add(this IntPtr _ptr, int _value)
        {
            return new IntPtr(_ptr.ToInt64() + _value);
        }

        public static IntPtr Add(this IntPtr _ptr, ushort _value)
        {
            return new IntPtr(_ptr.ToInt64() + _value);
        }

        public static IntPtr Sub(this IntPtr _ptr, uint _value)
        {
            return new IntPtr(_ptr.ToInt64() - _value);
        }

        public static IntPtr Sub(this IntPtr _ptr, ushort _value)
        {
            return new IntPtr(_ptr.ToInt64() - _value);
        }

        public static uint Sub(this IntPtr _ptr, IntPtr _value)
        {
            return (uint)(_ptr.ToInt64() - _value.ToInt64());
        }

        public static bool IsLineBreak(this char _ch) => _ch == '\n'; // line feed (0xA)
        public static bool IsSpace(this char _ch) => _ch == ' ' || _ch == '\t' || _ch == '\r';
        public static bool IsAlphabat(this char _ch) => (_ch >= 'a' && _ch <= 'z') || (_ch >= 'A' && _ch <= 'Z');
        public static bool IsBinary(this char _ch) => _ch == '0' || _ch == '1';
        public static bool IsDigit(this char _ch) => _ch >= '0' && _ch <= '9';
        public static bool IsHexaDecimal(this char _ch) => (_ch >= '0' && _ch <= '9') || (_ch >= 'A' && _ch <= 'F') || (_ch >= 'a' && _ch <= 'f');

        public static string RepeatString(this int _num, string _str)
        {
            if (string.IsNullOrEmpty(_str))
                return string.Empty;
            if (_num <= 0)
                return string.Empty;
            return string.Concat(Enumerable.Repeat(_str, _num));
        }

        public static bool HasFlag(this int _flag, int _value)
        {
            return (_flag & _value) == _value;
        }
        public static bool HasFlag(this uint _flag, uint _value)
        {
            return (_flag & _value) == _value;
        }
        public static bool HasFlag(this long _flag, long _value)
        {
            return (_flag & _value) == _value;
        }
        public static bool HasFlag(this ulong _flag, ulong _value)
        {
            return (_flag & _value) == _value;
        }

        public static string ToHexString(this byte[] _bytes)
        {
            if (_bytes == null || _bytes.Length == 0)
                return string.Empty;

            var hexChars = StringEx.HEX_CHAR;
            var sb = new StringBuilder(_bytes.Length * 2);
            for (int i = 0; i < _bytes.Length; i++)
            {
                sb.Append(hexChars[(_bytes[i] >> 4) & 0xF]);
                sb.Append(hexChars[_bytes[i] & 0xF]);
            }
            return sb.ToString();
        }

        public static string ToHexString(this Span<byte> _bytes)
        {
            if (_bytes == null || _bytes.Length == 0)
                return string.Empty;

            var hexChars = StringEx.HEX_CHAR;
            var sb = new StringBuilder(_bytes.Length * 2);
            for (int i = 0; i < _bytes.Length; i++)
            {
                sb.Append(hexChars[(_bytes[i] >> 4) & 0xF]);
                sb.Append(hexChars[_bytes[i] & 0xF]);
            }
            return sb.ToString();
        }
    }
}
