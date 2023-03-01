/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 12:48:46 PM
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
using SGToolsUI.Model;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class DeleteUIElement : MainCommandAbstract
    {
        public DeleteUIElement(MainViewModel viewModel)
            : base(viewModel, "파라미터로 전달된 SGUIElement를 트리뷰에서 삭제합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            SGUIElement element = parameter as SGUIElement;
        }
    }
}
