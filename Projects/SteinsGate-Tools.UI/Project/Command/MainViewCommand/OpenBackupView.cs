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
    public class OpenBackupView : MainCommandAbstract
    {

        public OpenBackupView(MainViewModel viewModel)
            : base(viewModel, "백업 뷰를 보여주거나 숨깁니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            if (ViewModel.BackupView.IsVisible)
                ViewModel.BackupView.Visibility = Visibility.Collapsed;
            else
                ViewModel.BackupView.Show();

        }
    }
}
