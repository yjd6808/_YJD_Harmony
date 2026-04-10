/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 12:25:28 AM
 *
 */

using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.SettingViewCommand
{
    public class SaveAndClose : SettingCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public SaveAndClose(SettingViewModel _viewModel)
            : base(_viewModel, "설정을 저장하고 윈도우를 닫습니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            ViewModel.Model.Save();
            ViewModel.View.Result = true;
            ViewModel.View.Close();
        }
    }
}
