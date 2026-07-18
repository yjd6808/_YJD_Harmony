using System.Collections.Generic;
using SGToolsUI.Model.Main;

namespace SGToolsUI.FileSystem
{
    public class ProjectFolderItem
    {
        public string Name { get; set; } = "";
        public bool IsFolder { get; set; }
        public SGUIMetaInfo? MetaInfo { get; set; }
        public SGUIRootGroup? RootGroup { get; set; }
        public List<ProjectFolderItem> Children { get; set; } = new();
    }

    public class SGUIProjectManager
    {
        public SGUIMetaManager MetaManager { get; }
        public List<ProjectFolderItem> FolderTree { get; private set; } = new();

        public SGUIProjectManager(string _baseDirectory)
        {
            MetaManager = new SGUIMetaManager(_baseDirectory);
        }

        public void ScanDirectory()
        {
            FolderTree.Clear();
            var metas = MetaManager.ScanAllMeta();
            foreach (var meta in metas)
            {
                var item = new ProjectFolderItem
                {
                    Name = meta.Name,
                    IsFolder = false,
                    MetaInfo = meta
                };
                FolderTree.Add(item);
            }
        }
    }
}