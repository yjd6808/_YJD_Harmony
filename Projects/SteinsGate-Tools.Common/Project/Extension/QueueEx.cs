// jdyun 24/04/07(일)
using System.Collections.Generic;

namespace SGToolsCommon.Extension
{
    public static class QueueEx
    {
        public static void AddRange<T>(this Queue<T> _queue, IEnumerable<T> _enu)
        {
            foreach (T obj in _enu)
                _queue.Enqueue(obj);
        }
    }
}
