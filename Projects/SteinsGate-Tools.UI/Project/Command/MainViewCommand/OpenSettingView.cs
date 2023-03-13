/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 1:41:46 AM
 *
 */

using SGToolsUI.ViewModel;
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
using SGToolsUI.Model;
using SGToolsUI.View;

namespace SGToolsUI.Command.MainViewCommand
{
    public class OpenSettingView : MainCommandAbstract
    {

        public OpenSettingView(MainViewModel viewModel)
            : base(viewModel, "설정 창을 엽니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            SettingView settingView = new SettingView();
            settingView.ShowDialog();

            if (settingView.Result)
                ViewModel.Setting = settingView.ViewModel.Model.Clone() as Setting;
        }
    }
}
