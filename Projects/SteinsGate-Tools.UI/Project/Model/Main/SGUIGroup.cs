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
using MoreLinq;
using Newtonsoft.Json.Linq;
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
        private int code_;
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
                    return null;

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

        [Browsable(false)] public override int Code => code_;
        [Browsable(false)] public bool HasOnlyGroup => Where(element => !element.IsGroup).Any() == false;
        [Browsable(false)] public bool HasOnlyGroupRecursive => WhereRecursive(element => !element.IsGroup).Any() == false;
        [Browsable(false)] public override bool Manipulatable => true;

        //////////////////////////////////////////////////////////////////////////////////
        public void SetCode(int _code) => code_ = _code;

        //////////////////////////////////////////////////////////////////////////////////
        public void SetDepth(int _depth)
        {
            Debug.Assert(depth_ < SGUIExporter.DepthStrings.Length, "뎁쓰가 너무 깊습니다.");
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

        //////////////////////////////////////////////////////////////////////////////////
        public override object Clone()
        {
            SGUIGroup group = new SGUIGroup();
            group.visualSize_ = visualSize_;
            group.CopyFrom(this);

            for (int i = 0; i < children_.Count; ++i)
            {
                SGUIElement cloned = children_[i].Clone() as SGUIElement;
                if (cloned == null)
                    throw new Exception("클론한 갹체가 null입니다.");
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
        public void AddChild(SGUIElement _newChild, PropertyReflect _updateProperty = PropertyReflect.Update)
        {
            Children.Add(_newChild);
            _newChild.Parent = this;
            ViewModel.GroupMaster.UpdateZOrder();

            SGUIGroupMaster groupMaster = ViewModel.GroupMaster;

            bool isPicked = Picked;  // 현재 그룹이 픽된 경우 자식도 픽상태
            if (isPicked)
            {
                _newChild.SetPick(true);
                groupMaster.PickedElements.Add(_newChild);
            }

            if (_newChild.IsGroup)
            {
                SGUIGroup newGroup = _newChild.Cast<SGUIGroup>();

                groupMaster.AddGroup(newGroup);
                newGroup.SetDepth(Depth + 1);
            }
            else
            {
                groupMaster.AddElement(_newChild);
            }

            if (_updateProperty == PropertyReflect.Update)
            {
                if (isPicked)
                {
                    groupMaster.OnPropertyChanged(SGUIGroupMaster.PickedElementKey);
                    groupMaster.OnPropertyChanged(SGUIGroupMaster.HasPickedElementKey);
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
            SGUIGroupMaster groupMaster = ViewModel.GroupMaster;

            // 부모 업데이트를 먼저 수행해줘야한다.
            // 아래의 groupMaster.PickedElements.AddRange를 수행하면서 CollectionViewSource에서 Code를 기준으로 오름차순 정렬을 하는데
            // 부모가 업데이트 되지 않은 상태에서는 Code가 -1로 고정되어버리기 떄문이다.

            // 새로 추가된 자식들중 바로 다음 계층(깊이)의 자식들만 부모를 업데이트해줌 된다.
            // 하지만 다음, 다음 계층의 경우 깊이와, 그룹이 존재하는 경우 업데이트 해줘야하므로
            _newChildren.ForEach(newChild => newChild.Parent = this);
            _newChildren.ForEachRecursive(newChild =>
            {
                if (!newChild.IsGroup)
                {
                    groupMaster.AddElement(newChild);
                    return;
                }

                SGUIGroup newGroup = newChild.Cast<SGUIGroup>();
                groupMaster.AddGroup(newGroup);
                newGroup.SetDepth(newGroup.Parent.Depth + 1);
            });

            // 중간에 넣는 작업은 데이터를 계속 밀기땜에 그냥 새로 만들어서 교체해주자.
            // PickedElements.AddRange이전에 수행을 무조건 해줘야한다.
            // CollectionViewSource의 SortDescriptor로 Code를 설정해놨는데 Code는 Index프로퍼티를 사용하고
            // Index 프로퍼티는 _children에 새로 추가된 children이 포함되어야지 올바르게 동작하기 떄문이다.
            children_ = children_.InsertRangeNew(_index, _newChildren.Children);
            ViewModel.GroupMaster.UpdateZOrder();

            // 현재 그룹이 픽된 경우 추가되는 자식들도 픽해줘야함.
            bool isPicked = Picked;
            if (isPicked)
            {
                _newChildren.ForEach(element => element.SetPick(true));

                // https://stackoverflow.com/questions/7284805/how-to-sort-observablecollection
                // PickedElements와 트리뷰의 엘리먼트들간에 정렬순서가 일치해야한다.
                // 이때 Depth순으로 오름차순된 상태에서 Depth가 동일하다면 코드순으로 정렬되어야한다.
                // 여기서 이렇게 막 추가해주더라도 내가 MainView에서 CollectionViewSource로 정렬순서를 정해줬기때문에
                // 내부적으로 알아서 정렬을 수행하게 된다.
                groupMaster.PickedElements.AddRange(_newChildren.Children);
            }

            OnPropertyChanged(nameof(Children));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override JObject ToJObject()
        {
            JObject root = base.ToJObject();
            root[JsonVisualSizeKey] = visualSize_.ToFullString();
            JArray children = new JArray();

            for (int i = 0; i < children_.Count; ++i)
            {
                SGUIElement e = children_[i];
                children.Add($"{e.Code} {e.RelativePosition.ToFullString()}");
            }

            root[JsonChildrenKey] = children;
            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void ParseJObject(JObject _root)
        {
            base.ParseJObject(_root);
            visualSize_ = SizeEx.ParseFullString((string)_root[JsonVisualSizeKey]);
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
