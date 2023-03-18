/*
 * 작성자: 윤정도
 * 생성일: 3/18/2023 5:20:11 PM
 *
 */

using System;
using System.Collections.Generic;
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
using SGToolsUI.ViewModel;

namespace SGToolsUI.Model.Backup
{
    public class BackupFolder
    {
        public BackupFolder(string folderName, BackupViewModel viewModel)
        {
            FolderName = folderName;
            ViewModel = viewModel;
        }

        public string FolderName { get; }
        public BackupViewModel ViewModel { get; }
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
