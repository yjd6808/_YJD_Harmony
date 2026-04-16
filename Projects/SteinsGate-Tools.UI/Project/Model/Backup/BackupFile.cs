/*
 * 작성자: 윤정도
 * 생성일: 3/18/2023 5:27:01 PM
 *
 */

using System.Diagnostics;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Model.Backup
{
    public class BackupFile
    {
        public string Path { get; }
        public string Name => System.IO.Path.GetFileNameWithoutExtension(Path);
        public BackupViewModel ViewModel { get; }

        //////////////////////////////////////////////////////////////////////////////////
        public BackupFile(string _path, BackupViewModel _backUpViewModel)
        {
            Path = _path;
            ViewModel = _backUpViewModel;
        }

        //////////////////////////////////////////////////////////////////////////////////
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