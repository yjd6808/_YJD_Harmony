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
            return await Task.Run(() => Load(_filePath));
        }

        public SGUIRootGroup Load(string _filePath)
        {
            XElement root = XElement.Load(_filePath);
            SGUIRootGroup rootGroup = ParseRootGroup(root);
            rootGroup.ViewModel = ViewModel;
            return rootGroup;
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

                SGUIElement? child = ParseElement(childElement, rootGroup);
                if (child != null)
                    rootGroup.Children.Add(child);
            }

			rootGroup.UpdateZOrder();
			return rootGroup;
        }

        private SGUIElement? ParseElement(XElement _element, SGUIGroup _parent)
        {
            string tagName = _element.Name.LocalName;
            SGUIElementType type = TagNameToElementType(tagName);
            SGUIElement? element = SGUIElement.Create(type);
            if (element != null)
            {
                element.ViewModel = ViewModel;
                element.Parent = _parent;
                element.ParseXElement(_element);
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