/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 12:34:42 PM
 *
 * 모든 툴에서 공통으로 사용할 로그박스
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
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
using SGToolsCommon.Resource;

namespace SGToolsCommon.CustomControl
{
    public class LogData
    {
        public LogData(string log, object data, BitmapImage headerImageSource, Brush foreground = null)
        {
            Log = log;
            Data = data;
            HeaderImageSource = headerImageSource;

            if (foreground != null)
                Foreground = foreground;
        }

        
        public string Log { get; set; }
        public object Data { get; set; }
        public Brush Foreground { get; } = Brushes.Black;
        public BitmapImage HeaderImageSource { get; }
        public bool HasData => Data != null;
    }


    public class LogClickEventArgs : RoutedEventArgs
    {
        public LogData LogData { get; }

        public LogClickEventArgs(RoutedEvent routedEvent, LogData logData) : base(routedEvent)
            => LogData = logData;
    }

    public class LogListBox : ListBox, INotifyPropertyChanged
    {
        public ObservableCollection<LogData> Logs { get; } = new ();

        public LogData LastLog
        {
            get => _lastLog;
            set
            {
                _lastLog = value;
                OnPropertyChanged();
            }
        }

        private LogData _lastLog;

        public LogListBox()
        {
            Loaded += OnLoaded;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            Binding sourceBinding = new Binding(nameof(Logs));
            sourceBinding.Source = this;
            SetBinding(ItemsSourceProperty, sourceBinding);
        }

        public void AddLog(Exception e)
        {
            StackFrame frame = (new StackTrace(e, true)).GetFrame(0);
            string lastFrameInfo = $"{System.IO.Path.GetFileName(frame.GetFileName())}\n{frame.GetMethod().Name}()\n{frame.GetFileLineNumber()}";
            
            AddLog(e.Message + "\n" + lastFrameInfo, null, IconCommonType.NotUsable,
                Brushes.Crimson);
        }

        public void AddLog(string log, object data = null, IconCommonType type = IconCommonType.Info, Brush brush = null)
        {
            if (Logs.Count > MaxItemCount)
                Logs.RemoveAt(0);

            LastLog = new LogData(log, data, R.GetIconCommon(type), brush);
            Logs.Add(LastLog);

            if (Logs.Count > 0)
                ScrollIntoView(Logs.Last());
        }

        protected override void OnMouseDoubleClick(MouseButtonEventArgs e)
        {
            base.OnMouseDoubleClick(e);

            RoutedEventArgs newEventArgs = new LogClickEventArgs(LogClickEvent, SelectedItem as LogData);
            RaiseEvent(newEventArgs);
        }

        public int MaxItemCount
        {
            get => (int)GetValue(MaxItemCountProperty);
            set => SetValue(MaxItemCountProperty, value);
        }

        public event RoutedEventHandler LogClick
        {
            add { AddHandler(LogClickEvent, value); }
            remove { RemoveHandler(LogClickEvent, value); }
        }

        public static DependencyProperty MaxItemCountProperty = DependencyProperty.Register(
            nameof(MaxItemCount),
            typeof(int),
            typeof(LogListBox),
            new PropertyMetadata(1000)
        );

        public static readonly RoutedEvent LogClickEvent = EventManager.RegisterRoutedEvent(
            nameof(LogClickEvent),
            RoutingStrategy.Direct,
            typeof(RoutedEventHandler),
            typeof(LogListBox)
        );

        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

    }
}
