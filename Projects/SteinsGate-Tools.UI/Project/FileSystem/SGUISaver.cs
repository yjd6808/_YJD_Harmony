using System;
using System.Threading.Tasks;
using System.Xml.Linq;
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.FileSystem
{
    public class SGUISaver : SGUIFileSystem
    {
        public MainViewModel ViewModel { get; }

        public SGUISaver(MainViewModel _viewModel)
        {
            ViewModel = _viewModel;
        }

        public async Task SaveAsync(string _filePath, SGUIRootGroup _rootGroup)
        {
            await Task.Run(() =>
            {
                XElement rootXml = _rootGroup.ToXElement();
                rootXml.Save(_filePath);
            });
        }

        public async Task BackupAsync(string _tag)
        {
            string backupDir = System.IO.Path.Combine(
                Environment.CurrentDirectory,
                BackupDirectoryRoot,
                DateTime.Now.ToString("yyyy-MM-dd"));
            System.IO.Directory.CreateDirectory(backupDir);

            string backupPath = System.IO.Path.Combine(
                backupDir,
                $"{DateTime.Now:HH.mm.ss}_{_tag}.xml");

            await SaveAsync(backupPath, ViewModel.RootGroup);
        }
    }
}