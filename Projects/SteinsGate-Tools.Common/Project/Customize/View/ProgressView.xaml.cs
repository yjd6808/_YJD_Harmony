/*
 * 비동기 진행작업 상황을 표시할 뷰를 간단하게 만듬
 */

using System.Windows;

namespace SGToolsCommon.Customize.View
{
    public partial class ProgressView : Window
    {
        public static DependencyProperty ProgressTextProperty = DependencyProperty.Register(
            nameof(ProgressText),
            typeof(string),
            typeof(ProgressView),
            new PropertyMetadata(string.Empty)
        );

        public static DependencyProperty ProgressPercentageProperty = DependencyProperty.Register(
            nameof(ProgressPercentage),
            typeof(double),
            typeof(ProgressView),
            new PropertyMetadata(0.0)
        );

        public string ProgressText
        {
            get => (string)GetValue(ProgressTextProperty);
            set => SetValue(ProgressTextProperty, value);
        }

        public double ProgressPercentage
        {
            get => (double)GetValue(ProgressPercentageProperty);
            set
            {
                SetValue(ProgressPercentageProperty, value);
                if (value >= 100)
                    Dispatcher.BeginInvoke(() => TitleBar.Close());
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public ProgressView()
        {
            InitializeComponent();
        }
    }
}
