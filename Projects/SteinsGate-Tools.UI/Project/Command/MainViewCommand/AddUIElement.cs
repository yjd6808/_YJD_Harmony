/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 12:52:59 PM
 *
 */

using SGToolsUI.Model;
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

namespace SGToolsUI.Command.MainViewCommand
{
    public class AddUIElement : MainCommandAbstract
    {
        public AddUIElement(MainViewModel viewModel)
            : base(viewModel, "선택한 그룹에 UIElement를 추가합니다. " +
                              "선택한 그룹이 없을 경우 루트에 추가합니다. " +
                              "루트에는 그룹 엘리먼트만 추가가능합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            SGUIElementType addElementType = (SGUIElementType)Enum.Parse(typeof(SGUIElementType), parameter.ToString());



        }
    }
}
