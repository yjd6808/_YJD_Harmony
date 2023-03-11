/*
 * 작성자: 윤정도
 * 생성일: 3/11/2023 10:07:09 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace SGToolsCommon.Extension
{

    
    public static class TaskEx
    {
        // https://github.com/johnthiriet/AsyncVoid/blob/af772360567db5d4869f1af69aafc86f0ea83787/AsyncVoid/AsyncVoid/TaskUtilities.cs
        public static async void FireAndForgetSafeAsync(this Task task, Action<Exception> errorHandler = null)
#pragma warning restore RECS0165 // Asynchronous methods should return a Task instead of void
        {
            try
            {
                await task;
            }
            catch (Exception ex)
            {
                errorHandler?.Invoke(ex);
            }
        }
    }
}
