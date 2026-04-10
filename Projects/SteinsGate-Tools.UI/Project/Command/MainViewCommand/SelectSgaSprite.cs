/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 7:15:40 AM
 *
 */

using System;
using System.Windows;
using System.Windows.Input;
using SGToolsCommon.Sga;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{
    public class SelectSgaSprite : MainCommandAbstract
    {
        //////////////////////////////////////////////////////////////////////////////////
        public SelectSgaSprite(MainViewModel _viewModel)
            : base(_viewModel, "SGA 스프라이트를 선택합니다.")
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Execute(object? _parameter)
        {
            switch (_parameter)
            {
                case MouseEventArgs eventArgs: SelectByEventArgs(eventArgs); break;
                case SgaSpriteAbstract sprite: SelectBySprite(sprite); break;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SelectBySprite(SgaSpriteAbstract _sprite)
        {
            string linkText = string.Empty;

            if (_sprite.IsLink)
                linkText = $"링크({_sprite.TargetFrameIndex})";

            ViewModel.SelectedSprite = _sprite;
            ViewModel.ResourceSelectionStatus =
                $"{_sprite.Parent.Parent.FileNameWithoutExt} ➯ " +
                $"{_sprite.Parent.Header.NameWithoutExt} ➯ " +
                $"{_sprite.FrameIndex} {linkText}";
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SelectByEventArgs(MouseEventArgs _eventArgs)
        {
            FrameworkElement sender = _eventArgs.Source as FrameworkElement;

            if (sender == null)
                throw new Exception("프레임워크 엘리먼트가 아닙니다.");

            SgaSpriteAbstract sprite = sender.DataContext as SgaSpriteAbstract;
            SelectBySprite(sprite);
        }
    }
}
