/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:40:42 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
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
using System.Windows.Shapes;
using MoreLinq.Extensions;
using SGToolsCommon.CustomControl;
using SGToolsCommon.Extension;
using SGToolsUI.ViewModel;

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

        public LogView(LogListBox box)
        {
            InitializeComponent();
            LogBox = box;
            LogBox.LogClick += LogBox_OnLogClick;
            MainPanel.Children.Add(box);
            DockPanel.SetDock(box, Dock.Top);
        }

        private void LogBox_OnLogClick(object sender, RoutedEventArgs e)
        {
            object data = ((LogClickEventArgs)e).LogData.Data;

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

        private void LogView_OnActivated(object? sender, EventArgs e)
        {
        }

        private void CleanLogBoxButton_OnClick(object sender, RoutedEventArgs e)
        {
            LogBox.Clear();
        }
    }
}
