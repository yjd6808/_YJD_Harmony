/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 12:28:28 AM
 *
 */

using SGToolsCommon.Command;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command
{
    public abstract class SettingCommandAbstract : CommandAbstract
    {
        public SettingViewModel ViewModel { get; }

        //////////////////////////////////////////////////////////////////////////////////
        public SettingCommandAbstract(SettingViewModel _viewModel, string _description) : base(_description)
        {
            ViewModel = _viewModel;
        }
    }
}
