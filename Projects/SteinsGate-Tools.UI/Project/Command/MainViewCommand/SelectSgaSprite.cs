/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 7:15:40 AM
 *
 */

using SGToolsCommon.Sga;
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
    public class SelectSgaSprite : MainCommandAbstract
    {
        public SelectSgaSprite(MainViewModel viewModel)
            : base(viewModel, "SGA 스프라이트를 선택합니다.")
        {
        }


        public override void Execute(object? parameter)
        {
            var eventArgs = parameter as MouseEventArgs;

            if (eventArgs == null)
                return;

            FrameworkElement sender = eventArgs.Source as FrameworkElement;

            if (sender == null)
                throw new Exception("프레임워크 엘리먼트가 아닙니다.");

            SgaSpriteAbstract sprite = sender.DataContext as SgaSpriteAbstract;
            string linkText = string.Empty;

            if (sprite.IsLink)
                linkText = $"링크({sprite.TargetFrameIndex})";

            ViewModel.ResourceSelectionStatus = 
                $"{sprite.Parent.Parent.FileNameWithoutExt} ➯ " +
                $"{sprite.Parent.Header.NameWithoutExt} ➯ " +
                $"{sprite.FrameIndex} {linkText}";
        }
    }
}
