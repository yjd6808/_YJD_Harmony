/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 6:26:11 AM
 *
 */

using System;
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class ReloadSgaPackage : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public ReloadSgaPackage(MainViewModel _viewModel)
            : base(_viewModel, "패키지를 모두 언로드 후 다시 로딩홥니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
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
