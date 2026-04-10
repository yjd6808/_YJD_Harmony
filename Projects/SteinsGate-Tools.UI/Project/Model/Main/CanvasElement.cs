/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 3:53:53 AM
 *
 */

using System.ComponentModel;
using SGToolsCommon;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Model.Main
{
    public enum CanvasElementType
    {
        ShapeElement,
        UIElement
    }

    public abstract class CanvasElement : Bindable
    {
        [Browsable(false)]
        public abstract CanvasElementType CanvasElementType { get; }

        // Xaml에서 접근 편하게 하기위함.
        // 메모리 몇바이트 더 먹고 훨씬 편해지고, 성능도 큰 차이는 없겠지만 개선될듯
        // Binding의 RelativeSource수행시 부모 찾아가는 연산 자체가 없어질테니
        [Browsable(false)]
        public MainViewModel ViewModel { get; set; }

        //////////////////////////////////////////////////////////////////////////////////
        public void SetViewModel(MainViewModel _model) => viewModel_ = _model;
        private MainViewModel viewModel_;
    }
}
