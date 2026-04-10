/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 1:41:46 AM
 *
 */

using SGToolsUI.Model.Setting;
using SGToolsUI.View;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class OpenSettingView : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public OpenSettingView(MainViewModel _viewModel)
            : base(_viewModel, "설정 창을 엽니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            SettingView settingView = new SettingView();
            settingView.ShowDialog();

            if (settingView.Result)
                ViewModel.Setting = settingView.ViewModel.Model.Clone() as Setting;
        }
    }
}
