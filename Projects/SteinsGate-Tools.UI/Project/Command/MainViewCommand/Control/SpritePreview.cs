/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 2:26:04 AM
 *
 */

using System;
using System.Windows;
using System.Windows.Input;
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;
using SGToolsUI.View;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand.Control
{
    public class SpritePreview : MainCommandAbstract
    {
        private SpriteView spriteView_;

        //////////////////////////////////////////////////////////////////////////////////
        public SpritePreview(MainViewModel _viewModel)
            : base(_viewModel, "SpriteListBoxItem위에 마우스를 올린 경우 이벤트를 처리합니다.")
        {
            spriteView_ = new SpriteView();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            var eventArgs = _parameter as MouseEventArgs;

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

                spriteView_.ViewModel.PreviewTarget = sprite;
                spriteView_.Show();
                spriteView_.AdjustSize();
                spriteView_.AdjustPosition(sender.GetOffsetInMonitor());
            }
            else if (eventArgs.RoutedEvent == Mouse.MouseLeaveEvent)
            {
                spriteView_.Visibility = Visibility.Collapsed;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Dispose()
        {
            spriteView_.Close();
        }
    }
}
