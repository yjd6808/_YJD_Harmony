/*
 * 작성자: 윤정도
 * 생성일: 3/11/2023 10:07:09 AM
 *
 */

using System;
using System.Threading;
using System.Threading.Tasks;

namespace SGToolsCommon.Extension
{
    public static class TaskEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        // https://github.com/johnthiriet/AsyncVoid/blob/af772360567db5d4869f1af69aafc86f0ea83787/AsyncVoid/AsyncVoid/TaskUtilities.cs
        public static async void FireAndForgetSafeAsync(this Task _task, Action<Exception>? _errorHandler = null)
#pragma warning restore RECS0165 // Asynchronous methods should return a Task instead of void
        {
            try
            {
                await _task;
            }
            catch (Exception ex)
            {
                _errorHandler?.Invoke(ex);
            }
        }

        public static Task StartSafe(this Task _task, Action<Exception>? _errorHandler = null)
        {
            try
            {
                _task.Start();
                return _task;
            }
            catch (Exception ex)
            {
                _errorHandler?.Invoke(ex);
                return _task;
            }
        }

        public static Task<T> StartSafe<T>(this Task<T> _task, Action<Exception>? _errorHandler = null)
        {
            try
            {
                _task.Start();
                return _task;
            }
            catch (Exception ex)
            {
                _errorHandler?.Invoke(ex);
                return _task;
            }
        }

        [global::System.Diagnostics.DebuggerNonUserCode()]
        public static bool TryThrowIfCancellationRequested(this CancellationToken _token)
        {
            try
            {
                _token.ThrowIfCancellationRequested();
                return false;
            }
            catch (OperationCanceledException)
            {
                return true;
            }
        }
    }
}

