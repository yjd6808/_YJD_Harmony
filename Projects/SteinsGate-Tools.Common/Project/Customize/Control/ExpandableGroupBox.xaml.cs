using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace SGToolsCommon.Customize.Control
{
    public partial class ExpandableGroupBox : UserControl
    {
        public static Thickness DefaultTitleContentMargin = new (0, 0, 0, 0);
        public static Thickness DefaultTitleBorderThickness = new(0);

        public static GridLength DefaultBoxHeight0 = new(0, GridUnitType.Auto);
        public static GridLength DefaultBoxHeight1 = new(0, GridUnitType.Auto);    // auto의 경우 첫번째 인자는 무시됨

        public static GridLength DefaultTitleWidth0 = new(0, GridUnitType.Auto);
        public static GridLength DefaultTitleWidth1 = new(0, GridUnitType.Auto);    // auto의 경우 첫번째 인자는 무시됨

        public ExpandableGroupBox()
        {
            InitializeComponent();
        }

        public static readonly DependencyProperty IsExpandedProperty =
            DependencyProperty.Register(
                nameof(IsExpanded),
                typeof(bool),
                typeof(ExpandableGroupBox),
                new PropertyMetadata(false));

        public bool IsExpanded
        {
            get => (bool)GetValue(IsExpandedProperty);
            set => SetValue(IsExpandedProperty, value);
        }

        public static readonly RoutedEvent ExpandStateChangedEvent =
            EventManager.RegisterRoutedEvent(
                nameof(ExpandStateChanged),
                RoutingStrategy.Bubble,
                typeof(RoutedEventHandler),
                typeof(ExpandableGroupBox));

        public event RoutedEventHandler ExpandStateChanged
        {
            add => AddHandler(ExpandStateChangedEvent, value);
            remove => RemoveHandler(ExpandStateChangedEvent, value);
        }

        // IconType 의존성 제거 -> ImageSource DP로 대체
        public static readonly DependencyProperty TitleFoldStateImageSourceProperty =
            DependencyProperty.Register(
                nameof(TitleFoldStateImageSource),
                typeof(ImageSource),
                typeof(ExpandableGroupBox),
                new PropertyMetadata(null)
            );

        public static readonly DependencyProperty TitleExpandStateImageSourceProperty =
            DependencyProperty.Register(
                nameof(TitleExpandStateImageSource),
                typeof(ImageSource),
                typeof(ExpandableGroupBox),
                new PropertyMetadata(null)
            );

        public ImageSource TitleFoldStateImageSource
        {
            get => (ImageSource)GetValue(TitleFoldStateImageSourceProperty);
            set => SetValue(TitleFoldStateImageSourceProperty, value);
        }
        public ImageSource TitleExpandStateImageSource
        {
            get => (ImageSource)GetValue(TitleExpandStateImageSourceProperty);
            set => SetValue(TitleExpandStateImageSourceProperty, value);
        }

        public static readonly DependencyProperty TitleFoldStateBackgroundProperty =
            DependencyProperty.Register(
                nameof(TitleFoldStateBackground),
                typeof(Brush),
                typeof(ExpandableGroupBox),
                new PropertyMetadata(Brushes.LightGray));

        public Brush TitleFoldStateBackground
        {
            get => (Brush)GetValue(TitleFoldStateBackgroundProperty);
            set => SetValue(TitleFoldStateBackgroundProperty, value);
        }

        public static readonly DependencyProperty TitleExpandStateBackgroundProperty =
            DependencyProperty.Register(
                nameof(TitleExpandStateBackground),
                typeof(Brush),
                typeof(ExpandableGroupBox),
                new PropertyMetadata(Brushes.LightSkyBlue));

        public Brush TitleExpandStateBackground
        {
            get => (Brush)GetValue(TitleExpandStateBackgroundProperty);
            set => SetValue(TitleExpandStateBackgroundProperty, value);
        }

        public static readonly DependencyProperty TitleContentProperty =
            DependencyProperty.Register(
                nameof(TitleContent),
                typeof(object),
                typeof(ExpandableGroupBox),
                new PropertyMetadata(null));

        public object TitleContent
        {
            get => GetValue(TitleContentProperty);
            set => SetValue(TitleContentProperty, value);
        }

        public new static readonly DependencyProperty ContentProperty =
            DependencyProperty.Register(
                nameof(Content),
                typeof(object),
                typeof(ExpandableGroupBox),
                new PropertyMetadata(null));

        public new object Content
        {
            get => GetValue(ContentProperty);
            set => SetValue(ContentProperty, value);
        }

        public static readonly DependencyProperty TitleButtonStyleProperty =
            DependencyProperty.Register(
                nameof(TitleButtonStyle),
                typeof(System.Windows.Style),
                typeof(ExpandableGroupBox),
                new PropertyMetadata(null));

        public System.Windows.Style TitleButtonStyle
        {
            get => (System.Windows.Style)GetValue(TitleButtonStyleProperty);
            set => SetValue(TitleButtonStyleProperty, value);
        }

        public static readonly DependencyProperty TitleForgroundProperty =
            DependencyProperty.Register(
                nameof(TitleForground),
                typeof(Brush),
                typeof(ExpandableGroupBox),
                new PropertyMetadata(Brushes.Black));

        public Brush TitleForground
        {
            get => (Brush)GetValue(TitleForgroundProperty);
            set => SetValue(TitleForgroundProperty, value);
        }

        public static readonly DependencyProperty TitleBackgroundProperty =
            DependencyProperty.Register(
                nameof(TitleBackground),
                typeof(Brush),
                typeof(ExpandableGroupBox),
                new PropertyMetadata(Brushes.Transparent));

        public Brush TitleBackground
        {
            get => (Brush)GetValue(TitleBackgroundProperty);
            set => SetValue(TitleBackgroundProperty, value);
        }

        public static readonly DependencyProperty TitleContentMarginProperty = DependencyProperty.Register(
            nameof(TitleContentMargin),
            typeof(Thickness),
            typeof(ExpandableGroupBox),
            new PropertyMetadata(DefaultTitleContentMargin));

        public Thickness TitleContentMargin
        {
            get => (Thickness)GetValue(TitleContentMarginProperty);
            set => SetValue(TitleContentMarginProperty, value);
        }

        public static readonly DependencyProperty TitleBorderThicknessProperty = DependencyProperty.Register(
            nameof(TitleBorderThickness),
            typeof(Thickness),
            typeof(ExpandableGroupBox),
            new PropertyMetadata(DefaultTitleBorderThickness));

        public Thickness TitleBorderThickness
        {
            get => (Thickness)GetValue(TitleBorderThicknessProperty);
            set => SetValue(TitleBorderThicknessProperty, value);
        }

        public static readonly DependencyProperty TitleContentHorizontalAlignmentProperty = DependencyProperty.Register(
            nameof(TitleContentHorizontalAlignment),
            typeof(HorizontalAlignment),
            typeof(ExpandableGroupBox),
            new PropertyMetadata(HorizontalAlignment.Left));

        public HorizontalAlignment TitleContentHorizontalAlignment
        {
            get => (HorizontalAlignment)GetValue(TitleContentHorizontalAlignmentProperty);
            set => SetValue(TitleContentHorizontalAlignmentProperty, value);
        }

        public static readonly DependencyProperty TitleContentVerticalAlignmentProperty = DependencyProperty.Register(
            nameof(TitleContentVerticalAlignment),
            typeof(VerticalAlignment),
            typeof(ExpandableGroupBox),
            new PropertyMetadata(VerticalAlignment.Center));

        public VerticalAlignment TitleContentVerticalAlignment
        {
            get => (VerticalAlignment)GetValue(TitleContentVerticalAlignmentProperty);
            set => SetValue(TitleContentVerticalAlignmentProperty, value);
        }

        public static readonly DependencyProperty H0Property =
            DependencyProperty.Register(
                nameof(H0),
                typeof(GridLength),
                typeof(ExpandableGroupBox),
                new PropertyMetadata(DefaultBoxHeight0));

        public GridLength H0
        {
            get => (GridLength)GetValue(H0Property);
            set => SetValue(H0Property, value);
        }

        public static readonly DependencyProperty H1Property =
            DependencyProperty.Register(
                nameof(H1),
                typeof(GridLength),
                typeof(ExpandableGroupBox),
                new PropertyMetadata(DefaultBoxHeight1));

        public GridLength H1
        {
            get => (GridLength)GetValue(H1Property);
            set => SetValue(H1Property, value);
        }

        public static readonly DependencyProperty TW0Property =
            DependencyProperty.Register(
                nameof(TW0),
                typeof(GridLength),
                typeof(ExpandableGroupBox),
                new PropertyMetadata(DefaultTitleWidth0));

        public GridLength TW0
        {
            get => (GridLength)GetValue(TW0Property);
            set => SetValue(TW0Property, value);
        }

        public static readonly DependencyProperty TW1Property =
            DependencyProperty.Register(
                nameof(TW1),
                typeof(GridLength),
                typeof(ExpandableGroupBox),
                new PropertyMetadata(DefaultTitleWidth1));

        public GridLength TW1
        {
            get => (GridLength)GetValue(TW1Property);
            set => SetValue(TW1Property, value);
        }

       
        private void TitleButton_Click(object _sender, RoutedEventArgs _e)
        {
            IsExpanded = !IsExpanded;
            RaiseEvent(new RoutedEventArgs(ExpandStateChangedEvent));
        }
    }
}
