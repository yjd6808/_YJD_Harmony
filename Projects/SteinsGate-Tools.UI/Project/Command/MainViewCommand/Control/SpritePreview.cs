/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 2:26:04 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
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
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;
using SGToolsUI.View;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand.Control
{
    public class SpritePreview : MainCommandAbstract
    {
        private SpriteView _spriteView;

        public SpritePreview(MainViewModel viewModel)
            : base(viewModel, "SpriteListBoxItem위에 마우스를 올린 경우 이벤트를 처리합니다.")
        {
            _spriteView = new SpriteView();
        }

        public override void Execute(object? parameter)
        {
            var eventArgs = parameter as MouseEventArgs;

            if (eventArgs == null)
                return;

            if (eventArgs.RoutedEvent == Mouse.MouseEnterEvent)
            {
                FrameworkElement sender = eventArgs.Source as FrameworkElement;

                if (sender == null)
                    throw new Exception("프레임워크 엘리먼트가 아닙니다.");

                SgaSpriteAbstract sprite = sender.DataContext as SgaSpriteAbstract;

                if (sprite == null)
                    throw new Exception("SgaSpriteAbstract 타입이 아닙니다.");

                _spriteView.ViewModel.PreviewTarget = sprite;
                _spriteView.Show();
                _spriteView.AdjustSize();
                _spriteView.AdjustPosition(sender.GetOffsetInMonitor());
            }
            else if (eventArgs.RoutedEvent == Mouse.MouseLeaveEvent)
            {
                _spriteView.Visibility = Visibility.Collapsed;
            }
        }

        public override void Dispose()
        {
            _spriteView.Close();
        }
    }
}
