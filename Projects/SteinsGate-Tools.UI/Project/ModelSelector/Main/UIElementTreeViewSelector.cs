using System;
using System.Windows;
using System.Windows.Controls;
using SGToolsCommon.Extension;
using SGToolsUI.Customize.Control;
using SGToolsUI.Model.Main;

namespace SGToolsUI.ModelSelector.Main
{
    public class UIElementTreeViewSelector : DataTemplateSelector
    {
        public static readonly UIElementTreeViewSelector Selector = new();

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            switch (item)
            {
                case WorkspaceTreeItem wti:
                    return (DataTemplate)Application.Current.FindResource(wti.IsDirectory ? "DirectoryTreeTemplate" : "MetaItemTreeTemplate");
                case SGUIGroup uiGroup:
                    return (DataTemplate)Application.Current.FindResource("GroupTreeViewTemplate");
                case SGUIElement uiElement:
                    return (DataTemplate)Application.Current.FindResource("ElementTreeViewTemplate");
            }

            throw new Exception("UIElementTreeViewSelector: 알 수 없는 타입입니다. " + item.GetType().Name);
        }
    }
}
