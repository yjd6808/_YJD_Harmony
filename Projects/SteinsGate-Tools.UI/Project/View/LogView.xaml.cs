/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:40:42 PM
 *
 */

using System;
using System.Collections.Generic;
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
using SGToolsCommon.CustomControl;
using SGToolsUI.ViewModel;

namespace SGToolsUI.View
{
    public partial class LogView : Window
    {
        public LogViewModel ViewModel { get; }

        public LogView(LogListBox box)
        {
            ViewModel = new LogViewModel();
            InitializeComponent();

            MainPanel.Children.Add(box);
            DockPanel.SetDock(box, Dock.Top);
        }
    }
}
