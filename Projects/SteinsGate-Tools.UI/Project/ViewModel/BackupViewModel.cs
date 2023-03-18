/*
 * 작성자: 윤정도
 * 생성일: 3/18/2023 4:58:29 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Configuration;
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
using System.Windows.Navigation;
using System.Windows.Shapes;
using Microsoft.Xaml.Behaviors.Core;
using SGToolsCommon;
using SGToolsCommon.Extension;
using SGToolsUI.Model.Backup;
using SGToolsUI.View;
using Vanara.PInvoke;
using Path = System.IO.Path;

namespace SGToolsUI.ViewModel
{
    

    public class BackupViewModel : Bindable
    {
        public BackupViewModel(MainViewModel mainViewModel)
        {
            _selectedFolder = new BackupFolder(string.Empty, this);
            _selectedFile = new BackupFile(string.Empty, this);
            MainViewModel = mainViewModel;
        }

        public BackupFolder SelectedFolder
        {
            get => _selectedFolder;
            set
            {
                _selectedFolder = value;
                OnPropertyChanged();
            }
        }

        public BackupFile SelectedFile
        {
            get => _selectedFile;
            set
            {
                _selectedFile = value;
                OnPropertyChanged();
            }
        }

        public List<BackupFolder> Folders
        {
            get
            {
                List<string> backupDates = Directory.GetDirectories(Constant.BackupDirectoryRoot, "*", SearchOption.TopDirectoryOnly).ToList();
                backupDates = backupDates.Where(folder => DirectoryEx.IsDirectory(folder)).ToList();
                backupDates.Sort((s1, s2) => s1.CompareTo(s2) * -1);   // 오름차순 정렬 (최신)
                return backupDates.Select(name => new BackupFolder(Path.GetFileName(name), this)).ToList();
            }
        }


        public MainViewModel MainViewModel { get; }

        private BackupFolder _selectedFolder;
        private BackupFile _selectedFile;

    }
}


