// jdyun 24/04/16(수)
using System;

namespace SGToolsCommon.Extension
{
    public static class LockEx
    {
        public static T Do<T>(object _locker, Func<T> _func)
        {
            if (_locker == null)
                throw new ArgumentNullException();
            lock (_locker)
            {
                return _func();
            }
        }

        public static void Do(object _locker, Action _action)
        {
            if (_locker == null)
                throw new ArgumentNullException();
            lock (_locker)
            {
                _action();
            }
        }

        public static void DoIf(bool _condition, object _locker, Action _action)
        {
            if (_locker == null)
                throw new ArgumentNullException();

            if (_condition)
            {
                lock (_locker)
                {
                    _action();
                }
            }
            else
            {
                _action();
            }
        }

        public static T DoIf<T>(bool _condition, object _locker, Func<T> _func)
        {
            if (_locker == null)
                throw new ArgumentNullException();

            if (_condition)
            {
                lock (_locker)
                {
                    return _func();
                }
            }
            else
            {
                return _func();
            }
        }
    }
}
