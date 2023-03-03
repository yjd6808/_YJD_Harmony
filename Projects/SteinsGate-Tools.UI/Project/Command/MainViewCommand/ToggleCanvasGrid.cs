/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 7:10:16 AM
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
    public class ToggleCanvasGrid : MainCommandAbstract
    {
        public ToggleCanvasGrid(MainViewModel viewModel) 
            : base(viewModel, "캔버스에 그리드를 표시합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            ViewModel.View.CanvasShapesControl.IsGridVisible = !ViewModel.View.CanvasShapesControl.IsGridVisible;
        }
    }
}
