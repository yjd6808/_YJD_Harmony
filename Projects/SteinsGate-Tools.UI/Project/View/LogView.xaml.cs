/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:40:42 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using MoreLinq.Extensions;
using SGToolsCommon.CustomControl;
using SGToolsCommon.Extension;

namespace SGToolsUI.View
{
    public enum LogType
    {
        Path,
        MultiPath
    }

    public partial class LogView : Window
    {
        public LogListBox LogBox { get; }

        //////////////////////////////////////////////////////////////////////////////////
        public LogView(LogListBox _box)
        {
            InitializeComponent();
            LogBox = _box;
            LogBox.LogClick += LogBox_OnLogClick;
            MainPanel.Children.Add(_box);
            DockPanel.SetDock(_box, Dock.Top);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void LogBox_OnLogClick(object _sender, RoutedEventArgs _e)
        {
            object data = ((LogClickEventArgs)_e).LogData.Data;

            if (data == null)
                return;

            try
            {
                ValueTuple<LogType, object> logData = (ValueTuple<LogType, object>)(data);

                switch (logData.Item1)
                {
                    case LogType.Path:
                        ProcessEx.OpenDirectory((string)logData.Item2);
                        break;
                    case LogType.MultiPath:
                        ((IEnumerable<string>)logData.Item2).ForEach(path => ProcessEx.OpenDirectory(path));
                        break;
                    default:
                        throw new Exception("로그 타입이 이상합니다.");
                }
            }
            catch (Exception exception)
            {
                MessageBoxEx.ShowTopMost(exception);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void LogView_OnActivated(object? _sender, EventArgs _e)
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void CleanLogBoxButton_OnClick(object _sender, RoutedEventArgs _e)
        {
            LogBox.Clear();
        }
    }
}
