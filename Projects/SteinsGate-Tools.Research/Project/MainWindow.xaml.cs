using System;
using System.Windows;
using SGToolsCommon.Extension;

namespace SGToolsResearch
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OpenResearch(Research _research)
        {
            if (_research == null)
            {
                MessageBoxEx.ShowTopMost("Research가 null입니다.", "오류", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }

            Window window = _research.OpenWindow();
            window.Owner = this;
            window.Show();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnBtnRTBLogViewer_Click(object _sender, RoutedEventArgs _e)
        {
            OpenResearch(new RTBLogViewerResearch());
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnBtnAny_Click(object _sender, RoutedEventArgs _e)
        {
            OpenResearch(new AnyResearch());
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnBtnExit_Click(object _sender, RoutedEventArgs _e)
        {
            Close();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void OnCloseWindow(object _sender, System.Windows.Input.ExecutedRoutedEventArgs _e)
        {
            Close();
        }
    }
}
