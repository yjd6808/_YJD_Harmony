/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 9:10:29 AM
 *
 */

using SGToolsCommon.Sga;
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

namespace SGToolsUI.Command.MainViewCommand
{
    public class SelectElement : MainCommandAbstract
    {

        public SelectElement(MainViewModel viewModel)
            : base(viewModel, "SGA 엘리멘트를 선택합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            SgaElementHeader? selectedElementHeader = parameter as SgaElementHeader;
            if (selectedElementHeader == null)
                throw new Exception("선택한 리스트박스 아이템은 Sga 엘리먼트 타입이 아닙니다.");


            int selectedElementIndex = selectedElementHeader.IndexInPackage;
            SgaPackage selectedPackage = ViewModel.SelectedPackage;

            if (!selectedPackage.IndexLoaded)
                selectedPackage.LoadIndex();

            if (!selectedPackage.IsElementLoaded(selectedElementIndex))
                selectedPackage.LoadElement(selectedElementIndex, true);

            SgaElement selectedElement = selectedPackage.GetElement(selectedElementIndex);

            switch (selectedElement.Type)
            {
                case SgaElementType.Sound:
                    // 사운드는 어차피 보여줄게 없으므로.. 비어있는놈으로 보여준다.
                    ViewModel.SelectedImage = new SgaImage();
                    break;
                case SgaElementType.Image:
                    ViewModel.SelectedImage = selectedElement as SgaImage ?? throw new Exception("말도 안됩니다. SgaImage 타입인데 캐스팅에 실패했습니다.");
                    break;
            }

            ViewModel.ResourceSelectionStatus = $"{selectedPackage.FileNameWithoutExt} ➯ {selectedElement.Header.NameWithoutExt}";
        }
    }
}
