// jdyun 24/08/24(토)
using System;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;

namespace SGToolsCommon.Extension
{
    public static class ActionEx
    {
        public static void TryCatch(this Action _action)
        {
            try
            {
                _action();
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.Message);
            }
        }

        public static T TryUntil<T>(this Func<T> _func, TimeSpan? _duration = null)
        {
            _duration ??= TimeSpan.FromSeconds(5);
            Stopwatch sw = Stopwatch.StartNew();
            while (sw.Elapsed < _duration)
            {
                bool success = false;
                try
                {
                    T r = _func();
                    success = true;
                    return r;
                }
#if DEBUG
                catch (Exception)
                {
                }
#endif
                finally
                {
                    if (!success)
                    {
                        Thread.Sleep(100);
                    }
                }
            }

            throw new HandledException("TryUntil 타임아웃");
        }

        public static async Task<T> TryUntilAsync<T>(this Func<T> _func, int? _count = null, int? _delay = null, int? _duration = null)
        {
            _delay ??= 100;
            _duration ??= 5000;
            _count ??= 100000;
            int tryCount = 0;
            Stopwatch sw = Stopwatch.StartNew();
            while ((int)sw.Elapsed.TotalMilliseconds < _duration && tryCount < _count)
            {
                bool success = false;
                try
                {
                    T r = _func();
                    success = true;
                    return r;
                }
                finally
                {
                    ++tryCount;
                    if (!success)
                    {
                        await Task.Delay(_delay.Value);
                    }
                }
            }

            throw new HandledException("TryUntilAsync 타임아웃");
        }
    }
}
