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
        public BackupView(MainViewModel mainViewModel)
        {
            ViewModel = new BackupViewModel(mainViewModel);
            InitializeComponent();
        }

        private void BackupFolderListBox_OnSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (BackupFolderListBox.SelectedItems.Count == 0)
                return;

            ViewModel.SelectedFolder = BackupFolderListBox.SelectedItems[0] as BackupFolder;
        }

        private void BackupFileListBox_OnSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (BackupFileListBox.SelectedItems.Count == 0)
                return;

            ViewModel.SelectedFile = BackupFileListBox.SelectedItems[0] as BackupFile;
        }

        private async void BackupFileListBox_OnPreviewMouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (BackupFileListBox.SelectedItems.Count == 0)
                return;

            BackupFile file = BackupFileListBox.SelectedItems[0] as BackupFile;
            if (file == null)
                return;

            if (MessageBoxEx.ShowTopMost($"{file.Name} 파일을 로딩하시겠습니까?", "질문", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.No)
                return;

            TitleBar.Close();
            await ViewModel.MainViewModel.Saver.BackupAsync("백업뷰 로딩");
            ViewModel.MainViewModel.GroupMaster = await ViewModel.MainViewModel.Loader.LoadAsync(file.Path);
        }

        private void RefreshButton_OnClick(object sender, RoutedEventArgs e)
        {
            ViewModel.NotifyProperty("Folders");
            ViewModel.SelectedFolder = new BackupFolder(string.Empty, ViewModel);
            ViewModel.SelectedFile = new BackupFile(string.Empty, ViewModel);
        }
    }
}

