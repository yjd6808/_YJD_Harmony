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
using SGToolsCommon;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model
{
    public class SGUIGroup : SGUIElement
    {
        public const string CategoryName = "그룹";
        public const int CategoryOrder = 2;

        public const int OrderVerticalAlignment = 1;
        public const int OrderHorizontalAlignment = 1;
        public const int OrderChildCount = 3;
        public const int OrderChildCountRecursive = 4;


        public SGUIGroup(int depth)
        {
            _depth = depth;
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


        [Category(CategoryName), DisplayName("자식 수"), PropertyOrder(OrderChildCount)]
        [Description("UI 그룹의 자식수를 의미합니다.")]
        public int ChildCount => Children.Count;

        [Category(CategoryName), DisplayName("자식 수(Total)"), PropertyOrder(OrderChildCountRecursive)]
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

        [Category(CategoryName), DisplayName("가로 정렬"), PropertyOrder(OrderHorizontalAlignment)]
        [Description("UI 그룹내 자식의 가로 정렬 기준입니다.")]
        public HorizontalAlignment HorizontalAlignment
        {
            get => _horizontalAlignment;
            set
            {
                if (_horizontalAlignment == value)
                    return;

                _horizontalAlignment = value;
                OnPropertyChanged();
            }
        }

        [Category(CategoryName), DisplayName("수직 정렬"), PropertyOrder(OrderVerticalAlignment)]
        [Description("UI 그룹내 자식의 세로 정렬 기준입니다.")]
        public VerticalAlignment VerticalAlignment
        {
            get => _verticalAlignment;
            set
            {
                if (_verticalAlignment == value)
                    return;

                _verticalAlignment = value;
                OnPropertyChanged();
            }
        }

        [Category(SGUIElement.CategoryName), DisplayName("보이기"), PropertyOrder(SGUIElement.OrderIsVisible)]
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

        [Category(SGUIElement.CategoryName), DisplayName("계층적 높이"), PropertyOrder(SGUIElement.OrderDepth)]
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
        

        private ObservableCollection<SGUIElement> _children = new();
        private HorizontalAlignment _horizontalAlignment = HorizontalAlignment.Left;
        private VerticalAlignment _verticalAlignment = VerticalAlignment.Bottom;
        private int _depth; // 계층 구조상 깊이. 추가한 이유: 깊이 계산시 연산 낭비가 심함. 특히 모든 원소 깊이를 계산하는 경우
        private int _code;

        [Browsable(false)] public override int Code => _code;
        public void SetCode(int code) => _code = code;

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
            SGUIGroup group = new SGUIGroup(_depth);
            group.CopyFrom(this);
            group._code = _code;
            group.HorizontalAlignment = HorizontalAlignment;
            group.VerticalAlignment = VerticalAlignment;

            for (int i = 0; i < _children.Count; ++i)
            {
                SGUIElement cloned = _children[i].Clone() as SGUIElement;
                if (cloned == null)
                    throw new Exception("클론한 갹체가 null입니다.");
                group._children.Add(cloned);
            }

            return group;
        }

        public void ForEachRecursive(Action<SGUIElement> action)
        {
            for (int i = 0; i < _children.Count; ++i)
            {
                SGUIElement element = _children[i];
                action(element);

                if (element.UIElementType == SGUIElementType.Group)
                {
                    ((SGUIGroup)element).ForEachRecursive(action);
                    continue;
                }
            }
        }

        public void ForEach(Action<SGUIElement> action)
        {
            action(this);

            _children.ForEach(action);
        }


        public void AddChild(SGUIElement child, PropertyReflect updateProperty = PropertyReflect.Update)
        {
            Children.Add(child);

            SGUIGroupMaster groupMaster = ViewModel.GroupMaster;

            bool isPicked = Picked;  // 현재 그룹이 픽된 경우 자식도 픽상태
            if (isPicked)
            {
                child.SetPick(true);
                groupMaster.PickedElements.Add(child);
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


        // 디버깅용
        public void ____Update()
        {
            ViewModel.GroupMaster.AddGroup(this);
            Children.ForEach(x =>
            {
                x.Parent = this;

                if (x.IsGroup)
                    x.Cast<SGUIGroup>().____Update();
            });
        }

        
    }
}
