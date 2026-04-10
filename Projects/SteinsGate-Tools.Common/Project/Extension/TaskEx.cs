/*
 * 작성자: 윤정도
 * 생성일: 3/11/2023 10:07:09 AM
 *
 */

using System;
using System.Threading.Tasks;

namespace SGToolsCommon.Extension
{
    public static class TaskEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        // https://github.com/johnthiriet/AsyncVoid/blob/af772360567db5d4869f1af69aafc86f0ea83787/AsyncVoid/AsyncVoid/TaskUtilities.cs
        public static async void FireAndForgetSafeAsync(this Task _task, Action<Exception> _errorHandler = null)
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
    }
}
