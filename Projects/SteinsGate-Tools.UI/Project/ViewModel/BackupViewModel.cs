/*
 * 작성자: 윤정도
 * 생성일: 3/18/2023 4:58:29 PM
 *
 */

using System.Collections.Generic;
using System.IO;
using System.Linq;
using SGToolsCommon;
using SGToolsCommon.Extension;
using SGToolsUI.Model.Backup;

using Path = System.IO.Path;

namespace SGToolsUI.ViewModel
{
    public class BackupViewModel : Bindable
    {
        private BackupFolder selectedFolder_;
        private BackupFile selectedFile_;

        //////////////////////////////////////////////////////////////////////////////////
        public BackupViewModel(MainViewModel _mainViewModel)
        {
            selectedFolder_ = new BackupFolder(string.Empty, this);
            selectedFile_ = new BackupFile(string.Empty, this);
            MainViewModel = _mainViewModel;
        }

        public BackupFolder SelectedFolder
        {
            get => selectedFolder_;
            set
            {
                selectedFolder_ = value;
                OnPropertyChanged();
            }
        }

        public BackupFile SelectedFile
        {
            get => selectedFile_;
            set
            {
                selectedFile_ = value;
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
    }
}
