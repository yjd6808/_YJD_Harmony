using System;
using System.Threading.Tasks;
using System.Xml.Linq;
using SGToolsUI.Model.Main;
using SGToolsUI.ViewModel;

namespace SGToolsUI.FileSystem
{
    public class SGUILoader : SGUIFileSystem
    {
        public MainViewModel ViewModel { get; }

        public SGUILoader(MainViewModel _viewModel)
        {
            ViewModel = _viewModel;
        }

        public async Task<SGUIRootGroup> LoadAsync(string _filePath)
        {
            return await Task.Run(() =>
            {
                XElement root = XElement.Load(_filePath);
                SGUIRootGroup rootGroup = ParseRootGroup(root);
                rootGroup.ViewModel = ViewModel;
                return rootGroup;
            });
        }

        private SGUIRootGroup ParseRootGroup(XElement _root)
        {
            string rootName = (string)_root.Attribute("name")!;
            SGUIRootGroup rootGroup = SGUIRootGroup.Create(ViewModel);
            rootGroup.VisualName = rootName;

            foreach (var childElement in _root.Elements())
            {
                if (childElement.Name.LocalName == "data")
                    continue;

                SGUIElement? child = ParseElement(childElement);
                if (child != null)
                {
                    child.ViewModel = ViewModel;
                    rootGroup.Children.Add(child);
                }
            }

            return rootGroup;
        }

        private SGUIElement? ParseElement(XElement _element)
        {
            string tagName = _element.Name.LocalName;
            SGUIElementType type = TagNameToElementType(tagName);
            SGUIElement? element = SGUIElement.Create(type);
            if (element != null)
            {
                element.ParseXElement(_element);
                element.ViewModel = ViewModel;

                if (element is SGUIGroup group)
                {
                    foreach (var childElem in _element.Elements())
                    {
                        if (childElem.Name.LocalName == "data")
                            continue;
                        SGUIElement? child = ParseElement(childElem);
                        if (child != null)
                        {
                            child.ViewModel = ViewModel;
                            group.Children.Add(child);
                        }
                    }
                }
            }
            return element;
        }

        private SGUIElementType TagNameToElementType(string _tagName)
        {
            return _tagName switch
            {
                "Group" => SGUIElementType.Group,
                "Button" => SGUIElementType.Button,
                "Label" => SGUIElementType.Label,
                "Sprite" => SGUIElementType.Sprite,
                "EditBox" => SGUIElementType.EditBox,
                "CheckBox" => SGUIElementType.CheckBox,
                "ToggleButton" => SGUIElementType.ToggleButton,
                "ScrollBar" => SGUIElementType.ScrollBar,
                "ProgressBar" => SGUIElementType.ProgressBar,
                "Static" => SGUIElementType.Static,
                _ => throw new Exception($"Unknown element tag: {_tagName}")
            };
        }
    }
}