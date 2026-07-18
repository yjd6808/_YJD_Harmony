/*
 * 작성자: 윤정도
 * 생성일: 3/18/2023 5:27:01 PM
 *
 */

using System.Diagnostics;
using System.IO;
using System.Xml.Linq;
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

                if (File.Exists(Path))
                {
                    try
                    {
                        XElement xml = XElement.Load(Path);
                        root.VisualName = (string)xml.Attribute("name") ?? Name;
                        ParseChildren(xml, root);
                    }
                    catch
                    {
                        Debug.Assert(false, "백업 파일 로딩 실패");
                    }
                }

                return root;
            }
        }

        private void ParseChildren(XElement _parent, BackupTreeViewItemGroup _parentGroup)
        {
            foreach (var child in _parent.Elements())
            {
                if (child.Name.LocalName == "data")
                    continue;

                if (child.HasElements)
                {
                    var group = new BackupTreeViewItemGroup();
                    group.VisualName = (string)child.Attribute("name") ?? child.Name.LocalName;
                    _parentGroup.Children.Add(group);
                    ParseChildren(child, group);
                }
                else
                {
                    var item = new BackupTreeViewItem();
                    item.VisualName = (string)child.Attribute("name") ?? child.Name.LocalName;
                    _parentGroup.Children.Add(item);
                }
            }
        }
    }
}