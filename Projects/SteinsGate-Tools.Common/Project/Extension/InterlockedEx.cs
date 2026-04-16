// jdyun 24/04/10(수)
using System;
using System.Threading;
using static SGToolsCommon.Extension.InterlockedEx;

namespace SGToolsCommon.Extension
{
    public static class InterlockedEx
    {
        public static class Int
        {
            public static int Get(ref int _target)
            {
                return Interlocked.CompareExchange(ref _target, 0, 0);
            }

            public static int Set(ref int _target, int _value)
            {
                return Interlocked.Exchange(ref _target, _value);
            }

            public static int Inc(ref int _target)
            {
                return Interlocked.Increment(ref _target);
            }

            public static int Dec(ref int _target)
            {
                return Interlocked.Decrement(ref _target);
            }

            public static int Add(ref int _target, int _value)
            {
                return Interlocked.Add(ref _target, _value);
            }

            public static int Cas(ref int _target, int _comparand, int _value)
            {
                return Interlocked.CompareExchange(ref _target, _value, _comparand);
            }

            public static bool GCas(ref int _target, int _comparand, int _value)
            {
                for (;;)
                {
                    int old = Get(ref _target);
                    if (old > _comparand)
                    {
                        if (Cas(ref _target, _comparand, _value) == old)
                            return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }

        public static class Bool
        {
            public static bool Get(ref int _target)
            {
                return Convert.ToBoolean(Interlocked.CompareExchange(ref _target, 1, 1));
            }

            public static bool Set(ref int _target, bool _value)
            {
                if (_value)
                    return Interlocked.CompareExchange(ref _target, 1, 0) == 0;

                return Interlocked.CompareExchange(ref _target, 0, 1) != 0;
            }

            public static bool Cas(ref int _target, bool _comparand, bool _value)
            {
                int comparand = _comparand ? 1 : 0;
                int value = _value ? 1 : 0;
                int result = Interlocked.CompareExchange(ref _target, value, comparand);
                return Convert.ToBoolean(result);
            }
        }
    }

    public class AtomicBool
    {
        private int value_;
        public bool Value
        {
            get => Bool.Get(ref value_);
            set => Bool.Set(ref value_, value);
        }

        public AtomicBool(bool _value)
        {
            value_ = _value ? 1 : 0;
        }
    }
}
