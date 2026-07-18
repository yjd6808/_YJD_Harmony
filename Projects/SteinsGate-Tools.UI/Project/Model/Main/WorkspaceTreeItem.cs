using System.Collections.ObjectModel;
using System.Windows.Controls;
using System.Windows.Media;
using SGToolsCommon;

namespace SGToolsUI.Model.Main
{
    public class WorkspaceTreeItem : Bindable
    {
        private string name_ = "";
        private bool isDirectory_;
        private bool isExpanded_ = true;
        private bool selected_;
        private ImageSource? icon_;

        public string Name
        {
            get => name_;
            set { name_ = value; OnPropertyChanged(); }
        }

        public string? MetaFilePath { get; set; }
        public string? XmlFilePath { get; set; }

        public bool IsDirectory
        {
            get => isDirectory_;
            set { isDirectory_ = value; OnPropertyChanged(); }
        }

        public bool IsExpanded
        {
            get => isExpanded_;
            set { isExpanded_ = value; OnPropertyChanged(); }
        }

        public bool Selected
        {
            get => selected_;
            set { selected_ = value; OnPropertyChanged(); }
        }

        public ImageSource? Icon
        {
            get => icon_;
            set { icon_ = value; OnPropertyChanged(); }
        }

        private TreeViewItem? treeViewItem_;

        public TreeViewItem? Item
        {
            get => treeViewItem_;
            set { treeViewItem_ = value; OnPropertyChanged(); }
        }

        public ObservableCollection<object> Children { get; set; } = new();
    }
}
