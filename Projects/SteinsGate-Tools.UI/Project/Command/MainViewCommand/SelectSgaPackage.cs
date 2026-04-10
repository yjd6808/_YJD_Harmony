/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 8:46:00 AM
 *
 */

using System;
using System.Diagnostics;
using SGToolsCommon.Sga;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class SelectSgaPackage : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public SelectSgaPackage(MainViewModel _viewModel)
            : base(_viewModel, "SGA 패키지를 선택합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            SgaPackage? selectedPackage = _parameter as SgaPackage;
            if (selectedPackage == null)
                throw new Exception("선택한 리스트박스 아이템은 Sga 패키지 타입이 아닙니다.");

            ViewModel.SelectedPackage = selectedPackage;
            ViewModel.AlbumView.ViewModel.SelectedPackage = selectedPackage;

            ViewModel.SelectedPackage.NotifyUpdateList();

            ViewModel.SelectedImage = new SgaImage();
            ViewModel.AlbumView.ViewModel.SelectedImage = new SgaImage();

            ViewModel.ResourceSelectionStatus = $"{selectedPackage.FileNameWithoutExt}";

            Debug.WriteLine($"${selectedPackage} 패키지 선택");
        }
    }
}
