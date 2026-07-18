/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 9:16:47 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Xml.Linq;
using SGToolsCommon;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsUI.FileSystem;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{
    [RefreshProperties(RefreshProperties.All)]
    [CategoryOrder(Constant.GroupCategoryName, Constant.OtherCategoryOrder)]
    public class SGUIGroup : SGUIElement
    {
        public const int OrderChildCount = 1;
        public const int OrderChildCountRecursive = 2;

        private ObservableCollection<SGUIElement> children_;
        private IntSize visualSize_;
        private int depth_; // 계층 구조상 깊이. 추가한 이유: 깊이 계산시 연산 낭비가 심함. 특히 모든 원소 깊이를 계산하는 경우

        //////////////////////////////////////////////////////////////////////////////////
        public SGUIGroup()
        {
            children_ = new ObservableCollection<SGUIElement>();
            canvasSelectable_ = false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public SGUIGroup(int _capacity)
        {
            children_ = new ObservableCollection<SGUIElement>(new List<SGUIElement>(_capacity));
            canvasSelectable_ = false;
        }

        // ============================================================
        //            프로파티
        // ============================================================

        [Browsable(false)]
        public ObservableCollection<SGUIElement> Children
        {
            get => children_;
            set
            {
                children_ = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.GroupCategoryName), DisplayName("자식 수"), PropertyOrder(OrderChildCount)]
        [Description("UI 그룹의 자식수를 의미합니다.")]
        public int ChildCount => Children.Count;

        [Category(Constant.GroupCategoryName), DisplayName("자식 수(Total)"), PropertyOrder(OrderChildCountRecursive)]
        [Description("UI 그룹 자식수를 의미합니다. (하위 자식들 모두 포함)")]
        public int ChildCountRecursive
        {
            get
            {
                int count = 0;
                ForEachRecursive((_) => count++);
                return count;
            }
        }

        [ReadOnly(false)]
        [Category(Constant.ElementCategoryName), DisplayName("크기"), PropertyOrder(OrderVisualSize)]
        public override IntSize VisualSize
        {
            get => visualSize_;
            set
            {
                visualSize_ = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualRect));
            }
        }

        [Category(Constant.ElementCategoryName), DisplayName("보이기"), PropertyOrder(OrderIsVisible)]
        [Description("현재 엘리먼트를 캔버스상에서 표시될지를 결정합니다. (그룹 요소입니다. 하위 항목들도 모두 적용됩니다.)")]
        public override bool IsVisible
        {
            get => visible_;
            set
            {
                if (visible_ == value)
                    return;

                visible_ = value;
                ForEachRecursive(element => element.IsVisible = value);
                OnPropertyChanged();
            }
        }

        [Category(Constant.ElementCategoryName), DisplayName("계층적 높이"), PropertyOrder(OrderDepth)]
        [Description("이 엘리먼트의 계층구조상 높이")]
        public override int Depth => depth_;

        [Browsable(false)]
        public SGUIElement DeepestLastChild
        {
            get
            {
                if (ChildCount == 0)
                    return null!;

                SGUIElement lastChild = Children[ChildCount - 1];

                if (lastChild.IsGroup && lastChild.Item.IsExpanded)
                {
                    SGUIGroup lastChildGroup = lastChild.Cast<SGUIGroup>();
                    if (lastChildGroup.ChildCount > 0)
                        return lastChildGroup.DeepestLastChild;
                }

                return lastChild;
            }
        }

        [Browsable(false)] public override bool IsGroup => true;
        public override SGUIElementType UIElementType => SGUIElementType.Group;

        [Browsable(false)] public bool HasOnlyGroup => Where(element => !element.IsGroup).Any() == false;
        [Browsable(false)] public bool HasOnlyGroupRecursive => WhereRecursive(element => !element.IsGroup).Any() == false;
        [Browsable(false)] public override bool Manipulatable => true;

        //////////////////////////////////////////////////////////////////////////////////
        public void SetDepth(int _depth)
        {
            Debug.Assert(depth_ < 100, "뎁쓰가 너무 깊습니다.");
            depth_ = _depth;
        }

        public static int NameSeq = 0;

        // ============================================================
        //            기능
        // ============================================================

        //////////////////////////////////////////////////////////////////////////////////
        public override void CreateInit()
        {
            VisualName = $"그룹_{NameSeq++}";
        }

        private Dictionary<string, string>? dataMap_;

        //////////////////////////////////////////////////////////////////////////////////
        public Dictionary<string, string> DataMap
        {
            get
            {
                if (dataMap_ == null)
                    dataMap_ = new Dictionary<string, string>();
                return dataMap_;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override string GetElementTagName() => "Group";

        //////////////////////////////////////////////////////////////////////////////////
        public override XElement ToXElement()
        {
            XElement root = base.ToXElement();
            if (visualSize_.Width > 0 || visualSize_.Height > 0)
            {
                root.SetAttributeValue("width", visualSize_.Width);
                root.SetAttributeValue("height", visualSize_.Height);
            }
            foreach (var child in Children)
                root.Add(child.ToXElement());
            if (dataMap_ != null && dataMap_.Count > 0)
                root.Add(DataMapToXElement(dataMap_));
            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void ParseXElement(XElement _root)
        {
            base.ParseXElement(_root);
            XAttribute widthAttr = _root.Attribute("width");
            if (widthAttr != null)
                visualSize_.Width = (int)widthAttr;
            XAttribute heightAttr = _root.Attribute("height");
            if (heightAttr != null)
                visualSize_.Height = (int)heightAttr;
            foreach (var childElement in _root.Elements())
            {
                if (childElement.Name.LocalName == "data")
                {
                    ParseDataMap(childElement);
                    continue;
                }
                SGUIElement child = Create(childElement);
                child.Parent = this;
                child.ViewModel = ViewModel;
                child.ParseXElement(childElement);
                Children.Add(child);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void AddChild(SGUIElement _newChild, PropertyReflect _updateProperty = PropertyReflect.Update)
        {
            Children.Add(_newChild);
            _newChild.Parent = this;

            bool isPicked = Picked;
            if (isPicked)
            {
                _newChild.SetPick(true);
                ViewModel.RootGroup.PickedElements.Add(_newChild);
            }

            if (_newChild.IsGroup)
            {
                SGUIGroup newGroup = _newChild.Cast<SGUIGroup>();
                newGroup.SetDepth(Depth + 1);
            }

            if (_updateProperty == PropertyReflect.Update)
            {
                if (isPicked)
                {
                    ViewModel.RootGroup.OnPropertyChanged(SGUIRootGroup.PickedElementKey);
                    ViewModel.RootGroup.OnPropertyChanged(SGUIRootGroup.HasPickedElementKey);
                }

                OnPropertyChanged(nameof(ChildCount));
                OnPropertyChanged(nameof(ChildCountRecursive));
            }
        }

        // 0, 1, 2, 3
        // InsertChildren(children, 2)
        // 0, 1, Children, 2, 3
        // InsertChildren(children, 3)
        // 0, 1, 2, Children, 3
        // InsertChildren(children, 4)
        // 0, 1, 2, 3, Children
        //////////////////////////////////////////////////////////////////////////////////
        public void InsertChildren(SGUIGroup _newChildren, int _index)
        {
            SGUIRootGroup rootGroup = ViewModel.RootGroup;

            _newChildren.ForEach(newChild => newChild.Parent = this);
            _newChildren.ForEachRecursive(newChild =>
            {
                if (newChild.IsGroup)
                {
                    SGUIGroup newGroup = newChild.Cast<SGUIGroup>();
                    newGroup.SetDepth(newGroup.Parent.Depth + 1);
                }
            });

            children_ = children_.InsertRangeNew(_index, _newChildren.Children);

            bool isPicked = Picked;
            if (isPicked)
            {
                _newChildren.ForEach(element => element.SetPick(true));
                rootGroup.PickedElements.AddRange(_newChildren.Children);
            }

            OnPropertyChanged(nameof(Children));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override object Clone()
        {
            SGUIGroup group = new SGUIGroup();
            group.visualSize_ = visualSize_;
            group.CopyFrom(this);

            for (int i = 0; i < children_.Count; ++i)
            {
                SGUIElement? cloned = children_[i].Clone() as SGUIElement;
                if (cloned == null)
                    throw new Exception("클론한 객체가 null입니다.");
                group.children_.Add(cloned);
                cloned.Parent = group;
            }

            return group;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void ForEach(Action<SGUIElement> _action)
            => children_.ForEach(_action);

        //////////////////////////////////////////////////////////////////////////////////
        public void ForEachRecursive(Action<SGUIElement> _action)
        {
            for (int i = 0; i < children_.Count; ++i)
            {
                SGUIElement element = children_[i];
                _action(element);

                if (element.IsGroup)
                    element.Cast<SGUIGroup>().ForEachRecursive(_action);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private IEnumerable<SGUIElement> Where(Predicate<SGUIElement> _predicate)
        {
            for (int i = 0; i < children_.Count; ++i)
                if (_predicate(children_[i]))
                    yield return children_[i];
        }

        //////////////////////////////////////////////////////////////////////////////////
        private IEnumerable<SGUIElement> WhereRecursive(Predicate<SGUIElement> _predicate)
        {
            for (int i = 0; i < children_.Count; ++i)
            {
                SGUIElement element = children_[i];

                if (_predicate(element))
                    yield return element;

                if (element.IsGroup)
                    element.Cast<SGUIGroup>().WhereRecursive(_predicate);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static SGUIElement Create(XElement _element)
        {
            string tagName = _element.Name.LocalName;
            SGUIElementType type = tagName switch
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
                _ => throw new Exception($"알 수 없는 XML 태그: {tagName}")
            };
            return SGUIElement.Create(type);
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static XElement DataMapToXElement(Dictionary<string, string> dataMap)
        {
            XElement dataElement = new XElement("data");
            foreach (var kvp in dataMap)
            {
                XElement entry = new XElement(kvp.Key);
                entry.SetValue(kvp.Value);
                dataElement.Add(entry);
            }
            return dataElement;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void ParseDataMap(XElement dataElement)
        {
            dataMap_ = new Dictionary<string, string>();
            foreach (var child in dataElement.Elements())
            {
                dataMap_[child.Name.LocalName] = child.Value;
            }
        }

        // 기본적으로 엘리먼트의 이벤트는 "전파"되도록한다.
        //////////////////////////////////////////////////////////////////////////////////
        public override bool OnMouseMove(IntPoint _p)
        {
            // 자기 자신 그룹은 셀프 호출
            base.OnMouseMove(_p);

            for (int i = Children.Count - 1; i >= 0; i--)
            {
                if (!Children[i].OnMouseMove(_p))
                    return false;
            }

            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override bool OnMouseDown(IntPoint _p)
        {
            // 자기 자신 그룹은 셀프 호출
            base.OnMouseDown(_p);

            for (int i = Children.Count - 1; i >= 0; i--)
            {
                if (!Children[i].OnMouseDown(_p))
                    return false;
            }

            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override bool OnMouseUp(IntPoint _p)
        {
            // 자기 자신 그룹은 셀프 호출
            base.OnMouseUp(_p);

            for (int i = Children.Count - 1; i >= 0; i--)
            {
                if (!Children[i].OnMouseUp(_p))
                    return false;
            }

            return true;
        }
    }
}
