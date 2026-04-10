/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 2:38:01 PM
 * 이 그룹의 자식은 트리뷰의 아이템소스
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Linq;
using MoreLinq;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Model.Main
{
    public class SGUIGroupMaster : SGUIGroup
    {
        public const string HasSelectedElementKey = nameof(HasSelectedElement);
        public const string HasPickedElementKey = nameof(HasPickedElement);
        public const string HasPickedSelectedElementKey = nameof(HasPickedSelectedElement);
        public const string SelectedElementKey = nameof(SelectedElement);
        public const string PickedElementKey = nameof(PickedElement);
        public const string IsMultiSelectedKey = nameof(IsMultiSelected);

        private class ObservableElementsCollection : ObservableCollection<SGUIElement>
        {
            // 옵저버블컬렉션 생성자보면 크기 안먹음
            //////////////////////////////////////////////////////////////////////////////////
            public ObservableElementsCollection(int _capacity, MainViewModel _viewModel)
                : base(new List<SGUIElement>(_capacity))
            {
                ViewModel = _viewModel;
            }

            public MainViewModel ViewModel { get; }
        }

        // 아래 3가지 항목은 멀티쓰레드 접근 가능함, 따라서 락을 수행토록 한다.
        // 코드 어사이너!
        // 코드 수동할당은 에반거 같아서 자동으로 할당하도록 한다.
        private PriorityQueue<int, int> codeAssigner_;
        private SortedList<int, SGUIGroup> groups_;
        private LinkedList<SGUIElement> elements_;
        private ObservableCollection<SGUIElement> selectedElements_;
        private ObservableCollection<SGUIElement> pickedElements_;
        private bool pickedElementsDisabled_;       // 픽된 엘리먼트 전원 비활성화/활성화 변환용도

        // ============================================================
        //            프로파티
        // ============================================================

        //////////////////////////////////////////////////////////////////////////////////
        public SGUIGroupMaster(MainViewModel _viewModel)
        {
            ViewModel = _viewModel;
            selectedElements_ = new ObservableElementsCollection(120, _viewModel);
            selectedElements_.CollectionChanged += SelectedElementsOnCollectionChanged;
            pickedElements_ = new ObservableElementsCollection(120, _viewModel);
            pickedElements_.CollectionChanged += PickedElementsOnCollectionChanged;

            groups_ = new SortedList<int, SGUIGroup>(120);   // C++의 map같은 녀석임
            elements_ = new LinkedList<SGUIElement>();

            codeAssigner_ = new PriorityQueue<int, int>(Constant.CodeAssignerCapacity);

            // 이정도면 충분하겠지..?
            for (int i = 0; i < Constant.CodeAssignerCapacity; ++i)
                codeAssigner_.Enqueue(Constant.GroupCodeInterval * (i + 1), i);
        }

        public SGUIElement PickedElement => pickedElements_.Count > 0 ? pickedElements_[0] : null;
        public bool HasSelectedElement => selectedElements_.Count > 0;
        public bool HasPickedElement => pickedElements_.Count > 0;
        public bool HasPickedGroup => PickedElement != null && PickedElement.IsGroup;

        // 선택된 엘리먼트가 없을 경우 마스터
        // 선택된 엘리먼트가 그룹일 경우 최상위 그룹
        public SGUIGroup SelectedGroup
        {
            get
            {
                if (!HasSelectedElement)
                    return this;

                IEnumerable<SGUIElement> selectedGroups = selectedElements_.Where(element => element.IsGroup);

                if (!selectedGroups.Any())
                    return null;

                return selectedGroups.OrderBy(element => element.Depth).First() as SGUIGroup;  // 오름차순 정렬 후 처음 그중에서 처음 그룹을 가져옴
            }
        }

        public SGUIElement SelectedElement
        {
            get
            {
                if (selectedElements_.Count == 0)
                    return null;

                return selectedElements_[selectedElements_.Count - 1];
            }
        }

        public bool IsMultiSelected => selectedElements_.Count >= 2;

        public ObservableCollection<SGUIElement> SelectedElements => selectedElements_;
        public ObservableCollection<SGUIElement> PickedElements => pickedElements_;

        public SGUIGroup PickedGroup => HasPickedElement ? pickedElements_[0].Cast<SGUIGroup>() : null;
        public IEnumerable<SGUIElement> PickedSelectedElements => pickedElements_.Where(element => element.Selected);
        public SGUIElement PickedSelectedElement => pickedElements_.LastOrDefault(element => element.Selected);
        public bool HasPickedSelectedElement => pickedElements_.FirstOrDefault(element => element.Selected) != null;
        public int GroupCount { get { lock (groups_) { return groups_.Count; } } }
        public int ElementCount { get { lock (elements_) { return elements_.Count; } } }

        public bool PickedElementsDisabled
        {
            get => pickedElementsDisabled_;
            set
            {
                pickedElementsDisabled_ = value;

                if (value)
                    PickedElements.ForEach(element => element.State = StateDisabled);
                else
                    PickedElements.ForEach(element => element.State = StateNormal);
            }
        }

        // ============================================================
        //            기능
        // ============================================================

        //////////////////////////////////////////////////////////////////////////////////
        public void ForEachGroup(Action<SGUIGroup> _action)
        {
            lock (groups_)
            {
                groups_.Values.ForEach(_action);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void AddGroup(SGUIGroup _group)
        {
            lock (groups_)
            {
                int assignedCode = codeAssigner_.Dequeue();
                _group.SetCode(assignedCode);
                groups_.Add(_group.Code, _group);
                Debug.WriteLine($"할당된 코드 수 {Constant.CodeAssignerCapacity - codeAssigner_.Count}");
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void RemoveGroup(SGUIGroup _group)
        {
            lock (groups_)
            {
                codeAssigner_.Enqueue(_group.Code, _group.Code / Constant.GroupCodeInterval);

                if (!groups_.Remove(_group.Code))
                    throw new Exception("그룹목록에서 삭제하는데 실패했습니다.");

                Debug.WriteLine($"할당된 코드 수 {Constant.CodeAssignerCapacity - codeAssigner_.Count}");
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void ForEachElement(Action<SGUIElement> _action)
        {
            lock (elements_)
            {
                elements_.ForEach(_action);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void AddElement(SGUIElement _element)
        {
            if (_element.IsGroup)
                throw new Exception("그룹도 엘러먼트이긴 하지만 그룹이 아닌 엘리먼트만 보관하도록 하기로 결정했습니다.");

            lock (elements_)
            {
                elements_.AddFirst(_element);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void RemoveElement(SGUIElement _element)
        {
            if (_element.IsGroup)
                throw new Exception("그룹을 엘리먼트 목록에서 삭제할려고 하고 있습니다.");

            lock (elements_)
            {
                if (!elements_.Remove(_element))
                    throw new Exception("엘리먼트목록에서 삭제하는데 실패했습니다.");

                Debug.WriteLine($"할당된 엘리먼트 수 {elements_.Count}");
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void DeselectAll()
        {
            // 새로운 리스트를 만들어서 컨테이너 순회중 Selected를 해제할때 중간에 원소를 삭제해서 잘못된 결과가 나오지 않도록 한다.
            SelectedElements.ToList().ForEach(element => element.Selected = false);

            if (SelectedElements.Count > 0)
                throw new Exception("모두 선택해제 했음에도 불구하고 아직 선택목록에서 제거안된 대상이 있습니다.");

            if (Selected)
                throw new Exception("마스터 그룹이 셀렉티드 상태입니다.");

            int selectedCount = 0;

            ForEachRecursive(element =>
            {
                if (element.Selected)
                    selectedCount++;
            });

            if (selectedCount > 0)
                throw new Exception("자식 요소중에 선택된 원소가 있습니다.");
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void Depick()
        {
            PickedElements.ToList().ForEach(element => element.Picked = false);
            PickedElements.Clear();

#if DEBUG
            int count = 0;
            ForEachRecursive(element =>
            {
                if (element.Picked) count++;
            });

            if (count > 0)
                throw new Exception("모두 선택해제 했음에도 불구하고 아직 선택목록에서 제거안된 대상이 있습니다.");
#endif

            Debug.Assert(!Picked, "마스터 그룹이 픽드 상태입니다.");
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static void SelectedElementsOnCollectionChanged(object? _sender, NotifyCollectionChangedEventArgs _e)
        {
            ObservableElementsCollection collection = _sender as ObservableElementsCollection;

            if (collection == null)
                throw new Exception("말도안됩니다.");

            collection.ViewModel.GroupMaster.OnPropertyChanged("HasSelectedElement");
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void PickedElementsOnCollectionChanged(object? _sender, NotifyCollectionChangedEventArgs _e)
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        public List<SGUIElement> GetElementsBetween(SGUIElement _lhsElement, SGUIElement _rhsElement, bool _expandOnly)
        {
            List<SGUIElement> result = new List<SGUIElement>();

            SGUIElement low = _lhsElement;
            SGUIElement high = _rhsElement;

            int comp = CompareHeight(_lhsElement, _rhsElement);

            // 좌측 엘리먼트가 우선순위가 더 큰경우 스왑
            if (comp > 0)
                (low, high) = (high, low);

            // 0: low를 찾는 상태
            // 1: high를 찾는 상태
            // 2: 둘 모두 찾은 상태 (이후 탐색을 더이상하지 않는다.)
            int state = 0;
            ForEachRecursive(this, ref state);

            void ForEachRecursive(SGUIGroup group, ref int s)
            {
                if (s == 2)
                    return;

                for (int i = 0; i < group.Children.Count; ++i)
                {
                    SGUIElement child = group.Children[i];

                    // 1일때 high 만나기전까지 모든 자식 추가
                    if (child == low && s == 0)
                        s = 1;
                    else if (child == high && s == 1)
                    {
                        s = 2;
                        result.Add(child);
                    }

                    if (s == 1)
                        result.Add(child);

                    if (s == 2)
                        break;

                    if (child.IsGroup && _expandOnly && child.Item.IsLoaded && child.Item.IsExpanded)
                    {
                        ForEachRecursive(child.Cast<SGUIGroup>(), ref s);
                    }
                }
            }

            return result;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void ChangePickedElements(ObservableCollection<SGUIElement> _pickedElements)
        {
            pickedElements_ = _pickedElements;
            OnPropertyChanged(nameof(PickedElements));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public SGUIElement GetElementByCode(int _code)
        {
            int groupCode = _code / Constant.GroupCodeInterval * Constant.GroupCodeInterval;
            int codeIndex = _code % Constant.GroupCodeInterval;

            if (!groups_.ContainsKey(groupCode))
                throw new Exception($"{_code}의 그룹을 찾지 못했습니다.");

            SGUIGroup group = groups_[groupCode];
            if (codeIndex == 0)
                return group;

            if (codeIndex - 1 < group.ChildCount)
                throw new Exception($"{group}에서 {codeIndex - 1}인덱스에 있는 엘리먼트를 찾지못했습니다.");

            return group.Children[codeIndex - 1];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static SGUIGroupMaster Create(MainViewModel _viewModel)
        {
            var master = new SGUIGroupMaster(_viewModel)
            {
                DefineName = "group_master",
                VisualSize = new(Constant.ResolutionWidth, Constant.ResolutionHeight),
                VisualName = "그룹 마스터",
                Selected = false,
                VerticalAlignment = VAlignment.Top,
                HorizontalAlignment = HAlignment.Left,
            };
            master.SetDepth(-1);
            return master;
        }

        // 계층구조를 위에서부터 모두 펼쳐서 일렬로 쭉 나열했을때의 리스트
        //////////////////////////////////////////////////////////////////////////////////
        public List<SGUIElement> ToList()
        {
            List<SGUIElement> list = new();
            ForEachRecursive(element => list.Add(element));
            return list;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void UpdateZOrder()
            => ToList().IndexingForEach((index, element) => element.ZOrder = index);

        //////////////////////////////////////////////////////////////////////////////////
        public void Clear()
        {
            DeselectAll();
            Depick();
            Children.Clear();

            lock (groups_) groups_.Clear();
            lock (elements_) elements_.Clear();

            ViewModel.View.CanvasShapesControl.AdjustAnchor(null);
            ViewModel.View.UIElementPropertyGrid.SelectedObject = null;
#if DEBUG
            Debug.Assert(!HasSelectedElement);
            Debug.Assert(!HasPickedElement);
            Debug.Assert(GroupCount == 0);
            Debug.Assert(ElementCount == 0);
            Debug.Assert(!ViewModel.View.CanvasShapesControl.HasSelection);
#endif
        }
    }
}
