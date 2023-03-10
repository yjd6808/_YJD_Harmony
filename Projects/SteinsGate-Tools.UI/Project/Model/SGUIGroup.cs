/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 9:16:47 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Automation.Peers;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using MoreLinq;
using Newtonsoft.Json.Linq;
using SGToolsCommon;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsUI.File;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model
{


    [CategoryOrder(Constant.GroupCategoryName, Constant.OtherCategoryOrder)]
    public class SGUIGroup : SGUIElement
    {
        public const int OrderChildCount = 1;
        public const int OrderChildCountRecursive = 2;


        public SGUIGroup()
        {
            _children = new ObservableCollection<SGUIElement>();
            _canvasSelectable = false;
        }

        public SGUIGroup(int capacity)
        {
            _children = new ObservableCollection<SGUIElement>(new List<SGUIElement>(capacity));
            _canvasSelectable = false;
        }

        // ============================================================
        //            프로파티
        // ============================================================


        [Browsable(false)]
        public ObservableCollection<SGUIElement> Children
        {
            get => _children;
            set
            {
                _children = value;
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
                int Count = 0;
                ForEachRecursive((_) => Count++);
                return Count;
            }
        }


        [Category(Constant.ElementCategoryName), DisplayName("보이기"), PropertyOrder(SGUIElement.OrderIsVisible)]
        [Description("현재 엘리먼트를 캔버스상에서 표시될지를 결정합니다. (그룹 요소입니다. 하위 항목들도 모두 적용됩니다.)")]
        public override bool IsVisible
        {
            get => _visible;
            set
            {
                if (_visible == value)
                    return;

                _visible = value;
                ForEachRecursive(element => element.IsVisible = value);
                OnPropertyChanged();
            }
        }

        [Category(Constant.ElementCategoryName), DisplayName("계층적 높이"), PropertyOrder(SGUIElement.OrderDepth)]
        [Description("이 엘리먼트의 계층구조상 높이")]
        public override int Depth => _depth;


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
        
        [Browsable(false)] public override int Code => _code;
        [Browsable(false)] public bool HasOnlyGroup => Where(element => !element.IsGroup).Any() == false;
        [Browsable(false)] public bool HasOnlyGroupRecursive => WhereRecursive(element => !element.IsGroup).Any() == false;

        public void SetCode(int code) => _code = code;
        public void SetDepth(int depth) => _depth = depth;
        

        public static int NameSeq = 0;
        // ============================================================
        //            기능
        // ============================================================
        public override void CreateInit()
        {
            VisualName = $"그룹_{NameSeq++}";
        }

        public override object Clone()
        {
            SGUIGroup group = new SGUIGroup();
            group.CopyFrom(this);
            
            for (int i = 0; i < _children.Count; ++i)
            {
                SGUIElement cloned = _children[i].Clone() as SGUIElement;
                if (cloned == null)
                    throw new Exception("클론한 갹체가 null입니다.");
                group._children.Add(cloned);
                cloned.Parent = group;
            }

            return group;
        }


        public void ForEach(Action<SGUIElement> action)
            => _children.ForEach(action);

        public void ForEachRecursive(Action<SGUIElement> action)
        {
            for (int i = 0; i < _children.Count; ++i)
            {
                SGUIElement element = _children[i];
                action(element);

                if (element.IsGroup)
                    element.Cast<SGUIGroup>().ForEachRecursive(action);
            }
        }

        IEnumerable<SGUIElement> Where(Predicate<SGUIElement> predicate)
        {
            for (int i = 0; i < _children.Count; ++i)
                if (predicate(_children[i]))
                    yield return _children[i];
        }

        IEnumerable<SGUIElement> WhereRecursive(Predicate<SGUIElement> predicate)
        {
            for (int i = 0; i < _children.Count; ++i)
            {
                SGUIElement element = _children[i];

                if (predicate(element))
                    yield return element;

                if (element.IsGroup)
                    element.Cast<SGUIGroup>().WhereRecursive(predicate);
            }
        }




        public void AddChild(SGUIElement newChild, PropertyReflect updateProperty = PropertyReflect.Update)
        {
            Children.Add(newChild);
            newChild.Parent = this;
            SGUIGroupMaster groupMaster = ViewModel.GroupMaster;

            bool isPicked = Picked;  // 현재 그룹이 픽된 경우 자식도 픽상태
            if (isPicked)
            {
                newChild.SetPick(true);
                groupMaster.PickedElements.Add(newChild);
            }

            if (newChild.IsGroup)
            {
                SGUIGroup newGroup = newChild.Cast<SGUIGroup>();

                groupMaster.AddGroup(newGroup);
                newGroup.SetDepth(Depth + 1);
            }
            else
            {
                groupMaster.AddElement(newChild);
            }

            if (updateProperty == PropertyReflect.Update)
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
        public void InsertChildren(SGUIGroup newChildren, int index)
        {
            SGUIGroupMaster groupMaster = ViewModel.GroupMaster;

            // 현재 그룹이 픽된 경우 추가되는 자식들도 픽해줘야함.
            bool isPicked = Picked;
            if (isPicked)
            {
                newChildren.ForEach(element => element.SetPick(false));
                groupMaster.PickedElements.AddRange(newChildren.Children);
            }

            // 새로 추가된 자식들중 바로 다음 계층(깊이)의 자식들만 부모를 업데이트해줌 된다.
            // 하지만 다음, 다음 계층의 경우 깊이와, 그룹이 존재하는 경우 업데이트 해줘야하므로
            newChildren.ForEach(newChild => newChild.Parent = this);
            newChildren.ForEachRecursive(newChild =>
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
            _children = _children.InsertRangeNew(index, newChildren.Children);
            OnPropertyChanged(nameof(Children));
        }

        // 디버깅용, new로 임시로 생성한 데이터에 정보 주입해주기 위함
        public void DebugUpdate()
        {
            

            VisualSize = new Size(Constant.ResolutionWidth, Constant.ResolutionHeight);

            if (!IsMaster)
            {
                ViewModel.GroupMaster.AddGroup(this);
                SetDepth(Parent.Depth + 1);
            }

            Children.ForEach(x =>
            {
                x.Parent = this;

                if (x.IsGroup)
                    x.Cast<SGUIGroup>().DebugUpdate();
                else
                    ViewModel.GroupMaster.AddElement(x);
            });
        }

        public override JObject ToJObject(SaveMode mode)
        {
            JObject root = new JObject();
            JArray children = new JArray();
            CopyFrom(root, mode);

            for (int i = 0; i < _children.Count; ++i)
            {
                SGUIElement e = _children[i];
                children.Add($"{e.Code} {e.RelativePosition.ToFullString()}");
            }

            root["children"] = children;
            return root;
        }

        private ObservableCollection<SGUIElement> _children;
        private int _code;
        private int _depth; // 계층 구조상 깊이. 추가한 이유: 깊이 계산시 연산 낭비가 심함. 특히 모든 원소 깊이를 계산하는 경우
    }
}

