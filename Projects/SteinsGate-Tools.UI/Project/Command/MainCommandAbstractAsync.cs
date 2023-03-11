/*
 * 작성자: 윤정도
 * 생성일: 3/11/2023 10:18:58 AM
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
using SGToolsCommon.Command;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command
{
    public class MainCommandAbstractAsync : CommandAsyncAbstract
    {
        public MainViewModel ViewModel { get; }

        public MainCommandAbstractAsync(MainViewModel viewModel, string description, Action<Exception> errorHandler = null) 
                : base(description, errorHandler == null ? viewModel.LogErrorHandler : errorHandler)
        {
            ViewModel = viewModel;
        }
    }
}
