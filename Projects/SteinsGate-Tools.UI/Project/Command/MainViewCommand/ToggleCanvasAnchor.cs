/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 7:10:06 AM
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
    public class ToggleCanvasAnchor : MainCommandAbstract
    {
        public ToggleCanvasAnchor(MainViewModel viewModel)
            : base(viewModel, "그룹의 앵커 위치를 표시합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            ViewModel.View.CanvasShapesControl.IsAnchorVisible =
                !ViewModel.View.CanvasShapesControl.IsAnchorVisible;
        }
    }
}
