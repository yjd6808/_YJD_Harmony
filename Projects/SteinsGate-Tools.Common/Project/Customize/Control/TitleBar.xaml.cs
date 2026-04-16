/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 11:52:42 AM
 *
 */

using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

using SGToolsCommon.Extension;

namespace SGToolsCommon.Customize.Control
{
    public partial class TitleBar : UserControl
    {
        private Window window_ = null!;

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

        //////////////////////////////////////////////////////////////////////////////////
        public TitleBar()
        {
            InitializeComponent();
            Loaded += (sender, args) =>
            {
                window_ = this.FindParent<Window>()!;

                if (DesignerProperties.GetIsInDesignMode(this))
                    return;

                if (window_ == null)
                    throw new Exception("무조건 윈도우 내부에 포함되어야합니다.");

                window_.MouseDown += WindowOnMouseDown;
                window_.PreviewKeyDown += OnPreviewKeyDown;
            };
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void Close()
        {
            if (RealClose)
                window_.Close();
            else
                window_.Visibility = Visibility.Collapsed;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnPreviewKeyDown(object _sender, KeyEventArgs _e)
        {
            if (EscTermination && _e.Key == Key.Escape)
                Close();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void WindowOnMouseDown(object _sender, MouseButtonEventArgs _e)
        {
            if (_e.ChangedButton == MouseButton.Left && _e.ButtonState == MouseButtonState.Pressed)
            {
                if (!Draggable)
                    return;

                bool allowTransparency = window_.AllowsTransparency;

                if (allowTransparency)
                    window_.Opacity = 0.3;

                window_.DragMove();

                if (allowTransparency)
                    window_.Opacity = 1.0;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void PinOnClick(object _sender, RoutedEventArgs _e)
        {
            if (window_.Topmost)
                window_.Topmost = false;
            else
                window_.Topmost = true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void CloseOnClick(object _sender, RoutedEventArgs _e)
        {
            Close();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void MaximizeOnClick(object _sender, RoutedEventArgs _e)
        {
            if (window_.WindowState == WindowState.Maximized)
                window_.WindowState = WindowState.Normal;
            else
                window_.WindowState = WindowState.Maximized;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void MinimizeOnClick(object _sender, RoutedEventArgs _e)
        {
            window_.WindowState = WindowState.Minimized;
        }
    }
}
