// jdyun 24/04/13(토)
using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;

namespace SGToolsCommon.Extension
{
    public static class ObjectEx
    {
        // https://stackoverflow.com/questions/1749966/c-sharp-how-to-determine-whether-a-type-is-a-number
        public static bool IsNumericType(this object _o)
        {
            switch (Type.GetTypeCode(_o.GetType()))
            {
                case TypeCode.Byte:
                case TypeCode.SByte:
                case TypeCode.UInt16:
                case TypeCode.UInt32:
                case TypeCode.UInt64:
                case TypeCode.Int16:
                case TypeCode.Int32:
                case TypeCode.Int64:
                case TypeCode.Decimal:
                case TypeCode.Double:
                case TypeCode.Single:
                    return true;
                default:
                    return false;
            }
        }

        public static bool IsIntegerType(this object _o)
        {
            switch (Type.GetTypeCode(_o.GetType()))
            {
                case TypeCode.Byte:
                case TypeCode.SByte:
                case TypeCode.UInt16:
                case TypeCode.UInt32:
                case TypeCode.UInt64:
                case TypeCode.Int16:
                case TypeCode.Int32:
                case TypeCode.Int64:
                    return true;
                default:
                    return false;
            }
        }

        public static bool IsBoolType(this object _o)
        {
            switch (Type.GetTypeCode(_o.GetType()))
            {
                case TypeCode.Boolean:
                    return true;
                default:
                    return false;
            }
        }

        public static long ToLong(this object _o)
        {
            if (_o == null)
            {
                throw new ArgumentNullException(nameof(_o));
            }

            switch (Type.GetTypeCode(_o.GetType()))
            {
                case TypeCode.Byte:
                    return (byte)_o;
                case TypeCode.SByte:
                    return (sbyte)_o;
                case TypeCode.UInt16:
                    return (ushort)_o;
                case TypeCode.UInt32:
                    return (uint)_o;
                case TypeCode.UInt64:
                    // ulong을 long으로 변환할 때는 범위를 초과할 수 있으므로 주의
                    ulong ulongValue = (ulong)_o;
                    if (ulongValue > long.MaxValue)
                    {
                        throw new OverflowException("값이 long 범위를 초과합니다.");
                    }
                    return (long)ulongValue;
                case TypeCode.Int16:
                    return (short)_o;
                case TypeCode.Int32:
                    return (int)_o;
                case TypeCode.Int64:
                    return (long)_o;
                case TypeCode.String:
                    if (long.TryParse((string)_o, out long result))
                    {
                        return result;
                    }
                    throw new FormatException("문자열을 long으로 변환할 수 없습니다.");
                default:
                    throw new InvalidCastException("지원되지 않는 타입입니다.");
            }
        }

        // https://stackoverflow.com/questions/4223589/does-properties-increase-memory-size-of-instances
        // 인스턴스 메모리 크기 계산
        public static double SizeOf<T>(this object _o, int _testCount = 10000) where T : new()
        {
            object[] array = new object[_testCount];
            long initialMemory = GC.GetTotalMemory(true);

            for (int i = 0; i < _testCount; i++)
                array[i] = new T();

            long finalMemory = GC.GetTotalMemory(true);
            GC.KeepAlive(array);
            long total = finalMemory - initialMemory;
            return (double)total / _testCount;
        }

        public static void ThrowIfNull(object? _value)
        {
            if (_value == null)
                throw new Exception("널이면 안되는값이 널임");
        }

        public static bool IsArrayType(this object _val)
        {
            return _val.GetType().IsArray;
        }

        public static bool IsEnumerable(this object _val)
        {
            return _val is IEnumerable;
        }

        public static bool HasProperty(this object _dyn, string _name)
        {
            if (_dyn is ExpandoObject)
                return ((IDictionary<string, object>)_dyn).ContainsKey(_name);

            return _dyn.GetType().GetProperty(_name) != null;
        }

        public static int GetArraySize(this object _obj)
        {
            if (_obj is IEnumerable enumerable)
            {
                return enumerable.GetLength();
            }
            return 0;
        }

        public static byte[] ToByteArray<T>(this T _obj) where T : struct
        {
            int len = Marshal.SizeOf(_obj);
            byte[] arr = new byte[len];
            IntPtr ptr = Marshal.AllocHGlobal(len);
            Marshal.StructureToPtr(_obj, ptr, true);
            Marshal.Copy(ptr, arr, 0, len);
            Marshal.FreeHGlobal(ptr);
            return arr;
        }

        public static void CopyToByteArray<T>(this T _obj, byte[] _bytes, int _offset = 0) where T : struct
        {
            byte[] bytes = ToByteArray(_obj);
            Array.Copy(bytes, 0, _bytes, _offset, bytes.Length);
        }

        public static void ToStructure<T>(this byte[] _bytearray, ref T _obj) where T : struct
        {
            int len = Marshal.SizeOf(_obj);
            IntPtr i = Marshal.AllocHGlobal(len);
            Marshal.Copy(_bytearray, 0, i, len);
            _obj = (T)Marshal.PtrToStructure(i, _obj.GetType())!;
            Marshal.FreeHGlobal(i);
        }
    }
}
