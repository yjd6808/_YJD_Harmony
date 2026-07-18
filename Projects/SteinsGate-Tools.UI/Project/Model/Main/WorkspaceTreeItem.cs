using System.Collections.ObjectModel;
using System.Windows.Media;
using SGToolsCommon;

namespace SGToolsUI.Model.Main
{
    public class WorkspaceTreeItem : Bindable
    {
        private string name_ = "";
        private bool isDirectory_;
        private bool isExpanded_ = true;
        private ImageSource? icon_;
        private SGUIRootGroup? rootGroup_;

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

        public bool IsLoaded { get; set; }

        public SGUIRootGroup? RootGroup
        {
            get => rootGroup_;
            set { rootGroup_ = value; OnPropertyChanged(); }
        }

        public ImageSource? Icon
        {
            get => icon_;
            set { icon_ = value; OnPropertyChanged(); }
        }

        public ObservableCollection<object> Children { get; set; } = new();
    }
}
