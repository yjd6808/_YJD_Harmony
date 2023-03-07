/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 7:12:54 AM
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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace SGToolsCommon.Extension
{
    public static class MessageBoxEx
    {
        public static MessageBoxResult ShowTopMost(string message, string caption = null, MessageBoxButton button = MessageBoxButton.OK, MessageBoxImage icon = MessageBoxImage.None)
        {
            Window parentWindow = Application.Current.MainWindow;

            if (parentWindow == null)
                throw new Exception("Application의 메인윈도우가 null입니다.");

            parentWindow.Topmost = true;
            MessageBoxResult result = MessageBox.Show(parentWindow, message, caption, button, icon);
            parentWindow.Topmost = false;
            return result;
        }

        public static MessageBoxResult ShowTopMost(Exception e)
        {
            return ShowTopMost(e.Message + "\n\n" + e.StackTrace, "예외 발생", MessageBoxButton.OK, MessageBoxImage.Error);
        }

        public static void ShowTopMostMessageBox(this Window owner, string message, string caption = null, MessageBoxButton button = MessageBoxButton.OK, MessageBoxImage icon = MessageBoxImage.None)
        {
            owner.Topmost = true;
            MessageBox.Show(owner, message, caption, button, icon);
            owner.Topmost = false;
        }
    }
}
