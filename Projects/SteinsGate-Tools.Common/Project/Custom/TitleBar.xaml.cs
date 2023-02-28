/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 11:52:42 AM
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
using SGToolsCommon.Extension;

namespace SGToolsCommon.Custom
{
    public partial class TitleBar : UserControl
    {
        public string Title
        {
            get => _tbTitle.Text;
            set => _tbTitle.Text = value;
        }

       
        private Window _window;

        public TitleBar()
        {
            InitializeComponent();
            
            Loaded += (sender, args) =>
            {
                _window = this.FindParent<Window>();

                if (_window == null)
                    return;
                    //throw new Exception("무조건 윈도우 내부에 포함되어야합니다.");

                _window.MouseDown += WindowOnMouseDown;
            };
        }

        private void WindowOnMouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ChangedButton == MouseButton.Left)
            {
                bool allowTransparency = _window.AllowsTransparency;

                if (allowTransparency)
                    _window.Opacity = 0.3;

                _window.DragMove();

                if (allowTransparency)
                    _window.Opacity = 1.0;
            }
        }

        private void CloseOnClick(object sender, RoutedEventArgs e)
        {
            _window.Close();
        }

        private void MaximizeOnClick(object sender, RoutedEventArgs e)
        {
            if (_window.WindowState == WindowState.Maximized)
                _window.WindowState = WindowState.Normal;
            else
                _window.WindowState = WindowState.Maximized;
        }

        private void MinimizeOnClick(object sender, RoutedEventArgs e)
        {
            _window.WindowState = WindowState.Minimized;
        }
    }
}
