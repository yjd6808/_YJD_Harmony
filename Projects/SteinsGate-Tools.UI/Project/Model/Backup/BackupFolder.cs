/*
 * 작성자: 윤정도
 * 생성일: 3/18/2023 5:20:11 PM
 *
 */

using System.Collections.Generic;
using System.IO;
using System.Linq;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Model.Backup
{
    public class BackupFolder
    {
        public string FolderName { get; }
        public BackupViewModel ViewModel { get; }

        //////////////////////////////////////////////////////////////////////////////////
        public BackupFolder(string _folderName, BackupViewModel _viewModel)
        {
            FolderName = _folderName;
            ViewModel = _viewModel;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public List<BackupFile> Files
        {
            get
            {
                if (FolderName.Length == 0)
                    return new List<BackupFile>();

                string folderPath = Path.Combine(Constant.BackupDirectoryRoot, FolderName);
                return Directory.GetFiles(folderPath, "*.json", SearchOption.TopDirectoryOnly).Select(path => new BackupFile(path, ViewModel)).ToList();
            }
        }
    }
}
