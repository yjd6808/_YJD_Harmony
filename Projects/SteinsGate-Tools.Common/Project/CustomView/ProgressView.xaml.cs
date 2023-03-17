/*
 * 비동기 진행작업 상황을 표시할 뷰를 간단하게 만듬
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
using System.Windows.Shapes;

namespace SGToolsCommon.CustomView
{
    public partial class ProgressView : Window, INotifyPropertyChanged
    {
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

        public ProgressView()
        {
            InitializeComponent();
        }

        public static DependencyProperty ProgressTextProperty = DependencyProperty.Register(
            nameof(ProgressText),
            typeof(string),
            typeof(ProgressView),
            new PropertyMetadata("")
        );

        public static DependencyProperty ProgressPercentageProperty = DependencyProperty.Register(
            nameof(ProgressPercentage),
            typeof(double),
            typeof(ProgressView),
            new PropertyMetadata(0.0)
        );

        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

    }
}
