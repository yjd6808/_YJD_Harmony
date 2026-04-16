/*
 * 작성자: 윤정도
 *
 */

using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using SGToolsCommon.Extension;
using SGToolsUI.Model.Backup;
using SGToolsUI.ViewModel;

namespace SGToolsUI.View
{
    public class BackUpItem
    {
    }

    public partial class BackupView : Window
    {
        public BackupViewModel ViewModel { get; }

        //////////////////////////////////////////////////////////////////////////////////
        public BackupView(MainViewModel _mainViewModel)
        {
            ViewModel = new BackupViewModel(_mainViewModel);
            InitializeComponent();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void BackupFolderListBox_OnSelectionChanged(object _sender, SelectionChangedEventArgs _e)
        {
            if (BackupFolderListBox.SelectedItems.Count == 0)
                return;

            ViewModel.SelectedFolder = (BackupFolderListBox.SelectedItems[0] as BackupFolder)!;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void BackupFileListBox_OnSelectionChanged(object _sender, SelectionChangedEventArgs _e)
        {
            if (BackupFileListBox.SelectedItems.Count == 0)
                return;

            ViewModel.SelectedFile = (BackupFileListBox.SelectedItems[0] as BackupFile)!;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private async void BackupFileListBox_OnPreviewMouseDoubleClick(object _sender, MouseButtonEventArgs _e)
        {
            if (BackupFileListBox.SelectedItems.Count == 0)
                return;

            BackupFile? file = BackupFileListBox.SelectedItems[0] as BackupFile;
            if (file == null)
                return;

            if (MessageBoxEx.ShowTopMost($"{file.Name} 파일을 로딩하시겠습니까?", "질문", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.No)
                return;

            TitleBar.Close();
            await ViewModel.MainViewModel.Saver.BackupAsync("백업뷰 로딩");
            ViewModel.MainViewModel.GroupMaster = await ViewModel.MainViewModel.Loader.LoadAsync(file.Path);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void RefreshButton_OnClick(object _sender, RoutedEventArgs _e)
        {
            ViewModel.NotifyProperty("Folders");
            ViewModel.SelectedFolder = new BackupFolder(string.Empty, ViewModel);
            ViewModel.SelectedFile = new BackupFile(string.Empty, ViewModel);
        }
    }
}
