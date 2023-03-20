/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 7:54:27 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Net.WebSockets;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Threading.Tasks.Dataflow;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Xml.Linq;
using MoreLinq;
using Newtonsoft.Json.Linq;
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
        public const string DefineNameKey = nameof(DefineName);
        public const string VisualSizeKey = nameof(VisualSize);

        public const int OrderUIElementType = 1;
        public const int OrderCodeString = 2;
        public const int OrderVisualName = 3;
        public const int OrderDefineName = 4;
        public const int OrderVisualPosition = 5;
        public const int OrderRelativePosition = 6;    // 엘리먼트들
        public const int OrderVisualSize = 6;
        public const int OrderVAlignment = 7;
        public const int OrderHAlignment = 8;
        public const int OrderIsVisible = 9;
        public const int OrderCanvasSelectable = 10;
        public const int OrderDepth = 11;

        // 엘리먼트 스테이츠
        public const int StateCount = 4;
        public const int StateNormal = 0;
        public const int StateOver = 1;
        public const int StatePressed = 2;
        public const int StateDisabled = 3;

        // 엘리먼트
        public const string JsonCodeKey = "code";
        public const string JsonElementTypeKey = "type";
        public const string JsonVisualNameKey = "name";
        public const string JsonDefineNameKey = "dname";
        public const string JsonVisualSizeKey = "size";
        public const string JsonVAlignKey = "valign";
        public const string JsonHAlignKey = "halign";
        public const string JsonLinearDodgeKey = "linear_dodge";

        // 그룹
        public const string JsonChildrenKey = "children";

        // 버튼
        public const string JsonSgaKey = "sga";
        public const string JsonImgKey = "img";
        public const string JsonSpriteKey = "sprite";

        // 스프라이트
        // 라벨
        public const string JsonFontKey = "font";
        public const string JsonFontSizeKey = "font_size";
        public const string JsonFontColorKey = "font_color";
        public const string JsonTextKey = "text";
        public const string JsonTextWrapKey = "text_wrap";
        public const string JsonTextVAlignKey = "text_valign";
        public const string JsonTextHAlignKey = "text_halign";

        // 에딧 박스
        public const string JsonPlaceholderTextKey = "p_text";
        public const string JsonPlaceholderFontColorKey = "p_font_color";
        public const string JsonPlaceholderFontSizeKey = "p_font_size";
        public const string JsonMaxLengthKey = "max_length";
        public const string JsonInputModeKey = "input_mode";

        // 체크 박스
        public const string JsonBackgroundSga = "bg_sga";
        public const string JsonBackgroundImg = "bg_img";
        public const string JsonCrossSga = "cross_sga";
        public const string JsonCrossImg = "cross_img";

        // 토글 버튼
        public const string JsonToggleSpriteKey = "sprite2";

        // 프로그래스바
        public const string JsonDirectionKey = "direction";

        // 스크롤바
        public const string JsonTrackSizeKey = "track_size";






        public const string PickedKey = nameof(Picked);

        [Category(Constant.ElementCategoryName), DisplayName("타입"), PropertyOrder(OrderUIElementType)]
        [Description("UI 타입을 의미")]
        public abstract SGUIElementType UIElementType { get; }

        [Category(Constant.ElementCategoryName), DisplayName("코드"), PropertyOrder(OrderCodeString)]
        public string CodeString => Code.ToString("#,##0");

        [Category(Constant.ElementCategoryName), DisplayName("이름"), PropertyOrder(OrderVisualName)]
        [Description("UI엘리먼트가 트리뷰에서 나타내는 이름입니다.")]
        public string VisualName
        {
            get => _visualName;
            set
            {
                if (_visualName == value)
                    return;

                _visualName = value;
                OnPropertyChanged();
            }
        }

        [Category(Constant.ElementCategoryName), DisplayName("디파인 명"), PropertyOrder(OrderDefineName)]
        [Description("게임에서 실제로 사용될 디파인 이름")]
        public string DefineName
        {
            get => _defineName;
            set
            {
                _defineName = value;
                OnPropertyChanged();
            }
        }

        [Browsable(false)]
        public Rect VisualRect => new(VisualPosition, VisualSize);


        [Category(Constant.ElementCategoryName), DisplayName("위치 (절대)"), PropertyOrder(OrderVisualPosition)]
        [Description("UI엘리먼트의 캔버스 좌상단 위치를 의미")]
        public IntPoint VisualPosition
        {
            get => new((int)_visualPosition.X, (int)_visualPosition.Y);
            set
            {
                _visualPosition = value;
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
                if (Parent == null) throw new Exception("마스터 그룹은 호출 금지");
                return ConvertVisualPositionToRelativePosition(Parent);
            }
            set
            {
                // 정렬 좌표를 받는다.
                if (Parent == null) throw new Exception("마스터 그룹은 호출 금지");
                VisualPosition = ConvertRelativePositionToVisualPosition(Parent, value);
                OnPropertyChanged();
            }
        }


        [Category(Constant.ElementCategoryName), DisplayName("수직 정렬"), PropertyOrder(OrderVAlignment)]
        [Description("UI 그룹내 자식의 세로 정렬 기준입니다.")]
        public VAlignment VerticalAlignment
        {
            get => _verticalAlignment;
            set
            {
                if (_verticalAlignment == value)
                    return;

                _verticalAlignment = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(RelativePosition));
                OnPropertyChanged(nameof(VisualPositionAnchorAbsolute));
            }
        }


        [Category(Constant.ElementCategoryName), DisplayName("가로 정렬"), PropertyOrder(OrderHAlignment)]
        [Description("UI 그룹내 자식의 가로 정렬 기준입니다.")]
        public HAlignment HorizontalAlignment
        {
            get => _horizontalAlignment;
            set
            {
                if (_horizontalAlignment == value)
                    return;

                _horizontalAlignment = value;
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
            _visualPosition.X + VisualSize.Width / 2,
            _visualPosition.Y + VisualSize.Height / 2
        );

        [Browsable(false)]
        [Description("엘리먼트 Rect의 우하단위치")]
        public IntPoint VisualPositionRightBottom => new(
            _visualPosition.X + VisualSize.Width,
            _visualPosition.Y + VisualSize.Height
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


        [Browsable(false)] public virtual double VisibleOpacity => _visible ? 1.0 : 0;

        [Category(Constant.ElementCategoryName), DisplayName("보이기"), PropertyOrder(OrderIsVisible)]
        [Description("현재 엘리먼트를 캔버스상에서 표시될지를 결정")]
        public virtual bool IsVisible
        {
            get => _visible;
            set
            {
                if (_visible == value)
                    return;

                _visible = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisibleOpacity));
            }
        }

        [Category(Constant.ElementCategoryName), DisplayName("선택 가능"), PropertyOrder(OrderCanvasSelectable)]
        [Description("캔버스상에서 클릭가능하도록 할지")]
        public bool CanvasSelectable
        {
            get => _canvasSelectable;
            set
            {
                if (_canvasSelectable == value)
                    return;

                _canvasSelectable = value;

                if (Selected && !_canvasSelectable)
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
        public bool Deleted => _deleted;

        [Browsable(false)]
        public virtual bool IsGroup => false;

        [Browsable(false)]
        public virtual int Code => Parent == null ? -1 : Parent.Code + Index + 1;

        [Browsable(false)] 
        public int ZOrder { get; set; }


        [Browsable(false)]
        public int GroupCode => Code / Constant.GroupCodeInterval * Constant.GroupCodeInterval;

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
            get => _selected;
            set
            {
                if (_selected == value)
                    return;

                // Debug.WriteLine($"{VisualName} 셀렉 {value}");
                _selected = value;
                SGUIGroupMaster groupMaster = ViewModel.GroupMaster;
                ObservableCollection<SGUIElement> selectedElements = groupMaster.SelectedElements;



                if (_selected)
                {
                    selectedElements.Add(this);

                    if (_picked)
                        ViewModel.View.CanvasShapesControl.ArrangeSelection(this);

                    if (selectedElements.Count == 1)
                    {
                        groupMaster.OnPropertyChanged(SGUIGroupMaster.HasSelectedElementKey);
                        groupMaster.OnPropertyChanged(SGUIGroupMaster.HasPickedSelectedElementKey);
                    }
                    else if (selectedElements.Count == 2)
                        groupMaster.OnPropertyChanged(SGUIGroupMaster.IsMultiSelectedKey);


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

                    if (_picked)
                        ViewModel.View.CanvasShapesControl.ReleaseSelection(this);

                    if (selectedElements.Count == 0)
                    {
                        groupMaster.OnPropertyChanged(SGUIGroupMaster.HasSelectedElementKey);
                        groupMaster.OnPropertyChanged(SGUIGroupMaster.HasPickedSelectedElementKey);
                    }
                    else if (selectedElements.Count == 1)
                        groupMaster.OnPropertyChanged(SGUIGroupMaster.IsMultiSelectedKey);
                }


                groupMaster.OnPropertyChanged(SGUIGroupMaster.SelectedElementKey);
                OnPropertyChanged();
            }
        }

        [Browsable(false)]
        public bool LastSelected
        {
            get
            {
                var groupMaster = ViewModel.GroupMaster;

                if (this == groupMaster)
                    throw new Exception("그룹 마스터는 이 함수 호출 금지");

                return groupMaster.SelectedElement == this;
            }
        }


        public void SetPick(bool pick, bool notify = true)
        {
            _picked = pick;
            if (notify)
                OnPropertyChanged(nameof(Picked));
        }

        // 엘레멘트로 피크 가능, 그룹마스터로도 가능
        [Browsable(false)]
        public bool Picked
        {
            get => _picked;
            set
            {
                if (_picked == value)
                    return;

                // Debug.WriteLine($"{VisualName} 픽");
                SGUIGroupMaster groupMaster = ViewModel.GroupMaster;
                _picked = value;

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

                if (!_picked)
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

                groupMaster.OnPropertyChanged(SGUIGroupMaster.PickedElementKey);
                groupMaster.OnPropertyChanged(SGUIGroupMaster.HasPickedElementKey);
                groupMaster.OnPropertyChanged(SGUIGroupMaster.HasPickedSelectedElementKey);
            }
        }

        [Browsable(false)]
        public bool FirstPicked => ViewModel.GroupMaster.PickedElement == this;


        [Browsable(false)]
        public bool LastPicked
        {
            get
            {
                SGUIGroupMaster groupMaster = ViewModel.GroupMaster;

                if (groupMaster.PickedElements.Count <= 0)
                    return false;

                return groupMaster.PickedElements[groupMaster.PickedElements.Count - 1] == this;
            }
        }


        [Browsable(false)]
        public bool IsMaster => this == ViewModel.GroupMaster;
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
                    return Parent == ViewModel.GroupMaster ? null : Parent;

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
                    return null;

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
                    return null;

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
        public SGUIGroup Parent { get; set; }

        [Browsable(false)]
        public SGUIGroup TopParent
        {
            get
            {
                SGUIGroupMaster master = ViewModel.GroupMaster;

                // 자기 자신이 탑패런트인경우
                if (IsGroup && Parent == master)
                    return null;

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
                SGUIGroupMaster master = ViewModel.GroupMaster;
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

        [Browsable(false)] public bool ItemLoaded => _treeViewItem != null;

        [Browsable(false)]
        public TreeViewItem Item
        {
            get
            {
                if (_treeViewItem == null)
                    throw new Exception($"{VisualName}의 트리뷰아이템이 null입니다.");

                return _treeViewItem;
            }
        }

        [Browsable(false)]
        public virtual int State
        {
            get => _state;
            set => _state = value;
        }

        // ==============================================================================

        public void OnTreeViewItemLoaded(TreeViewItem item)
        {
            _treeViewItem = item;
            OnPropertyChanged(nameof(Item));
        }


        public void CopyFrom(SGUIElement element)
        {
            ViewModel = element.ViewModel;

            _canvasSelectable = element._canvasSelectable;
            _visualName = element._visualName;
            _visualPosition = element._visualPosition;
            _defineName = element._defineName;
        }

        public abstract void CreateInit();

        public static SGUIElement Create(SGUIElementType type)
        {
            switch (type)
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
                default: throw new Exception($"이런.. {type} 생성은 아직 구현되지 않았습니다.");
            }
        }


        // DeleteUIElement 설명 참고
        public void DeleteSelf()
        {
            if (_deleted)
                return;

            if (!Parent.Children.Remove(this))
                throw new Exception($"{VisualName}은 {Parent.VisualName}의 자식이 아닙니다.");

            _deleted = true;
            SGUIGroupMaster groupMaster = ViewModel.GroupMaster;

            if (IsGroup)
            {
                var delGroup = Cast<SGUIGroup>();
                groupMaster.RemoveGroup(delGroup);
                delGroup.ForEachRecursive(element =>
                {
                    if (element.IsGroup)
                        groupMaster.RemoveGroup(element.Cast<SGUIGroup>());
                    else
                        groupMaster.RemoveElement(element);

                    element._deleted = true;
                });
            }
            else
            {
                groupMaster.RemoveElement(this);
            }

            OnPropertyChanged(nameof(Deleted));
        }

        public static int CompareOrder(SGUIElement lhsElement, SGUIElement rhsElement)
        {
            int comp = Comparer<int>.Default.Compare(lhsElement.Depth, rhsElement.Depth);
            if (comp == 0)
                return Comparer<int>.Default.Compare(lhsElement.Code, rhsElement.Code);
            return comp;
        }

        // 트리뷰 모든 원소부터 위에서부터 한칸씩 계층구조 신경쓰지않고 확인했을 때 누가 위에있고 아래에잇는지 검사
        public int CompareHeight(SGUIElement lhsElement, SGUIElement rhsElement)
        {
            List<SGUIGroup> lhsTrack = lhsElement.ParentTrack.Reverse().ToList();
            List<SGUIGroup> rhsTrack = rhsElement.ParentTrack.Reverse().ToList();

            if (lhsTrack.Count == 0 || rhsTrack.Count == 0)
                throw new Exception("마스터 그룹은 인자로 전달하지 말아주세여 비교대상이 될 수 없습니다.");

            int maxCount = Math.Min(lhsTrack.Count, rhsTrack.Count);
            int comp = 0;
            int depth = 1;
            int lhsCurIndex;
            int rhsCurIndex;

            SGUIGroup parent = ViewModel.GroupMaster;

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
                lhsCurIndex = lhsTrack.Last().Children.IndexOf(lhsElement);
                rhsCurIndex = rhsTrack[depth - 1].Children.IndexOf(rhsTrack[depth]);
            }
            else if (lhsTrack.Count > rhsTrack.Count)   // 아래에서 위로 비교시
            {
                lhsCurIndex = lhsTrack[depth - 1].Children.IndexOf(lhsTrack[depth]);
                rhsCurIndex = rhsTrack.Last().Children.IndexOf(rhsElement);

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
                lhsCurIndex = parent.Children.IndexOf(lhsElement);
                rhsCurIndex = parent.Children.IndexOf(rhsElement);
            }

            return Comparer<int>.Default.Compare(lhsCurIndex, rhsCurIndex);
        }


        public IntPoint ConvertRelativePositionToVisualPosition(SGUIGroup group, IntPoint relativePosition)
        {
            IntPoint visualPos = new ();
            IntRect groupRect = group == null
                ? new Rect(0, 0, Constant.ResolutionWidth, Constant.ResolutionHeight)
                : group.VisualRect;


            switch (_horizontalAlignment)
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

            switch (_verticalAlignment)
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

            visualPos.X += relativePosition.X;
            visualPos.Y -= relativePosition.Y;
            return visualPos;
        }

        public IntPoint ConvertVisualPositionToRelativePosition(SGUIGroup group)
        {
            IntPoint alignedPos = new();


            switch (_horizontalAlignment)
            {
                case HAlignment.Left:
                    alignedPos.X = VisualPosition.X - group.VisualPosition.X;
                    break;
                case HAlignment.Center:
                    alignedPos.X = VisualPositionCenter.X - group.VisualPositionCenter.X;
                    break;
                case HAlignment.Right:
                    alignedPos.X = VisualPositionRightBottom.X - group.VisualPositionRightBottom.X;
                    break;
            }

            switch (_verticalAlignment)
            {
                case VAlignment.Top:
                    alignedPos.Y = group.VisualPosition.Y - VisualPosition.Y;
                    break;
                case VAlignment.Center:
                    alignedPos.Y = group.VisualPositionCenter.Y - VisualPositionCenter.Y;
                    break;
                case VAlignment.Bottom:
                    alignedPos.Y = group.VisualPositionRightBottom.Y - VisualPositionRightBottom.Y;
                    break;
            }

            return alignedPos;
        }


        public void SetPosition(VAlignment vAlign, HAlignment hAlign, IntPoint IntPoint)
        {
            IntPoint groupPosition = Parent.VisualPosition;
            IntPoint zeroPosition = new ();
            switch (vAlign)
            {
                case VAlignment.Center:
                    zeroPosition.Y = groupPosition.Y + Parent.VisualSize.Height / 2 - VisualSize.Height / 2;
                    break;
                case VAlignment.Bottom:
                    zeroPosition.Y = groupPosition.Y + Parent.VisualSize.Height - VisualSize.Height;
                    break;
            }

            switch (hAlign)
            {
                case HAlignment.Center:
                    zeroPosition.X = groupPosition.X + Parent.VisualSize.Width / 2 - VisualSize.Width / 2;
                    break;
                case HAlignment.Right:
                    zeroPosition.X = groupPosition.X + Parent.VisualSize.Width - VisualSize.Width;
                    break;
            }

            zeroPosition.X += IntPoint.X;
            zeroPosition.Y += IntPoint.Y;
            VisualPosition = zeroPosition;
        }


        // 9방향위치에 딱 붙여서 배치하는 용도
        public void SetPositionZero(VAlignment vAlign, HAlignment hAlign)
            => SetPosition(vAlign, hAlign, IntPoint.Zero);

        public bool ContainPoint(IntPoint p)
        {
            if (!_visible)
                return false;

            return VisualRect.Contains(p);
        }

        public abstract object Clone();

        [Browsable(false)]
        public override CanvasElementType CanvasElementType => CanvasElementType.UIElement;

        public T Cast<T>() where T : SGUIElement
        {
            T casted = this as T;

            if (casted == null)
                throw new Exception($"{VisualName}은 {typeof(T).Name}타입이 아닙니다.");

            return casted;
        }

        public int Compare(SGUIElement lhs, SGUIElement rhs)
        {
            return lhs.Code.CompareTo(rhs.Code);
        }


        public virtual JObject ToJObject()
        {
            JObject root = new JObject();
            root[JsonCodeKey] = Code;
            root[JsonElementTypeKey] = (int)UIElementType;
            root[JsonDefineNameKey] = _defineName;

            // 비주얼 이름은 게임데이터에서 필요없음
            root[JsonVisualNameKey] = _visualName;

            // 필요없는 자식도 있음
            root[JsonVAlignKey] = (int)_verticalAlignment;
            root[JsonHAlignKey] = (int)_horizontalAlignment;
            return root;
        }

        public virtual void ParseJObject(JObject root)
        {
            // 코드필요없음
            // 엘리먼트타입 고정이므로 필요없음
            _visualName = (string)root[JsonVisualNameKey];
            _defineName = (string)root[JsonDefineNameKey];
            _verticalAlignment = (VAlignment)(int)root[JsonVAlignKey];
            _horizontalAlignment = (HAlignment)(int)root[JsonHAlignKey];
        }

        [Browsable(false)]
        public object Tag { get; set; } // 아무런 데이터나 기록할 수 있도록하는 프로퍼티

        // 기본적으로 엘리먼트의 이벤트는 "전파"되도록한다.
        public virtual bool OnMouseMove(IntPoint p)
        {
            if (State == StateDisabled ||
                State == StatePressed)
                return true;

            bool contained = ContainPoint(p);

            if (!contained)
            {
                State = StateNormal;
                return true;
            }

            State = StateOver;
            return true;
        }

        public virtual bool OnMouseDown(IntPoint p)
        {
            if (State == StateDisabled ||
                State == StatePressed)
                return true;

            bool contained = ContainPoint(p);
            if (!contained)
                return true;

            State = StatePressed;
            return true;
        }

        public virtual bool OnMouseUp(IntPoint p)
        {
            if (State != StatePressed)
                return true;

            bool contained = ContainPoint(p);
            State = StateNormal;

            if (!contained)
                return true;

            return true;
        }


        public override string ToString() => _visualName;


        protected string _visualName = string.Empty;
        protected IntPoint _visualPosition;
        protected bool _selected = false;
        protected bool _visible = true;
        protected bool _deleted = false;
        protected int _state = StateNormal;
        protected bool _picked = false;
        protected bool _canvasSelectable = true;
        protected HAlignment _horizontalAlignment = HAlignment.Left;
        protected VAlignment _verticalAlignment = VAlignment.Bottom;
        protected TreeViewItem _treeViewItem;
        protected string _defineName = string.Empty;



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
