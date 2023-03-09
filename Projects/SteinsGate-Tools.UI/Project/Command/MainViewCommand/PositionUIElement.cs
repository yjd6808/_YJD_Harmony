/*
 * 작성자: 윤정도
 * 생성일: 3/8/2023 7:22:25 AM
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
using MoreLinq;
using SGToolsCommon.Primitive;
using SGToolsCommon.Resource;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class PositionUIElement : MainCommandAbstract
    {
        public PositionUIElement(MainViewModel viewModel) 
            : base(viewModel, "픽돠고 선택된 엘리먼트들(캔버스상에서 선택된 엘리먼트)의 정렬 위치를 (0, 0)으로 초기화합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            if (parameter is not Positioning positioning)
                throw new Exception("포지셔닝 타입이 아닙니다.");

            ValueTuple<VAlignment, HAlignment> value = Alignment.PositioningToAlignMap[(int)positioning];
            ViewModel.GroupMaster.SelectedElements.ForEach(
                element =>
                {
                    if (element.Picked)
                    {
                        element.SetPositionZero(value.Item1, value.Item2);
                    }
                });
        }
    }
}
