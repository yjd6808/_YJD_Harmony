/*
 * 작성자: 윤정도
 * 생성일: 3/8/2023 7:22:25 AM
 *
 */

using System;
using MoreLinq;
using SGToolsCommon.Primitive;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class PositionUIElement : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public PositionUIElement(MainViewModel _viewModel)
            : base(_viewModel, "픽돠고 선택된 엘리먼트들(캔버스상에서 선택된 엘리먼트)의 정렬 위치를 (0, 0)으로 초기화합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            if (_parameter is not Positioning positioning)
                throw new Exception("포지셔닝 타입이 아닙니다.");

            ValueTuple<VAlignment, HAlignment> value = Alignment.PositioningToAlignMap[(int)positioning];
            ViewModel.RootGroup.SelectedElements.ForEach(
                element =>
                {
                    if (element.Picked)
                        element.SetPositionZero(value.Item1, value.Item2);
                });
        }
    }
}
