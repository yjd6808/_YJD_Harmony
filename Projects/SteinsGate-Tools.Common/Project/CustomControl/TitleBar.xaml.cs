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


        // true로 설정시 코드로 반드시 마지막에 Close 수동 
        public bool RealClose
        {
            get => (bool)GetValue(RealCloseProperty);
            set => SetValue(RealCloseProperty, value);
        }

        public bool EscTermination
        {
            get => (bool)GetValue(EscTerminationProperty);
            set => SetValue(EscTerminationProperty, value);
        }

        public object TitleContent
        {
            get => (object)GetValue(TitleContentProperty);
            set => SetValue(TitleContentProperty, value);
        }

        public DataTemplate TitleContentTemplate
        {
            get => (DataTemplate)GetValue(TitleContentTemplateProperty);
            set => SetValue(TitleContentTemplateProperty, value);
        }

        public Thickness TitleContentMargin
        {
            get => (Thickness)GetValue(TitleContentMarginProperty);
            set => SetValue(TitleContentMarginProperty, value);
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
                _window.PreviewKeyDown += OnPreviewKeyDown;
            };
        }

        private void OnPreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (EscTermination && e.Key == Key.Escape)
                Close();
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

        private void PinOnClick(object sender, RoutedEventArgs e)
        {
            if (_window.Topmost)
                _window.Topmost = false;
            else
                _window.Topmost = true;
        }

        private void CloseOnClick(object sender, RoutedEventArgs e)
        {
            Close();
        }

        public void Close()
        {
            if (RealClose)
                _window.Close();
            else
                _window.Visibility = Visibility.Collapsed;
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
            nameof(Title),
            typeof(string),
            typeof(TitleBar),
            new PropertyMetadata(null) { DefaultValue = "제목을 입력해주세요." });

        public static readonly DependencyProperty RealCloseProperty = DependencyProperty.Register(
            nameof(RealClose),
            typeof(bool),
            typeof(TitleBar),
            new PropertyMetadata(true));

        public static readonly DependencyProperty DraggableProperty = DependencyProperty.Register(
           nameof(Draggable),
           typeof(bool),
           typeof(TitleBar),
           new PropertyMetadata(null) { DefaultValue = true });

        public static readonly DependencyProperty EscTerminationProperty = DependencyProperty.Register(
            nameof(EscTermination),
            typeof(bool),
            typeof(TitleBar),
            new PropertyMetadata(false));

        public static readonly DependencyProperty TitleContentProperty = DependencyProperty.Register(
            nameof(TitleContent),
            typeof(object),
            typeof(TitleBar),
            new PropertyMetadata(null));

        public static readonly DependencyProperty TitleContentTemplateProperty = DependencyProperty.Register(
            nameof(TitleContentTemplate),
            typeof(DataTemplate),
            typeof(TitleBar),
            new PropertyMetadata(null));

        public static readonly DependencyProperty TitleContentMarginProperty = DependencyProperty.Register(
            nameof(TitleContentMargin),
            typeof(Thickness),
            typeof(TitleBar),
            new PropertyMetadata(new Thickness()));
    }
}
