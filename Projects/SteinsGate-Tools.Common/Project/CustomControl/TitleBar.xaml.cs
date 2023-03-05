/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 11:52:42 AM
 *
 */


using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
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
using SGToolsCommon.CustomControl;
using SGToolsCommon.Extension;

namespace SGToolsCommon.CustomControl
{
    public partial class TitleBar : UserControl
    {
        public bool Draggable
        {
            get => (bool)GetValue(DraggableProperty);
            set => SetValue(DraggableProperty, value);
        }

        public string Title
        {
            get => (string)GetValue(TitleProperty);
            set => SetValue(TitleProperty, value);
        }


        private Window _window;

        public TitleBar()
        {
            InitializeComponent();
            
            Loaded += (sender, args) =>
            {
                _window = this.FindParent<Window>();

                if (DesignerProperties.GetIsInDesignMode(this))
                    return;

                if (_window == null)
                    throw new Exception("무조건 윈도우 내부에 포함되어야합니다.");

                _window.MouseDown += WindowOnMouseDown;
            };
        }

        private void WindowOnMouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ChangedButton == MouseButton.Left && e.ButtonState == MouseButtonState.Pressed)
            {
                if (!Draggable)
                    return;

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

        public static readonly DependencyProperty TitleProperty = DependencyProperty.Register(
            "Ttitle",
            typeof(string),
            typeof(TitleBar),
            new PropertyMetadata(null) { DefaultValue = "제목을 입력해주세요." });

        public static readonly DependencyProperty DraggableProperty = DependencyProperty.Register(
           "Draggable",
           typeof(bool),
           typeof(TitleBar),
           new PropertyMetadata(null) { DefaultValue = true });


    }
}
