// jdyun 24/08/24(토)
using System;

namespace SGToolsCommon.Extension
{
    public class HandledException : Exception
    {
        public HandledException(string _message) : base(_message)
        {
        }
    }

    public static class ExceptionEx
    {
        public static void ThrowIfInvalidArgument(bool _exp, string _argName = "")
        {
            if (!_exp)
            {
                throw new ArgumentException("올바르지 않은 인자", _argName);
            }
        }
    }
}
