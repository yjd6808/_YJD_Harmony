/*
 * 작성자: 윤정도
 * 생성일: 3/11/2023 10:18:58 AM
 *
 */

using System;
using SGToolsCommon.Command;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command
{
    public class MainCommandAbstractAsync : CommandAsyncAbstract
    {
        public MainViewModel ViewModel { get; }

        //////////////////////////////////////////////////////////////////////////////////
        public MainCommandAbstractAsync(MainViewModel _viewModel, string _description, Action<Exception>? _errorHandler = null)
                : base(_description, _errorHandler == null ? _viewModel.LogErrorHandler : _errorHandler)
        {
            ViewModel = _viewModel;
        }
    }
}
