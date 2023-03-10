/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 11:55:35 AM
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

namespace SGToolsUI.Command.MainViewCommand
{
    public class FileExportHeader : MainCommandAbstract
    {
        public FileExportHeader(MainViewModel viewModel) 
            : base(viewModel, "UI툴 데이터의 디파인을 추출하여 저장합니다.")
        {
        }

        public override void Execute(object? parameter)
        {

        }
    }
}
