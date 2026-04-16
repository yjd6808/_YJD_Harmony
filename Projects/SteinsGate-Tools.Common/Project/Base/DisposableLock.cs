using System;
using System.Threading;

namespace SGToolsCommon.Base
{
    /// <summary>
    /// using 블록 패턴에서 사용 가능한 Monitor 락 래퍼 struct.
    /// </summary>
    public struct DisposableLock : IDisposable
    {
        private object? syncRoot_;

        public DisposableLock(object _syncRoot)
        {
            syncRoot_ = _syncRoot;
            if (syncRoot_ != null)
                Monitor.Enter(_syncRoot);
        }

        public void Dispose()
        {
            if (syncRoot_ != null)
                Monitor.Exit(syncRoot_);
        }
    }
}
