using System;
using System.Collections.Generic;
using System.IO;
using SGToolsUI.Model.Main;

namespace SGToolsUI.FileSystem
{
    public class SGUIProjectManager
    {
        public string BaseDirectory { get; }

        public SGUIProjectManager(string _baseDirectory)
        {
            BaseDirectory = _baseDirectory;
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

                var metaItem = new WorkspaceTreeItem
                {
                    Name = metaInfo.Name,
                    IsDirectory = false,
                    MetaFilePath = metaFile.FullName,
                    XmlFilePath = xmlFullPath
                };
                _parent.Children.Add(metaItem);
            }
        }
    }
}
