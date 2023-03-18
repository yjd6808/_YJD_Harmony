/*
 * 작성자: 윤정도
 * 생성일: 3/18/2023 5:27:01 PM
 *
 */

using System;
using System.IO;
using System.Collections.Generic;
using System.Diagnostics;
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
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Model.Backup
{
    public class BackupFile
    {
        public BackupFile(string path, BackupViewModel backUpViewModel)
        {
            Path = path;
            ViewModel = backUpViewModel;
        }

        public string Path { get; }
        public string Name => System.IO.Path.GetFileNameWithoutExtension(Path);
        public BackupViewModel ViewModel { get; }

        public BackupTreeViewItemGroup Root
        {
            get
            {
                BackupTreeViewItemGroup root = new BackupTreeViewItemGroup();

                if (Path.Length == 0)
                    return root;

                bool success = ViewModel.MainViewModel.Loader.Load(Path, root);
                Debug.Assert(success, "백업 파일로딩 실패");
                return root;
            }
        }
    }
}
