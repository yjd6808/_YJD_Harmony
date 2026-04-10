/*
 * 작성자: 윤정도
 * 생성일: 3/11/2023 9:59:59 AM
 *
 */

using System;
using System.Threading.Tasks;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Command
{
    public abstract class CommandAsyncAbstract : CommandAbstract
    {
        private Action<Exception>? errorHandler_;

        //////////////////////////////////////////////////////////////////////////////////
        public CommandAsyncAbstract(string _description, Action<Exception>? _errorHandler = null) : base(_description)
        {
            errorHandler_ = _errorHandler;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public virtual async Task ExecuteAsync(object? _parameter)
        {
            // 상속받아서 구현
            throw new NotImplementedException("구현이 안되었어요");
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            if (CanExecute(_parameter))
            {
                ExecuteAsync(_parameter).FireAndForgetSafeAsync(errorHandler_);
            }
        }
    }
}
