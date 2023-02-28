/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 12:25:28 AM
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
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.SettingViewCommand
{
    public class SaveAndClose : SettingCommandAbstract
    {
        public SaveAndClose(SettingViewModel viewModel) 
            : base(viewModel, "설정을 저장하고 윈도우를 닫습니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            ViewModel.Model.Save();
            ViewModel.View.Result = true;
            ViewModel.View.Close();
        }
    }
}
