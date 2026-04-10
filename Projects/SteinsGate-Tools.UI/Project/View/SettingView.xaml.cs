/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 8:43:20 AM
 *
 */

using System.ComponentModel;
using System.Windows;
using SGToolsUI.ViewModel;

namespace SGToolsUI.View
{
    /// <summary>
    /// Interaction logic for SettingView.xaml
    /// </summary>
    public partial class SettingView : Window
    {
        public SettingViewModel ViewModel { get; }
        public bool Result { get; set; } = false;

        //////////////////////////////////////////////////////////////////////////////////
        public SettingView()
        {
            ViewModel = new SettingViewModel();
            ViewModel.View = this;
            Resources.Add("ViewModel", ViewModel);
            InitializeComponent();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SettingView_OnClosing(object? _sender, CancelEventArgs _e)
        {
            ViewModel.Commander.Finalize();
        }
    }
}
