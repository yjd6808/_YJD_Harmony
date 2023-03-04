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
    public class CanvasZoomReset : MainCommandAbstract
    {
        public CanvasZoomReset(MainViewModel viewModel) 
            : base(viewModel, "줌을 가장 기본 상태로 리셋합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            ViewModel.ZoomState.ZoomLevelX = 1.0;
            ViewModel.ZoomState.ZoomLevelY = 1.0;
        }
    }
}
