using System;
using System.Threading.Tasks;
using System.Windows.Forms;
using SGToolsUI.FileSystem;
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;
using SGToolsCommon.Extension;

namespace SGToolsUI.Command.MainViewCommand
{
    public class OpenWorkspace : MainCommandAbstractAsync
    {
        public OpenWorkspace(MainViewModel _viewModel)
            : base(_viewModel, "워크스페이스를 엽니다.")
        {
        }

        public override async Task ExecuteAsync(object? _parameter)
        {
            string dirPath;

            if (_parameter is string path && !string.IsNullOrEmpty(path))
            {
                dirPath = path;
            }
            else
            {
                using var dialog = new FolderBrowserDialog();
                dialog.Description = "UI 워크스페이스 디렉토리 선택";
                dialog.ShowNewFolderButton = false;
                dialog.InitialDirectory = Environment.CurrentDirectory;

                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    dirPath = dialog.SelectedPath;
                }
                else
                {
                    return;
                }
            }

            ViewModel.LogBox.AddLog($"워크스페이스 열기: {dirPath}");

            var projectManager = new SGUIProjectManager(dirPath);
            WorkspaceTreeItem rootItem = await Task.Run(() => projectManager.ScanDirectory());

            ViewModel.WorkspaceRoot = rootItem;

            if (!ViewModel.RecentDirectories.Contains(dirPath))
                ViewModel.RecentDirectories.Insert(0, dirPath);
        }
    }
}
