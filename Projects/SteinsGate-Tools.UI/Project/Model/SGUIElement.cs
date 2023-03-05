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
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
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
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model
{
    public abstract class SGUIElement : CanvasElement, ICloneable
    {
        public const string PickedKey = nameof(Picked);
        // =========================================================================
        //                         엘리먼트 관련 정보
        // =========================================================================

        [Browsable(false)] public virtual bool IsGroup => false;

        [Category("Element")]
        [DisplayName("ElementType")]
        [Description("UI 타입을 의미")]
        public abstract SGUIElementType UIElementType { get; }

        

        [Category("Element")]
        [DisplayName(nameof(Name))]
        [Description("게임에서 사용할 이름 혹은 변수명의 축약형태")]
        public string Name
        {
            get => _name;
            set
            {
                _name = value;
                OnPropertyChanged();
            }
        }

        [Browsable(true)]
        [Category("Element")]
        [DisplayName(nameof(DefineName))]
        [Description("게임에서 실제로 사용할 변수/Define명")]
        public string DefineName
        {
            get
            {
                StringBuilder sb = new StringBuilder(60);


                return sb.ToString();
            }
        }

        [Category("Element")]
        [DisplayName(nameof(Code))]
        [Description("엘리먼트 고유 코드를 의미")]
        public int Code
        {
            get => _code;
            set
            {
                _code = value;
                OnPropertyChanged();
            }
        }

        [Browsable(false)]
        [Category("Element")]
        [DisplayName(nameof(Selected))]
        [Description("엘리먼트가 트리뷰/캔버스 상에서 선택되었는지 ")]
        public bool Selected
        {
            get => _selected;
            set
            {
                if (_selected == value)
                    return;

                _selected = value;
                SGUIGroupMaster groupMaster = ViewModel.GroupMaster;
                ObservableCollection<SGUIElement> selectedElements = groupMaster.SelectedElements;


                if (_selected)
                {
                    selectedElements.Add(this);

                    if (_picked)
                        ViewModel.View.CanvasShapesControl.ArrangeSelection(this);

                    if (selectedElements.Count == 1)
                        groupMaster.OnPropertyChanged(SGUIGroupMaster.HasSelectedElementKey);
                    else if (selectedElements.Count == 2)
                        groupMaster.OnPropertyChanged(SGUIGroupMaster.IsMultiSelectedKey);

                }
                else
                {
                    if (!selectedElements.Remove(this))
                        throw new Exception("선택목록에 엘리먼트가 없습니다.");

                    if (_picked)
                        ViewModel.View.CanvasShapesControl.ReleaseSelection(this);

                    if (selectedElements.Count == 0)
                        groupMaster.OnPropertyChanged(SGUIGroupMaster.HasSelectedElementKey);
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

        [Browsable(false)]
        public bool PrevSelected
        {
            get
            {
                var groupMaster = ViewModel.GroupMaster;

                if (this == groupMaster)
                    throw new Exception("그룹 마스터는 이 함수 호출 금지");


                return groupMaster.PrevSelectedElement == this;
            }
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

                SGUIGroupMaster groupMaster = ViewModel.GroupMaster;

                _picked = value;

                if (_picked)
                {
                    groupMaster.DeselectAll();
                    groupMaster.PickedElements.ForEach(element => element.Picked = false);
                    groupMaster.PickedElements.Clear();
                    groupMaster.PickedElements.Add(this);

                    if (IsGroup)
                    {
                        Cast<SGUIGroup>().ForEachRecursive(element =>
                        {
                            element._picked = true;
                            groupMaster.PickedElements.Add(element);
                            element.OnPropertyChanged();    // 트리뷰 아이콘 교체를 위한 노티파이
                        });
                    }
                    
                    groupMaster.OnPropertyChanged(SGUIGroupMaster.PickedElementKey);
                    groupMaster.OnPropertyChanged(SGUIGroupMaster.HasPickedElementKey);
                }

                OnPropertyChanged();
            }
        }

        [Browsable(false)] public bool FirstPicked => ViewModel.GroupMaster.SelectedElement == this;

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

        // =========================================================================
        //                         비주얼 관련 정보
        // =========================================================================
        [Browsable(false)]
        [Category("Visual")]
        [DisplayName(nameof(VisualRect))]
        [Description("UI엘리먼트의 캔버스 좌상단 위치와 크기를 의미")]
        public Rect VisualRect
        {
            get => _visualRect;
            set
            {
                _visualRect = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualSize));
                OnPropertyChanged(nameof(VisualPosition));
            } 
        }

        [Category("Visual")]
        [DisplayName(nameof(VisualPosition))]
        [Description("UI엘리먼트의 캔버스 좌상단 위치를 의미")]
        public Point VisualPosition
        {
            get => _visualRect.Location;
            set
            {
                _visualRect.Location = value;
                Debug.WriteLine(value);
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualRect));
            }
        }

        [Category("Visual")]
        [DisplayName(nameof(VisualSize))]
        [Description("UI엘리먼트의 크기를 의미")]
        public Size VisualSize
        {
            get => _visualRect.Size;
            set
            {
                _visualRect.Size = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualRect));
            }
        }

        [Category("Visual")]
        [DisplayName(nameof(VisualName))]
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

        [Category("Visual")]
        [DisplayName(nameof(IsVisible))]
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
            }
        }

        [Category("Visual")]
        [DisplayName(nameof(Depth))]
        [Description("이 엘리먼트의 계층구조상 위치")]
        public virtual int Depth => Parent.Depth + 1;




        [Category("Visual")]
        [DisplayName(nameof(Deleted))]
        [Description("이 엘리먼트가 이미 삭제되었는지 여부")]
        public bool Deleted => _deleted;


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


        protected string _visualName = string.Empty;
        protected Rect _visualRect = new (0, 0, 50, 50);
        protected bool _selected = false;
        protected bool _visible = true;
        protected bool _deleted = false;
        protected bool _picked = false;

        protected string _name;
        protected int _code;


        public void CopyFrom(SGUIElement element)
        {
            ViewModel = element.ViewModel;

            _visualName = element._visualName;
            _visualRect = element._visualRect;
            _visible = element._visible;

            // 셀렉션 여부는 복사안함.

            _name = element._name;
            _code = element._code;
        }

        public abstract void CreateInit();

        public static SGUIElement Create(SGUIElementType type, SGUIGroup parent)
        {
            SGUIElement element = null;

            switch (type)
            {
                case SGUIElementType.Button: 
                    element = new SGUIButton();
                    break;
                case SGUIElementType.Group: 
                    element = new SGUIGroup(parent.Depth + 1);
                    break;
            }

            if (element == null)
                throw new Exception("");
            
            element.CreateInit();
            element.Parent = parent;
            return element;
        }

        public void DeleteSelf()
        {
            if (_deleted)
                return;

            if (!Parent.Children.Remove(this))
                throw new Exception($"{VisualName}은 {Parent.VisualName}의 자식이 아닙니다.");

            _deleted = true;

            if (IsGroup)
                Cast<SGUIGroup>().ForEachRecursive(element => element._deleted = true);

            OnPropertyChanged(nameof(Deleted));
        }

        // 트리뷰 모든 원소부터 위에서부터 한칸씩 계층구조 신경쓰지않고 확인했을 때 누가 위에있고 아래에잇는지 검사
        public int Compare(SGUIElement lhsElement, SGUIElement rhsElement)
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
    }
}
