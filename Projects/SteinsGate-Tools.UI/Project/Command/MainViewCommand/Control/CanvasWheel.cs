/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 6:05:12 AM
 *
 */

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
using System.Windows.Media.Media3D;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace SGToolsUI.Command.MainViewCommand.Control
{
    public class CanvasWheel : MainCommandAbstract
    {
        public CanvasWheel(MainViewModel viewModel)
            : base(viewModel, "UI 툴의 캔버스 크기를 휠로 조절합니다. 이에따라 윈도우 크기도 같이 조절됩니다.")
        {
        }
        public override void Execute(object? parameter)
        {
            MouseWheelEventArgs wheelEventArgs =  parameter as MouseWheelEventArgs;

            if (wheelEventArgs == null)
                return;

            double canvasWidth = ViewModel.View.MainCanvas.DesiredSize.Width;
            double canvasHeight = ViewModel.View.MainCanvas.DesiredSize.Height;
            double deltaWidth = Constant.CanvasWheelDelta;
            double deltaHeight = Constant.CanvasWheelDelta * Constant.ResoltionRatio;
            double deltaScaleX;
            double deltaScaleY;

            if (wheelEventArgs.Delta > 0)
            {
                if (canvasWidth >= 1500.0f)
                    return;
            }
            else
            {
                if (canvasWidth <= 200.0f)
                    return;

                deltaWidth *= -1;
                deltaHeight *= -1;
            }

            deltaScaleX = (canvasWidth + deltaWidth) / Constant.CanvasWidth;
            deltaScaleY = (canvasHeight + deltaHeight) / Constant.CanvasHeight;

            // 캔버스 스케일 반영
            ScaleTransform scaleTransform = new ScaleTransform(deltaScaleX, deltaScaleY);
            ViewModel.View.MainCanvas.LayoutTransform = scaleTransform;

            // 매인 윈도 크기 반영
            ViewModel.View.Width += deltaWidth;
            ViewModel.View.Height += deltaHeight;

            // 캔버스 상태표시줄 너비 반영
            double lengthValue = ViewModel.View.CanvasStatusBarColumnDefinition.ActualWidth + deltaWidth;
            GridLengthConverter gridLengthConverter = new GridLengthConverter();
            GridLength gridLength = (GridLength)gridLengthConverter.ConvertFrom(lengthValue.ToString());
            ViewModel.View.CanvasStatusBarColumnDefinition.Width = gridLength;
        }
    }
}
