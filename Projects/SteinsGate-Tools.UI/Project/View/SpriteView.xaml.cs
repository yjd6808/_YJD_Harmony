/*
 * 작성자: 윤정도
 *
 */

using System.ComponentModel;
using System.Windows;
using SGToolsCommon.Primitive;
using SGToolsUI.ViewModel;

namespace SGToolsUI.View
{
    /// <summary>
    /// Interaction logic for SpriteView.xaml
    /// </summary>
    public partial class SpriteView : Window
    {
        public SpriteViewModel ViewModel { get; }

        //////////////////////////////////////////////////////////////////////////////////
        public SpriteView()
        {
            ViewModel = new SpriteViewModel();
            ViewModel.View = this;
            Resources.Add("ViewModel", ViewModel);
            InitializeComponent();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SpriteView_OnClosing(object? _sender, CancelEventArgs _e)
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void AdjustSize()
        {
            SpriteImage.Width = ViewModel.RealTarget.Width;
            SpriteImage.Height = ViewModel.RealTarget.Height;
            Width = SpriteImage.Width + BorderThickness.Left + BorderThickness.Right;
            Height = SpriteImage.Height + BorderThickness.Top + BorderThickness.Bottom;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void AdjustPosition(IntPoint _offsetInMonitor)
        {
            Left = _offsetInMonitor.X - Width - 5;
            Top = _offsetInMonitor.Y;
        }
    }
}
