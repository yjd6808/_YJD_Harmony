/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 12:28:39 AM
 *
 */

using SGToolsCommon.Command;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command
{
    public abstract class MainCommandAbstract : CommandAbstract
    {
        public MainViewModel ViewModel { get; }

        //////////////////////////////////////////////////////////////////////////////////
        public MainCommandAbstract(MainViewModel _viewModel, string _description) : base(_description)
        {
            ViewModel = _viewModel;
        }
    }
}
