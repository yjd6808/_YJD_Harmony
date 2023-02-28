/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 6:26:11 AM
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
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;

namespace SGToolsUI.Command.MainViewCommand
{
    public class ReloadPackage : MainCommandAbstract
    {
        public ReloadPackage(MainViewModel viewModel)
            : base(viewModel, "패키지를 모두 언로드 후 다시 로딩홥니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            try
            {
                ViewModel.PackManager.UnloadAll();
                ViewModel.SelectedPackage = new SgaPackage();
                ViewModel.SelectedImage = new SgaImage();

                ViewModel.PackManager.SgaDirectory = ViewModel.Setting.SgaDirectory;
                ViewModel.PackManager.LoadSga(SgaPackageType.Interface);

                ViewModel.ResourceSelectionStatus = string.Empty;
            }
            catch (Exception e)
            {
                MessageBoxEx.ShowTopMost(e);
            }
        }
    }
}
