using SGToolsUI.ViewModel;
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
using System.Windows.Shapes;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;

namespace SGToolsUI.View
{
    /// <summary>
    /// Interaction logic for SpriteView.xaml
    /// </summary>
    public partial class SpriteView : Window
    {
        public SpriteViewModel ViewModel { get; }

        public SpriteView()
        {
            ViewModel = new SpriteViewModel();
            ViewModel.View = this;
            Resources.Add("ViewModel", ViewModel);
            InitializeComponent();
        }

        private void SpriteView_OnClosing(object? sender, CancelEventArgs e)
        {
        }

        public void AdjustSize()
        {
            SpriteImage.Width = ViewModel.RealTarget.Width;
            SpriteImage.Height = ViewModel.RealTarget.Height;
            Width = SpriteImage.Width + BorderThickness.Left + BorderThickness.Right;
            Height = SpriteImage.Height+ BorderThickness.Top + BorderThickness.Bottom;

        }

        public void AdjustPosition(IntPoint offsetInMonitor)
        {
            Left = offsetInMonitor.X - Width - 5;
            Top = offsetInMonitor.Y;
        }
    }
}
