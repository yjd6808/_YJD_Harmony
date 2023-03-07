/*
 * 작성자: 윤정도
 * 생성일: 3/7/2023 1:19:17 PM
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

namespace SGToolsCommon
{
    public class DisposeAction : IDisposable
    {
        public DisposeAction(Action action) => _action = action;

        public void Dispose()
        {
            if (_action == null)
                return;

            _action();
        }

        private Action _action;
    }
}
