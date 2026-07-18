/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 7:54:27 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Xml.Linq;
using SGToolsCommon;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsCommon.Resource;
using SGToolsUI.FileSystem;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{
    [RefreshProperties(RefreshProperties.All)]
    [CategoryOrder(Constant.ElementCategoryName, Constant.ElementCategoryOrder)]
    public abstract class SGUIElement : CanvasElement, ICloneable, IComparer<SGUIElement>
    {
        public const string VisualNameKey = nameof(VisualName);
        public const string VisualSizeKey = nameof(VisualSize);

        public const int OrderUIElementType = 1;
        public const int OrderVisualName = 2;
        public const int OrderVisualPosition = 3;
        public const int OrderRelativePosition = 4;
        public const int OrderVisualSize = 5;
        public const int OrderVAlignment = 6;
        public const int OrderHAlignment = 7;
        public const int OrderIsVisible = 8;
        public const int OrderCanvasSelectable = 9;
        public const int OrderDepth = 10;

        // 엘리먼트 스테이츠
        public const int StateCount = 4;
        public const int StateNormal = 0;
        public const int StateOver = 1;
        public const int StatePressed = 2;
        public const int StateDisabled = 3;

        public const string PickedKey = nameof(Picked);

        [Category(Constant.ElementCategoryName), DisplayName("타입"), PropertyOrder(OrderUIElementType)]
        [Description("UI 타입을 의미")]
        public abstract SGUIElementType UIElementType { get; }

        [Category(Constant.ElementCategoryName), DisplayName("이름"), PropertyOrder(OrderVisualName)]
        [Description("UI엘리먼트가 트리뷰에서 나타내는 이름입니다.")]
        public string VisualName
        {
            get => visualName_;
            set
            {
                if (visualName_ == value)
                    return;

                visualName_ = value;
                OnPropertyChanged();
            }
        }

        [Browsable(false)]
        public Rect VisualRect => new(VisualPosition, VisualSize);


        [Category(Constant.ElementCategoryName), DisplayName("위치 (절대)"), PropertyOrder(OrderVisualPosition)]
        [Description("UI엘리먼트의 캔버스 좌상단 위치를 의미")]
        public IntPoint VisualPosition
        {
            get => new((int)visualPosition_.X, (int)visualPosition_.Y);
            set
            {
                visualPosition_ = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualRect));
                OnPropertyChanged(nameof(RelativePosition));

                if (IsGroup)
                {
                    SGUIGroup group = Cast<SGUIGroup>();

                    group.OnPropertyChanged(nameof(VisualPositionAnchorRelative));
                    group.OnPropertyChanged(nameof(VisualPositionAnchorAbsolute));
                }
            }
        }

        [Category(Constant.ElementCategoryName), DisplayName("위치 (상대)"), PropertyOrder(OrderRelativePosition)]
        [Description("VAlign, HAlign을 적용시킨 위치이고 이때 좌표계는 특별히 코코스 좌표계를 따른다.")]
        public IntPoint RelativePosition
        {
            get
            {
                if (Parent == null) throw new Exception("루트 그룹은 호출 금지");
                return ConvertVisualPositionToRelativePosition(Parent);
            }
            set
            {
                // 정렬 좌표를 받는다.
                if (Parent == null) throw new Exception("루트 그룹은 호출 금지");
                VisualPosition = ConvertRelativePositionToVisualPosition(Parent, value);
                OnPropertyChanged();
            }
        }


        [Category(Constant.ElementCategoryName), DisplayName("수직 정렬"), PropertyOrder(OrderVAlignment)]
        [Description("UI 그룹내 자식의 세로 정렬 기준입니다.")]
        public VAlignment VerticalAlignment
        {
            get => verticalAlignment_;
            set
            {
                if (verticalAlignment_ == value)
                    return;

                verticalAlignment_ = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(RelativePosition));
                OnPropertyChanged(nameof(VisualPositionAnchorAbsolute));
            }
        }


        [Category(Constant.ElementCategoryName), DisplayName("가로 정렬"), PropertyOrder(OrderHAlignment)]
        [Description("UI 그룹내 자식의 가로 정렬 기준입니다.")]
        public HAlignment HorizontalAlignment
        {
            get => horizontalAlignment_;
            set
            {
                if (horizontalAlignment_ == value)
                    return;

                horizontalAlignment_ = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(RelativePosition));
                OnPropertyChanged(nameof(VisualPositionAnchorAbsolute));
            }
        }


        [Browsable(false)]
        // 앵커의 위치 (절대)
        public IntPoint VisualPositionAnchorAbsolute
        {
            get
            {
                IntPoint absoluteAnchorPosition = VisualPosition;
                IntPoint relativeAnchorPosition = VisualPositionAnchorRelative;

                absoluteAnchorPosition.X += relativeAnchorPosition.X;
                absoluteAnchorPosition.Y += relativeAnchorPosition.Y;
                return absoluteAnchorPosition;
            }
        }


        [Browsable(false)]
        // 엘리먼트 좌상단 위치를 기준으로 앵커의 위치
        public IntPoint VisualPositionAnchorRelative
        {
            get
            {
                IntPoint relativeAnchorRelative = new ();

                switch (HorizontalAlignment)
                {
                    /* case HorizontalAlignment.Left: relativeAnchorRelative.X = 0;  break; */
                    case HAlignment.Center: relativeAnchorRelative.X = VisualSize.Width / 2; break;
                    case HAlignment.Right: relativeAnchorRelative.X = VisualSize.Width; break;
                }

                switch (VerticalAlignment)
                {
                    /* case HorizontalAlignment.Top: relativeAnchorRelative.Y = 0;  break; */
                    case VAlignment.Center: relativeAnchorRelative.Y = VisualSize.Height / 2; break;
                    case VAlignment.Bottom: relativeAnchorRelative.Y = VisualSize.Height; break;
                }

                return relativeAnchorRelative;
            }
        }

        [Browsable(false)]
        [Description("엘리먼트 Rect의 중앙위치")]
        public IntPoint VisualPositionCenter => new(
            visualPosition_.X + VisualSize.Width / 2,
            visualPosition_.Y + VisualSize.Height / 2
        );

        [Browsable(false)]
        [Description("엘리먼트 Rect의 우하단위치")]
        public IntPoint VisualPositionRightBottom => new(
            visualPosition_.X + VisualSize.Width,
            visualPosition_.Y + VisualSize.Height
        );


        [ReadOnly(true)]
        [Category(Constant.ElementCategoryName), DisplayName("크기")/*, PropertyOrder(OrderVisualSize)*/]
        [Description("UI엘리먼트의 크기를 의미")]
        public virtual IntSize VisualSize
        {
            get => Constant.DefaultVisualSize;
            set { }
        }

        /*
         *
         * _visualRect.Size = value;
                OnPropertyChanged();
         *
         */


        [Browsable(false)] public virtual double VisibleOpacity => visible_ ? 1.0 : 0;

        [Category(Constant.ElementCategoryName), DisplayName("보이기"), PropertyOrder(OrderIsVisible)]
        [Description("현재 엘리먼트를 캔버스상에서 표시될지를 결정")]
        public virtual bool IsVisible
        {
            get => visible_;
            set
            {
                if (visible_ == value)
                    return;

                visible_ = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisibleOpacity));
            }
        }

        [Category(Constant.ElementCategoryName), DisplayName("선택 가능"), PropertyOrder(OrderCanvasSelectable)]
        [Description("캔버스상에서 클릭가능하도록 할지")]
        public bool CanvasSelectable
        {
            get => canvasSelectable_;
            set
            {
                if (canvasSelectable_ == value)
                    return;

                canvasSelectable_ = value;

                if (Selected && !canvasSelectable_)
                    Selected = false;

                OnPropertyChanged();
            }
        }


        [Category(Constant.ElementCategoryName), DisplayName("계층적 높이"), PropertyOrder(OrderDepth)]
        [Description("이 엘리먼트의 계층구조상 위치")]
        public virtual int Depth
        {
            get
            {
                if (Parent == null) return -1;
                return Parent.Depth + 1;
            }
        }


        [Browsable(false)]
        [Description("이 엘리먼트가 이미 삭제되었는지 여부")]
        public bool Deleted => deleted_;

        [Browsable(false)]
        public virtual bool IsGroup => false;

        [Browsable(false)]
        public int ZOrder { get; set; }

        [Browsable(false)]
        public abstract bool Manipulatable { get; }


        /*
         * 셀렉트는 픽에 의존성을 가지고 있다.
         * 픽 되어있을 때만 캔버스상에 셀렉션을 표시하도록 하기 위함이다.
         * 픽을 해제할때는 무조건 셀렉트를 먼저 해제 해줘야한다.
         * 그렇지않으면 캔버스에 할당된 셀렉션이 반환되지 않는 문제가 생긴다.
         */

        [Browsable(false)]
        [Category(Constant.ElementCategoryName), DisplayName(nameof(Selected))]
        [Description("엘리먼트가 트리뷰/캔버스 상에서 선택되었는지 ")]
        public bool Selected
        {
            get => selected_;
            set
            {
                if (selected_ == value)
                    return;

                // Debug.WriteLine($"{VisualName} 셀렉 {value}");
                selected_ = value;
                SGUIRootGroup groupMaster = ViewModel.RootGroup;
                ObservableCollection<SGUIElement> selectedElements = groupMaster.SelectedElements;



                if (selected_)
                {
                    selectedElements.Add(this);

                    if (picked_)
                        ViewModel.View.CanvasShapesControl.ArrangeSelection(this);

                    if (selectedElements.Count == 1)
                    {
                        groupMaster.OnPropertyChanged(SGUIRootGroup.HasSelectedElementKey);
                        groupMaster.OnPropertyChanged(SGUIRootGroup.HasPickedSelectedElementKey);
                    }
                    else if (selectedElements.Count == 2)
                        groupMaster.OnPropertyChanged(SGUIRootGroup.IsMultiSelectedKey);


                    // 선택된 원소가 접힌 상태인경우 확장시킬지
                    // 트리뷰에서는 어차피 확장된 상태여야만 선택가능한데
                    // 캔버스상에서 선택할때는 접혀있을 수가 있다.
                    // 그룹마스터는 TreeViewItem자체를 가지고 있지 않기때문에 스킵해야함
                    if (ViewModel.Setting.AutoExpandWhenSelected)
                        ParentTrack.ForEach(parentGroup =>
                        {
                            if (parentGroup == groupMaster)
                                return;

                            if (parentGroup.ItemLoaded)
                                parentGroup.Item.IsExpanded = true;
                        });

                }
                else
                {
                    if (!selectedElements.Remove(this))
                        throw new Exception("선택목록에 엘리먼트가 없습니다.");

                    if (picked_)
                        ViewModel.View.CanvasShapesControl.ReleaseSelection(this);

                    if (selectedElements.Count == 0)
                    {
                        groupMaster.OnPropertyChanged(SGUIRootGroup.HasSelectedElementKey);
                        groupMaster.OnPropertyChanged(SGUIRootGroup.HasPickedSelectedElementKey);
                    }
                    else if (selectedElements.Count == 1)
                        groupMaster.OnPropertyChanged(SGUIRootGroup.IsMultiSelectedKey);
                }


                groupMaster.OnPropertyChanged(SGUIRootGroup.SelectedElementKey);
                OnPropertyChanged();
            }
        }

        [Browsable(false)]
        public bool LastSelected
        {
            get
            {
                var groupMaster = ViewModel.RootGroup;

                if (this == groupMaster)
                    throw new Exception("루트 그룹은 이 함수 호출 금지");

                return groupMaster.SelectedElement == this;
            }
        }


        //////////////////////////////////////////////////////////////////////////////////
        public void SetPick(bool _pick, bool _notify = true)
        {
            picked_ = _pick;
            if (_notify)
                OnPropertyChanged(nameof(Picked));
        }

        // 엘레멘트로 피크 가능, 그룹마스터로도 가능
        [Browsable(false)]
        public bool Picked
        {
            get => picked_;
            set
            {
                if (picked_ == value)
                    return;

                // Debug.WriteLine($"{VisualName} 픽");
                SGUIRootGroup groupMaster = ViewModel.RootGroup;
                picked_ = value;

                if (value == false)
                {
                    if (groupMaster.PickedGroup == this)
                        groupMaster.PickedElements.Clear();
                    else if (!IsGroup)
                        groupMaster.PickedElements.Remove(this);
                    else
                    {
                        groupMaster.PickedElements.Remove(this);
                        Cast<SGUIGroup>().ForEachRecursive(element => groupMaster.PickedElements.Remove(element));
                    }
                }


                OnPropertyChanged();

                if (!picked_)
                    return;

                if (IsGroup)
                {
                    groupMaster.PickedElements.Add(this);
                    SGUIGroup group = Cast<SGUIGroup>();

                    group.ForEachRecursive(element =>
                    {
                        element.SetPick(true); // 트리뷰 아이콘 교체를 위한 노티파이
                        groupMaster.PickedElements.Add(element);
                        element.OnPropertyChanged();
                    });

                    // 그룹을 선택했으면 자기자신의 앵커포인터 반영
                    ViewModel.View.CanvasShapesControl.AdjustAnchor(group);
                }
                else
                {
                    // 그룹이 아니면 부모 그룹도 픽하고 부모그룹에 앵커포인터를 반영한다.
                    groupMaster.PickedElements.Add(Parent);
                    groupMaster.PickedElements.Add(this);

                    Parent.SetPick(true);
                    ViewModel.View.CanvasShapesControl.AdjustAnchor(Parent);
                }

                groupMaster.OnPropertyChanged(SGUIRootGroup.PickedElementKey);
                groupMaster.OnPropertyChanged(SGUIRootGroup.HasPickedElementKey);
                groupMaster.OnPropertyChanged(SGUIRootGroup.HasPickedSelectedElementKey);
            }
        }

        [Browsable(false)]
        public bool FirstPicked => ViewModel.RootGroup.PickedElement == this;


        [Browsable(false)]
        public bool LastPicked
        {
            get
            {
                SGUIRootGroup groupMaster = ViewModel.RootGroup;

                if (groupMaster.PickedElements.Count <= 0)
                    return false;

                return groupMaster.PickedElements[groupMaster.PickedElements.Count - 1] == this;
            }
        }


        [Browsable(false)]
        public bool IsFirst => Index == 0;
        [Browsable(false)]
        public bool IsLast => Index == Parent.Children.Count - 1;

        [Browsable(false)]
        public int Index
        {
            get
            {
                int index = Parent.Children.IndexOf(this);
                if (index == -1) throw new Exception("부모에 포함안된 자식입니다.");
                return index;
            }
        }


        // 1   <--- (Previous.Previous)
        //   2 <--- (Previous)
        //   3 <--- (현재)
        //   4 <--- (Next)
        // 2   <--- (Next.Next)

        [Browsable(false)]
        public SGUIElement Previous
        {
            get
            {
                if (IsFirst)
                    return Parent == ViewModel.RootGroup ? null! : Parent;

                /*
                 * 왜 인덱스 아웃오브레인지 검사를 수행하지 않는가?
                 * 1
                 *   2 <<--- 내가 여기 위치에서 Previous를 수행한 경우 IsFirst조건이 충족되어서 1로 이동한다.
                 *   3       3또는 4인경우에만 이 블록으로 진입하므로 아래 코드가 인덱스 범위를 벗어날일은 절대 발생하지 않는다.
                 *   4
                 */
                SGUIElement prev = Parent.Children[Index - 1];



                /*
                 * 1   <<-- prev
                 *   2
                 *   3
                 *   4
                 *      5
                 *      6 <<-- 이녀석 필요
                 * 2   <<-- 여기서 이전노드로 prev로 이동할 경우 6번 노드로 이동해야한다.
                 *          따라서 prev가 그룹인 경우 그룹내에서 가장 아래에 있는 노드 6번노드를 가져와야한다.
                 */

                if (prev.IsGroup && prev.Item.IsExpanded)
                {
                    SGUIGroup prevGroup = prev.Cast<SGUIGroup>();

                    if (prevGroup.ChildCount > 0)
                        return prevGroup.DeepestLastChild;
                }

                return prev;
            }
        }

        [Browsable(false)]
        public SGUIElement Next
        {
            get
            {
                // 그룹마스터의 부모인경우, 그룹마스터는 부모가 절대없음
                if (Parent == null)
                    return null!;

                int index = Index;

                // 내가 그룹이고 확장된 경우, 그리고 자식이 있는 경우 자식계층으로 진입한다.
                if (IsGroup && Item.IsExpanded)
                {
                    SGUIGroup group = Cast<SGUIGroup>();

                    if (group.ChildCount > 0)
                        return group.Children[0];
                }

                /*
                 * 1
                 *   3
                 *   4
                 *   5  <<-- (IsLast)인경우 부모의 동일 계층구조상 다음원소 즉, 2로 이동해야함.
                 * 2    <<-- 여기
                 */
                return IsLast ? Parent.NextWithChildSkip : Parent.Children[index + 1];
            }
        }

        /*
         * 동등 계층 구조상의 다음 원소로 간다.
         * 즉, 자신이 그룹이고 자식이 있더라도 진입하지 않음
         *
         * 1   <<-- 여기에 위치하더라도 3으로 가지않고 2로간다.
         *   3
         *   4
         *   5  
         * 2   <<-- 여기
         */

        [Browsable(false)]
        public SGUIElement NextWithChildSkip
        {
            get
            {
                if (Parent == null)
                    return null!;

                int index = Index;
                if (Parent.Children.Count > index + 1)
                    return Parent.Children[index + 1];

                /*
                 * 재귀 프로퍼티
                 * 0
                 *   1
                 *     2
                 *     3
                 *     4 <<-- 여기서 수행한 경우 1로 올라간다. 1에서 다시 자식이 없는 경우 0으로 올라감
                 *        5                               
                 *        6
                 * ------------------------------------
                 * 계단 구조라면 결국 Parent == null을 만나게됨
                 *  0
                 *    1
                 *      2
                 */
                return Parent.NextWithChildSkip;
            }
        }


        [Browsable(false)]
        public SGUIGroup Parent { get; set; } = null!;

        [Browsable(false)]
        public SGUIGroup TopParent
        {
            get
            {
                SGUIRootGroup master = ViewModel.RootGroup;

                // 자기 자신이 탑패런트인경우
                if (IsGroup && Parent == master)
                    return null!;

                SGUIGroup parent = Parent;
                int infCheck = 0;

                while (true)
                {
                    if (parent == master)
                        return parent;

                    parent = parent.Parent;
                    infCheck++;

                    if (infCheck >= 10000)
                        throw new Exception("TopParent 함수 실행중 무한루프 발생");
                }
            }
        }

        [Browsable(false)]
        public IEnumerable<SGUIGroup> ParentTrack
        {
            get
            {
                SGUIRootGroup master = ViewModel.RootGroup;
                SGUIGroup parent = Parent;

                // 그룹마스터가 호출하면 이상황이 발생할 수 있다.
                if (parent == null)
                    yield break;

                yield return parent;
                int infCheck = 0;

                while (true)
                {
                    if (parent == master)
                        yield break;


                    parent = parent.Parent;
                    yield return parent;
                    infCheck++;

                    if (infCheck >= 10000)
                        throw new Exception("TopParent 함수 실행중 무한루프 발생");
                }
            }
        }

        [Browsable(false)] public bool ItemLoaded => treeViewItem_ != null;

        [Browsable(false)]
        public TreeViewItem Item
        {
            get
            {
                if (treeViewItem_ == null)
                    throw new Exception($"{VisualName}의 트리뷰아이템이 null입니다.");

                return treeViewItem_;
            }
        }

        [Browsable(false)]
        public virtual int State
        {
            get => state_;
            set => state_ = value;
        }

        // ==============================================================================

        //////////////////////////////////////////////////////////////////////////////////
        public void OnTreeViewItemLoaded(TreeViewItem _item)
        {
            treeViewItem_ = _item;
            OnPropertyChanged(nameof(Item));
        }


        //////////////////////////////////////////////////////////////////////////////////
        public void CopyFrom(SGUIElement _element)
        {
            ViewModel = _element.ViewModel;

            horizontalAlignment_ = _element.horizontalAlignment_;
            verticalAlignment_ = _element.verticalAlignment_;
            canvasSelectable_ = _element.canvasSelectable_;
            visualName_ = _element.visualName_;
            visualPosition_ = _element.visualPosition_;
        }

        public abstract void CreateInit();

        //////////////////////////////////////////////////////////////////////////////////
        public static SGUIElement Create(SGUIElementType _type)
        {
            switch (_type)
            {
                case SGUIElementType.Group: return new SGUIGroup();
                case SGUIElementType.Button: return new SGUIButton();
                case SGUIElementType.Label: return new SGUILabel();
                case SGUIElementType.Sprite: return new SGUISprite();
                case SGUIElementType.EditBox: return new SGUIEditBox();
                case SGUIElementType.CheckBox: return new SGUICheckBox();
                case SGUIElementType.ToggleButton: return new SGUIToggleButton();
                case SGUIElementType.ScrollBar: return new SGUIScrollBar();
                case SGUIElementType.ProgressBar: return new SGUIProgressBar();
                case SGUIElementType.Static: return new SGUIStatic();
                default: throw new Exception($"이런.. {_type} 생성은 아직 구현되지 않았습니다.");
            }
        }


        // DeleteUIElement 설명 참고
        public void DeleteSelf()
        {
            if (deleted_)
                return;

            if (!Parent.Children.Remove(this))
                throw new Exception($"{VisualName}은 {Parent.VisualName}의 자식이 아닙니다.");

            deleted_ = true;

            if (IsGroup)
            {
                Cast<SGUIGroup>().ForEachRecursive(element =>
                {
                    element.deleted_ = true;
                });
            }

            OnPropertyChanged(nameof(Deleted));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static int CompareOrder(SGUIElement _lhsElement, SGUIElement _rhsElement)
        {
            int comp = Comparer<int>.Default.Compare(_lhsElement.Depth, _rhsElement.Depth);
            if (comp == 0)
                return Comparer<int>.Default.Compare(_lhsElement.ZOrder, _rhsElement.ZOrder);
            return comp;
        }

        // 트리뷰 모든 원소부터 위에서부터 한칸씩 계층구조 신경쓰지않고 확인했을 때 누가 위에있고 아래에잇는지 검사
        //////////////////////////////////////////////////////////////////////////////////
        public int CompareHeight(SGUIElement _lhsElement, SGUIElement _rhsElement)
        {
            List<SGUIGroup> lhsTrack = _lhsElement.ParentTrack.Reverse().ToList();
            List<SGUIGroup> rhsTrack = _rhsElement.ParentTrack.Reverse().ToList();

            if (lhsTrack.Count == 0 || rhsTrack.Count == 0)
                throw new Exception("마스터 그룹은 인자로 전달하지 말아주세여 비교대상이 될 수 없습니다.");

            int maxCount = Math.Min(lhsTrack.Count, rhsTrack.Count);
            int comp = 0;
            int depth = 1;
            int lhsCurIndex;
            int rhsCurIndex;

            SGUIGroup parent = ViewModel.RootGroup;

            // 맨위는 무조건 마스터 그룹이기 때문에 1번째 인덱스부터 검사
            for (int i = 1; i < maxCount; ++i)
            {
                SGUIElement lhsCur = lhsTrack[i];
                SGUIElement rhsCur = rhsTrack[i];

                lhsCurIndex = parent.Children.IndexOf(lhsCur);
                rhsCurIndex = parent.Children.IndexOf(rhsCur);

                // 0, 0 -> 0
                // 0, 1 -> -1
                // 1. 0 -> 1
                comp = Comparer<int>.Default.Compare(lhsCurIndex, rhsCurIndex);
                depth++;

                if (comp != 0)
                    return comp;

                // 둘이 인덱스가 같다는 말은 부모가 같다는 뜻이므로 둘중 아무나 캐스팅
                parent = lhsCur.Cast<SGUIGroup>();
            }

            // 우선순위 같은 경우
            // lhs
            // - 1
            //   - 2
            //     - 3
            //       - 4
            //         - (B)
            //       - (A)
            // A와 B모두 1, 2, 3번으로 부모 트랙이 동일할 때
            // 부모수가 더 작은 A를 기준으로 3번 부모에서 몇번째 인덱스에 있는지
            // B의 경우에는 트랙의 depth 인덱스가 3번 부모의 몇번째 인덱스에 있는지 검사하면 된다.

            if (lhsTrack.Count < rhsTrack.Count)    // 위에서 아래로 비교시
            {
                lhsCurIndex = lhsTrack.Last().Children.IndexOf(_lhsElement);
                rhsCurIndex = rhsTrack[depth - 1].Children.IndexOf(rhsTrack[depth]);
            }
            else if (lhsTrack.Count > rhsTrack.Count)   // 아래에서 위로 비교시
            {
                lhsCurIndex = lhsTrack[depth - 1].Children.IndexOf(lhsTrack[depth]);
                rhsCurIndex = rhsTrack.Last().Children.IndexOf(_rhsElement);

                /*
                 * A
                 *   A-1
                 *   A-2
                 *   A-3
                 * 와 같은 트리가 있을때
                 * A-2를 선택후 A를 선택한것과 같이, 밑에서 위로 선택했는데
                 * 부모가 동일한 경우 lhsCurIndex과 rhsCurIndex이 같은 값이 나온다.
                 * 이때는 깊이가 더 깊은 녀석이 우선순위가 더 높아지도록 한다.
                 *
                 * 위 if문 lhsTrack.Count < rhsTrack.Count이 조건에서는 어차피 같더라도 문제가 안됨
                 * A -> A-1 선택시 우선순위가 같지만 찾는걸 A부터 시작하기 때문에 상관이 없다.
                 * 이 조건문에서는 A-1부터 A를 탐색을 시도하기때문에 A부터 찾도록 해주기위해서 이렇게 처리해야함
                 */
                if (lhsCurIndex == rhsCurIndex)
                    return 1;
            }
            else // 동일한 계층에서 선택시
            {
                // 여기 들어온 경우 둘 모두 부모가 같음
                lhsCurIndex = parent.Children.IndexOf(_lhsElement);
                rhsCurIndex = parent.Children.IndexOf(_rhsElement);
            }

            return Comparer<int>.Default.Compare(lhsCurIndex, rhsCurIndex);
        }


        //////////////////////////////////////////////////////////////////////////////////
        public IntPoint ConvertRelativePositionToVisualPosition(SGUIGroup _group, IntPoint _relativePosition)
        {
            IntPoint visualPos = new();
            IntRect groupRect = _group == null
                ? new Rect(0, 0, Constant.ResolutionWidth, Constant.ResolutionHeight)
                : _group.VisualRect;

            switch (horizontalAlignment_)
            {
                case HAlignment.Left:
                    visualPos.X = groupRect.X;
                    break;
                case HAlignment.Center:
                    visualPos.X = groupRect.X +
                                  groupRect.Width / 2 -
                                  VisualSize.Width / 2;
                    break;
                case HAlignment.Right:
                    visualPos.X = groupRect.X +
                                  groupRect.Width -
                                  VisualSize.Width;
                    break;
            }

            switch (verticalAlignment_)
            {
                case VAlignment.Top:
                    visualPos.Y = groupRect.Y;
                    break;
                case VAlignment.Center:
                    visualPos.Y = groupRect.Y +
                                  groupRect.Height / 2 -
                                  VisualSize.Height / 2;
                    break;
                case VAlignment.Bottom:
                    visualPos.Y = groupRect.Y +
                                  groupRect.Height -
                                  VisualSize.Height;
                    break;
            }

            visualPos.X += _relativePosition.X;
            visualPos.Y -= _relativePosition.Y;
            return visualPos;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public IntPoint ConvertVisualPositionToRelativePosition(SGUIGroup _group)
        {
            IntPoint alignedPos = new();

            switch (horizontalAlignment_)
            {
                case HAlignment.Left:
                    alignedPos.X = VisualPosition.X - _group.VisualPosition.X;
                    break;
                case HAlignment.Center:
                    alignedPos.X = VisualPositionCenter.X - _group.VisualPositionCenter.X;
                    break;
                case HAlignment.Right:
                    alignedPos.X = VisualPositionRightBottom.X - _group.VisualPositionRightBottom.X;
                    break;
            }

            switch (verticalAlignment_)
            {
                case VAlignment.Top:
                    alignedPos.Y = _group.VisualPosition.Y - VisualPosition.Y;
                    break;
                case VAlignment.Center:
                    alignedPos.Y = _group.VisualPositionCenter.Y - VisualPositionCenter.Y;
                    break;
                case VAlignment.Bottom:
                    alignedPos.Y = _group.VisualPositionRightBottom.Y - VisualPositionRightBottom.Y;
                    break;
            }

            return alignedPos;
        }


        //////////////////////////////////////////////////////////////////////////////////
        public void SetPosition(VAlignment _vAlign, HAlignment _hAlign, IntPoint _point)
        {
            IntPoint groupPosition = Parent.VisualPosition;
            IntPoint zeroPosition = new();
            switch (_vAlign)
            {
                case VAlignment.Center:
                    zeroPosition.Y = groupPosition.Y + Parent.VisualSize.Height / 2 - VisualSize.Height / 2;
                    break;
                case VAlignment.Bottom:
                    zeroPosition.Y = groupPosition.Y + Parent.VisualSize.Height - VisualSize.Height;
                    break;
            }

            switch (_hAlign)
            {
                case HAlignment.Center:
                    zeroPosition.X = groupPosition.X + Parent.VisualSize.Width / 2 - VisualSize.Width / 2;
                    break;
                case HAlignment.Right:
                    zeroPosition.X = groupPosition.X + Parent.VisualSize.Width - VisualSize.Width;
                    break;
            }

            zeroPosition.X += _point.X;
            zeroPosition.Y += _point.Y;
            VisualPosition = zeroPosition;
        }

        // 9방향위치에 딱 붙여서 배치하는 용도
        //////////////////////////////////////////////////////////////////////////////////
        public void SetPositionZero(VAlignment _vAlign, HAlignment _hAlign)
            => SetPosition(_vAlign, _hAlign, IntPoint.Zero);

        //////////////////////////////////////////////////////////////////////////////////
        public bool ContainPoint(IntPoint _p)
        {
            if (!visible_)
                return false;

            return VisualRect.Contains(_p);
        }

        public abstract object Clone();

        [Browsable(false)]
        public override CanvasElementType CanvasElementType => CanvasElementType.UIElement;

        //////////////////////////////////////////////////////////////////////////////////
        public T Cast<T>() where T : SGUIElement
        {
            T? casted = this as T;

            if (casted == null)
                throw new Exception($"{VisualName}은 {typeof(T).Name}타입이 아닙니다.");

            return casted;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public int Compare(SGUIElement? _lhs, SGUIElement? _rhs)
        {
            return _lhs!.ZOrder.CompareTo(_rhs!.ZOrder);
        }

        public abstract string GetElementTagName();

        //////////////////////////////////////////////////////////////////////////////////
        public virtual XElement ToXElement()
        {
            XElement root = new XElement(GetElementTagName());
            root.SetAttributeValue("name", visualName_);
            root.SetAttributeValue("valign", (int)verticalAlignment_);
            root.SetAttributeValue("halign", (int)horizontalAlignment_);
            if (!visible_)
                root.SetAttributeValue("visible", false);
            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public virtual void ParseXElement(XElement _root)
        {
            visualName_ = (string)_root.Attribute("name")!;
            verticalAlignment_ = (VAlignment)(int)_root.Attribute("valign")!;
            horizontalAlignment_ = (HAlignment)(int)_root.Attribute("halign")!;
            XAttribute visibleAttr = _root.Attribute("visible");
            if (visibleAttr != null)
                visible_ = (bool)visibleAttr;
        }

        [Browsable(false)]
        public object? Tag { get; set; } // 아무런 데이터나 기록할 수 있도록하는 프로퍼티

        // 기본적으로 엘리먼트의 이벤트는 "전파"되도록한다.
        //////////////////////////////////////////////////////////////////////////////////
        public virtual bool OnMouseMove(IntPoint _p)
        {
            if (State == StateDisabled ||
                State == StatePressed)
                return true;

            bool contained = ContainPoint(_p);

            if (!contained)
            {
                State = StateNormal;
                return true;
            }

            State = StateOver;
            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public virtual bool OnMouseDown(IntPoint _p)
        {
            if (State == StateDisabled ||
                State == StatePressed)
                return true;

            bool contained = ContainPoint(_p);
            if (!contained)
                return true;

            State = StatePressed;
            return true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public virtual bool OnMouseUp(IntPoint _p)
        {
            if (State != StatePressed)
                return true;

            bool contained = ContainPoint(_p);
            State = StateNormal;

            if (!contained)
                return true;

            return true;
        }


        //////////////////////////////////////////////////////////////////////////////////
        public override string ToString() => visualName_;


        protected string visualName_ = string.Empty;
        protected IntPoint visualPosition_;
        protected bool selected_ = false;
        protected bool visible_ = true;
        protected bool deleted_ = false;
        protected int state_ = StateNormal;
        protected bool picked_ = false;
        protected bool canvasSelectable_ = true;
        protected HAlignment horizontalAlignment_ = HAlignment.Left;
        protected VAlignment verticalAlignment_ = VAlignment.Bottom;
        protected TreeViewItem? treeViewItem_;



        // =====================================================
        // 프로포티 그리드에서 리플렉션을 사용해서 프로퍼티 처리를 하기때문에
        // 타입 디스크립터로 중간에 가져오는 값을 변경해줘서 소수점 아래자리는 안보이도록 만들어주자.
        // =====================================================


    }

    public interface ISizeRestorable
    {
        void RestoreSize();
    }
}
