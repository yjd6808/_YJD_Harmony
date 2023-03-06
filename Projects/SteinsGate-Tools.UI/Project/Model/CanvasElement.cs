/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 3:53:53 AM
 *
 */

using SGToolsCommon;
using System;
using System.Collections.Generic;
using System.ComponentModel;
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

namespace SGToolsUI.Model
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

        public void SetViewModel(MainViewModel model)  => _viewModel = model;
        private MainViewModel _viewModel;
    }
}
