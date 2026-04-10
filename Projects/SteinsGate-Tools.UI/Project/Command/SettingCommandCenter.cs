/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 12:22:59 AM
 *
 */

using SGToolsCommon.Command;
using SGToolsUI.Command.SettingViewCommand;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command
{
    public class SettingCommandCenter : CommandCenter
    {
        public SettingViewModel ViewModel { get; }

        // =================================================
        // 커맨드 목록
        // =================================================
        public CommandAbstract SaveAndClose { get; }
        public CommandAbstract Close { get; }

        //////////////////////////////////////////////////////////////////////////////////
        public SettingCommandCenter(SettingViewModel _viewModel)
        {
            ViewModel = _viewModel;

            Add(SaveAndClose = new SaveAndClose(ViewModel));
            Add(Close        = new Close(ViewModel));
        }
    }
}
