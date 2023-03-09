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
using SGToolsCommon.Resource;

namespace SGToolsCommon.CustomControl
{
    public class LogData
    {
        public LogData(string log, object data, BitmapImage headerImageSource)
        {
            Log = log;
            Data = data;
            HeaderImageSource = headerImageSource;
        }

        
        public string Log { get; set; }
        public object Data { get; set; }
        public BitmapImage HeaderImageSource { get; }
        public bool HasData => Data != null;
    }


    public class LogClickEventArgs : RoutedEventArgs
    {
        public LogData LogData { get; }

        public LogClickEventArgs(RoutedEvent routedEvent, LogData logData) : base(routedEvent)
            => LogData = logData;
    }

    public class LogListBox : ListBox
    {
        public ObservableCollection<LogData> Logs { get; } = new ();

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
            => AddLog(e.Message + "\n" + e.StackTrace.Split('\n').Last(), null, IconCommonType.NotUsable);

        public void AddLog(string log, object data = null, IconCommonType type = IconCommonType.Info)
        {
            if (Logs.Count > MaxItemCount)
                Logs.RemoveAt(0);

            Logs.Add(new LogData(log, data, R.GetIconCommon(type)));

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
    }
}
