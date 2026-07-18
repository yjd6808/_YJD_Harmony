using System;
using System.Collections.Generic;
using System.IO;
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.FileSystem
{
    public class SGUIProjectManager
    {
        public string BaseDirectory { get; }
        private MainViewModel ViewModel { get; }

        public SGUIProjectManager(string _baseDirectory, MainViewModel _viewModel)
        {
            BaseDirectory = _baseDirectory;
            ViewModel = _viewModel;
        }

        public WorkspaceTreeItem ScanDirectory()
        {
            var root = new WorkspaceTreeItem
            {
                Name = Path.GetFileName(BaseDirectory),
                IsDirectory = true,
                IsExpanded = true
            };

            ScanRecursive(BaseDirectory, root);
            return root;
        }

        private void ScanRecursive(string _dirPath, WorkspaceTreeItem _parent)
        {
            var dirInfo = new DirectoryInfo(_dirPath);

            foreach (var subDir in dirInfo.GetDirectories())
            {
                var dirItem = new WorkspaceTreeItem
                {
                    Name = subDir.Name,
                    IsDirectory = true,
                    IsExpanded = false
                };
                _parent.Children.Add(dirItem);
                ScanRecursive(subDir.FullName, dirItem);
            }

            foreach (var metaFile in dirInfo.GetFiles("*.uimeta"))
            {
                SGUIMetaInfo metaInfo;
                try
                {
                    metaInfo = SGUIMetaManager.LoadMeta(metaFile.FullName);
                }
                catch
                {
                    continue;
                }

                string xmlFullPath = Path.GetFullPath(Path.Combine(metaFile.DirectoryName!, metaInfo.XmlPath));

                try
                {
                    var loader = new SGUILoader(ViewModel);
                    SGUIRootGroup rootGroup = loader.Load(xmlFullPath);
                    rootGroup.XmlFilePath = xmlFullPath;
                    _parent.Children.Add(rootGroup);
                }
                catch { }
            }
        }
    }
}
