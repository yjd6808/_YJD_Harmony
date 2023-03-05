/*
 * 작성자: 윤정도
 * 생성일: 3/4/2023 10:21:24 AM
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
    public class CanvasZoomUp : MainCommandAbstract
    {
        public CanvasZoomUp(MainViewModel viewModel) 
            : base(viewModel, "축소합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            double zoomLevelX =  ViewModel.ZoomState.ZoomLevelX;
            double zoomLevelY = ViewModel.ZoomState.ZoomLevelY;
            

            if (zoomLevelX - 0.1 < 0.53)
                return;

            ViewModel.ZoomState.ZoomLevelY = zoomLevelY - 0.1 * Constant.ResolutionRatio;
            ViewModel.ZoomState.ZoomLevelX = zoomLevelX - 0.1;
        }
    }
}
