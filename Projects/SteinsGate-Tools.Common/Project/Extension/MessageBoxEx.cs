/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 7:12:54 AM
 *
 */

using System;
using System.Windows;

namespace SGToolsCommon.Extension
{
    public static class MessageBoxEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        public static MessageBoxResult ShowTopMost(string _message, string _caption = null, MessageBoxButton _button = MessageBoxButton.OK, MessageBoxImage _icon = MessageBoxImage.None)
        {
            Window parentWindow = Application.Current.MainWindow;

            if (parentWindow == null)
                throw new Exception("Application의 메인윈도우가 null입니다.");

            parentWindow.Topmost = true;
            MessageBoxResult result = MessageBox.Show(parentWindow, _message, _caption, _button, _icon);
            parentWindow.Topmost = false;
            return result;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static MessageBoxResult ShowTopMost(Exception _e)
        {
            return ShowTopMost(_e.Message + "\n\n" + _e.StackTrace, "예외 발생", MessageBoxButton.OK, MessageBoxImage.Error);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ShowTopMostMessageBox(this Window _owner, string _message, string _caption = null, MessageBoxButton _button = MessageBoxButton.OK, MessageBoxImage _icon = MessageBoxImage.None)
        {
            _owner.Topmost = true;
            MessageBox.Show(_owner, _message, _caption, _button, _icon);
            _owner.Topmost = false;
        }
    }
}
