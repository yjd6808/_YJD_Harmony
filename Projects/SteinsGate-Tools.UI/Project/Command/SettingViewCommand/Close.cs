/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 1:12:41 AM
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

namespace SGToolsUI.Command.SettingViewCommand
{
    public class Close : SettingCommandAbstract
    {
        public Close(SettingViewModel viewModel)
            : base(viewModel, "설정을 저장하지 않고 윈도우를 닫습니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            ViewModel.View.Result = false;
            ViewModel.View.Close();
        }
    }
}
