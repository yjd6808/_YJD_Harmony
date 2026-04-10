/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 1:12:41 AM
 *
 */

using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.SettingViewCommand
{
    public class Close : SettingCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public Close(SettingViewModel _viewModel)
            : base(_viewModel, "설정을 저장하지 않고 윈도우를 닫습니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            ViewModel.View.Result = false;
            ViewModel.View.Close();
        }
    }
}
