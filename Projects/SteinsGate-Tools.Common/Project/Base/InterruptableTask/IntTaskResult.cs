using System;
using System.Threading;

namespace SGToolsCommon.Base.InterruptableTask
{
    public class IntCTS
    {
        public CancellationTokenSource Source { get; } = new();
        public object Value { get; set; } = null!;
    }

    public class IntTaskResult
    {
        public IntTaskResult(Exception? _exception = null)
        {
            Exception = _exception;
        }

        public Exception? Exception;
    }
}
