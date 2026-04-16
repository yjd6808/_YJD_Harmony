/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 12:34:42 PM
 *
 * 모든 툴에서 공통으로 사용할 로그박스
 */

using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;

using SGToolsCommon.Resource;

namespace SGToolsCommon.Customize.Control
{
    public class LogData
    {
        public string Log { get; set; }
        public object? Data { get; set; }
        public Brush Foreground { get; } = Brushes.Black;
        public BitmapImage HeaderImageSource { get; }
        public bool HasData => Data != null;

        //////////////////////////////////////////////////////////////////////////////////
        public LogData(string _log, object? _data, BitmapImage _headerImageSource, Brush? _foreground = null)
        {
            Log = _log;
            Data = _data;
            HeaderImageSource = _headerImageSource;

            if (_foreground != null)
                Foreground = _foreground;
        }
    }

    public class LogClickEventArgs : RoutedEventArgs
    {
        public LogData? LogData { get; }

        //////////////////////////////////////////////////////////////////////////////////
        public LogClickEventArgs(RoutedEvent _routedEvent, LogData? _logData) : base(_routedEvent)
            => LogData = _logData;
    }

    public class LogListBox : ListBox, INotifyPropertyChanged
    {
        private LogData? lastLog_;

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

        public ObservableCollection<LogData> Logs { get; } = new();

        public LogData? LastLog
        {
            get => lastLog_;
            set
            {
                lastLog_ = value;
                OnPropertyChanged();
            }
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

        public event PropertyChangedEventHandler? PropertyChanged;

        //////////////////////////////////////////////////////////////////////////////////
        public LogListBox()
        {
            Loaded += OnLoaded;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void AddLog(Exception _e)
        {
            StackFrame? frame = (new StackTrace(_e, true)).GetFrame(0);
            string lastFrameInfo = $"{System.IO.Path.GetFileName(frame?.GetFileName())}\n{frame?.GetMethod()?.Name}()\n{frame?.GetFileLineNumber()}";
            AddLog(_e.Message + "\n" + lastFrameInfo, null, IconCommonType.NotUsable, Brushes.Crimson);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void AddLog(string _log, object? _data = null, IconCommonType _type = IconCommonType.Info, Brush? _brush = null)
        {
            if (Logs.Count > MaxItemCount)
                Logs.RemoveAt(0);

            LastLog = new LogData(_log, _data, R.GetIconCommon(_type), _brush);
            Logs.Add(LastLog);

            if (Logs.Count > 0)
                ScrollIntoView(Logs.Last());
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void AddDispatchedLog(string _log, object? _data = null, IconCommonType _type = IconCommonType.Info, Brush? _brush = null)
            => Dispatcher.BeginInvoke(() => AddLog(_log, _data, _type, _brush));

        //////////////////////////////////////////////////////////////////////////////////
        public void AddDispatchedLog(Exception _e)
            => Dispatcher.BeginInvoke(() => AddLog(_e));

        //////////////////////////////////////////////////////////////////////////////////
        public void Clear()
        {
            Logs.Clear();
            LastLog = null;
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected override void OnMouseDoubleClick(MouseButtonEventArgs _e)
        {
            base.OnMouseDoubleClick(_e);

            RoutedEventArgs newEventArgs = new LogClickEventArgs(LogClickEvent, SelectedItem as LogData);
            RaiseEvent(newEventArgs);
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected virtual void OnPropertyChanged([CallerMemberName] string? _propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(_propertyName));
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnLoaded(object _sender, RoutedEventArgs _e)
        {
            Binding sourceBinding = new Binding(nameof(Logs));
            sourceBinding.Source = this;
            SetBinding(ItemsSourceProperty, sourceBinding);
        }
    }
}
