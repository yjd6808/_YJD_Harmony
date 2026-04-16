// jdyun 24/04/07(일)
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;

namespace SGToolsCommon.Extension
{
    public static class ArrayEx
    {
        public static T[] Create<T>(int _count, Func<T> _genFunc)
        {
            T[] array = new T[_count];
            for (int i = 0; i < _count; ++i)
                array[i] = _genFunc();
            return array;
        }

        public static T[] Create<T>(int _count, T _value)
        {
            T[] array = new T[_count];
            for (int i = 0; i < _count; ++i)
                array[i] = _value;
            return array;
        }

        // _bytes에서 strLen 문자열 길이만큼 문자열로 변환한다.
        public static string ToUniString(this byte[] _bytes, int _strLen)
        {
            if (_bytes.Length < _strLen * 2)
            {
                // 버퍼 크기는 당연히 문자열 길이 x2보다 커야한다.
                Debug.Assert(false, "문자열로 변환할 만큼의 버퍼 크기가 부족함");
                return string.Empty;
            }

            char[] chars = new char[_strLen];
            for (int i = 0; i < _strLen; i++)
            {
                chars[i] = (char)((_bytes[i * 2 + 1] << 8) | _bytes[i * 2]);
            }
            return new string(chars);
        }

        public static byte[] ToManaged(this IntPtr _unmanaged, int _len)
        {
            if (_unmanaged == IntPtr.Zero)
                throw new ArgumentException("Pointer cannot be null.", nameof(_unmanaged));
            if (_len < 0)
                throw new ArgumentOutOfRangeException(nameof(_len), "Length cannot be negative.");

            byte[] managedArray = new byte[_len];
            Marshal.Copy(_unmanaged, managedArray, 0, _len);
            return managedArray;
        }

        public static bool IsNullOrEmpty(this Array? _array)
        {
            return _array == null || _array.Length == 0;
        }

        public static int GetTotalLength(this string[] _strArray)
        {
            int len = 0;
            for (var i = 0; i < _strArray.Length; i++)
            {
                len += _strArray[i].Length;
            }
            return len;
        }

        public static string Combine(this string[] _strArray, char _ch, int _index = -1)
        {
            StringBuilder builder = new StringBuilder(_strArray.GetTotalLength() + _strArray.Length);

            if (_index < 0)
            {
                _index = _strArray.Length - 1;
            }
            else if (_index >= _strArray.Length)
            {
                _index = _strArray.Length - 1;
            }

            for (var i = 0; i <= _index; i++)
            {
                builder.Append(_strArray[i]);
                if (i <= _index - 1)
                {
                    builder.Append(_ch);
                }
            }
            return builder.ToString();
        }

        public static void CopyTo(this Span<byte> _span, int _spanOffset, Span<byte> _bytes, int _bytesOffset, int _count)
        {
            if (_spanOffset + _count > _span.Length)
                throw new ArgumentException("_span + _spanOffet의 크기가 _count보다 작음");
            if (_bytesOffset + _count > _bytes.Length)
                throw new ArgumentException("_bytes + _bytesOffset의 크기가 _count보다 작음");

            _span.Slice(_spanOffset, _count).CopyTo(_bytes.Slice(_bytesOffset, _count));
        }
    }
}
