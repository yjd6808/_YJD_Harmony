/*
 * 작성자: 윤정도
 * 생성일: 3/11/2023 9:59:59 AM
 *
 */

using SGToolsCommon.Extension;
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

namespace SGToolsCommon.Command
{
    public abstract class CommandAsyncAbstract : CommandAbstract
    {
        private Action<Exception> _errorHandler;

        public CommandAsyncAbstract(string description, Action<Exception> errorHandler = null) : base(description)
        {
            _errorHandler = errorHandler;
        }

        public virtual async Task ExecuteAsync(object? parameter)
        {
            // 상속받아서 구현
            throw new NotImplementedException("구현이 안되었어요");
        }

        public override void Execute(object? parameter)
        {
            if (CanExecute(parameter))
            {
                ExecuteAsync(parameter).FireAndForgetSafeAsync(_errorHandler);
            }
        }
    }
}
