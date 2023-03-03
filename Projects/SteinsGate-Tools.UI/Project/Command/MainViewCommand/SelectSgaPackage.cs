/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 8:46:00 AM
 *
 */

using SGToolsUI.Model;
using SGToolsUI.View;
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
using SGToolsCommon.Sga;

namespace SGToolsUI.Command.MainViewCommand
{
    public class SelectSgaPackage : MainCommandAbstract
    {

        public SelectSgaPackage(MainViewModel viewModel)
            : base(viewModel, "SGA 패키지를 선택합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            SgaPackage? selectedPackage = parameter as SgaPackage;
            if (selectedPackage == null)
                throw new Exception("선택한 리스트박스 아이템은 Sga 패키지 타입이 아닙니다.");

            ViewModel.SelectedPackage = selectedPackage;
            ViewModel.ResourceSelectionStatus = $"{selectedPackage.FileNameWithoutExt}";
        }
    }
}
