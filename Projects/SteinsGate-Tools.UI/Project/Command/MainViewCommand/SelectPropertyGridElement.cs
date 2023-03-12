/*
 * 작성자: 윤정도
 * 생성일: 3/12/2023 7:08:06 PM
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
    public class SelectPropertyGridElement : MainCommandAbstract
    {
        public SelectPropertyGridElement(MainViewModel viewModel) 
            : base(viewModel, "프로퍼티 그리드에 오브젝트를 할당합니다.")
        {
        }

        public override void Execute(object? parameter)
        {
            if (parameter is not SGUIElement element)
                return;

            ViewModel.View.UIElementPropertyGrid.SelectedObject = element;
        }
    }
}
