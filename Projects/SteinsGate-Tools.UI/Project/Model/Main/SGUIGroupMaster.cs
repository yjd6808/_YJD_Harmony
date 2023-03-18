/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 2:38:01 PM
 * 이 그룹의 자식은 트리뷰의 아이템소스
 *
 */

using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
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
            public ObservableElementsCollection(int capacity, MainViewModel viewModel)
                : base(new List<SGUIElement>(capacity))
            {
                ViewModel = viewModel;
            }

            public MainViewModel ViewModel { get; }
        }


        // ============================================================
        //            프로파티
        // ============================================================


        public SGUIGroupMaster(MainViewModel viewModel)
        {
            ViewModel = viewModel;
            _selectedElements = new ObservableElementsCollection(120, viewModel);
            _selectedElements.CollectionChanged += SelectedElementsOnCollectionChanged;
            _pickedElements = new ObservableElementsCollection(120, viewModel);
            _pickedElements.CollectionChanged += PickedElementsOnCollectionChanged;

            _groups = new SortedList<int, SGUIGroup>(120);   // C++의 map같은 녀석임
            _elements = new LinkedList<SGUIElement>();


            _codeAssigner = new PriorityQueue<int, int>(Constant.CodeAssignerCapacity);

            // 이정도면 충분하겠지..?
            for (int i = 0; i < Constant.CodeAssignerCapacity; ++i)
                _codeAssigner.Enqueue(Constant.GroupCodeInterval * (i + 1), i);
        }


        public SGUIElement PickedElement => _pickedElements.Count > 0 ? _pickedElements[0] : null;
        public bool HasSelectedElement => _selectedElements.Count > 0;
        public bool HasPickedElement => _pickedElements.Count > 0;
        public bool HasPickedGroup => PickedElement != null && PickedElement.IsGroup;

        // 선택된 엘리먼트가 없을 경우 마스터
        // 선택된 엘리먼트가 그룹일 경우 최상위 그룹
        public SGUIGroup SelectedGroup
        {
            get
            {
                if (!HasSelectedElement)
                    return this;

                IEnumerable<SGUIElement> selectedGroups = _selectedElements.Where(element => element.IsGroup);

                if (!selectedGroups.Any())
                    return null;

                return selectedGroups.OrderBy(element => element.Depth).First() as SGUIGroup;  // 오름차순 정렬 후 처음 그중에서 처음 그룹을 가져옴
            }
        }

        public SGUIElement SelectedElement
        {
            get
            {
                if (_selectedElements.Count == 0)
                    return null;

                return _selectedElements[_selectedElements.Count - 1];
            }
        }

        public bool IsMultiSelected => _selectedElements.Count >= 2;


        public ObservableCollection<SGUIElement> SelectedElements => _selectedElements;
        public ObservableCollection<SGUIElement> PickedElements => _pickedElements;

        public SGUIGroup PickedGroup => HasPickedElement ? _pickedElements[0].Cast<SGUIGroup>() : null;
        public IEnumerable<SGUIElement> PickedSelectedElements => _pickedElements.Where(element => element.Selected);
        public SGUIElement PickedSelectedElement => _pickedElements.LastOrDefault(element => element.Selected);
        public bool HasPickedSelectedElement => _pickedElements.FirstOrDefault(element => element.Selected) != null;
        public int GroupCount { get { lock (_groups) { return _groups.Count; } } }
        public int ElementCount { get { lock (_elements) { return _elements.Count; } } }

        public bool PickedElementsDisabled
        {
            get => _pickedElementsDisabled;
            set
            {
                _pickedElementsDisabled = value;

                if (value)
                    PickedElements.ForEach(element => element.State = StateDisabled);
                else
                    PickedElements.ForEach(element => element.State = StateNormal);
            }
        }




        // ============================================================
        //            기능
        // ============================================================

        public void ForEachGroup(Action<SGUIGroup> action)
        {
            lock (_groups)
            {
                _groups.Values.ForEach(action);
            }
        }

        public void AddGroup(SGUIGroup group)
        {
            lock (_groups)
            {
                int assignedCode = _codeAssigner.Dequeue();
                group.SetCode(assignedCode);
                _groups.Add(group.Code, group);
                Debug.WriteLine($"할당된 코드 수 {Constant.CodeAssignerCapacity - _codeAssigner.Count}");
            }
        }

        public void RemoveGroup(SGUIGroup group)
        {
            lock (_groups)
            {
                _codeAssigner.Enqueue(group.Code, group.Code / Constant.GroupCodeInterval);

                if (!_groups.Remove(group.Code))
                    throw new Exception("그룹목록에서 삭제하는데 실패했습니다.");

                Debug.WriteLine($"할당된 코드 수 {Constant.CodeAssignerCapacity - _codeAssigner.Count}");
            }
        }

        public void ForEachElement(Action<SGUIElement> action)
        {
            lock (_elements)
            {
                _elements.ForEach(action);
            }
        }

        public void AddElement(SGUIElement element)
        {
            if (element.IsGroup)
                throw new Exception("그룹도 엘러먼트이긴 하지만 그룹이 아닌 엘리먼트만 보관하도록 하기로 결정했습니다.");

            lock (_elements)
            {
                _elements.AddFirst(element);
            }
        }

        public void RemoveElement(SGUIElement element)
        {
            if (element.IsGroup)
                throw new Exception("그룹을 엘리먼트 목록에서 삭제할려고 하고 있습니다.");

            lock (_elements)
            {
                if (!_elements.Remove(element))
                    throw new Exception("엘리먼트목록에서 삭제하는데 실패했습니다.");

                Debug.WriteLine($"할당된 엘리먼트 수 {_elements.Count}");
            }

        }

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
            int selectedCount = 0;

            ForEachRecursive(element =>
            {
                if (element.Selected)
                    selectedCount++;
            });

            if (selectedCount > 0)
                throw new Exception("자식 요소중에 선택된 원소가 있습니다.");
        }



        private static void SelectedElementsOnCollectionChanged(object? sender, NotifyCollectionChangedEventArgs e)
        {
            ObservableElementsCollection collection = sender as ObservableElementsCollection;

            if (collection == null)
                throw new Exception("말도안됩니다.");

            collection.ViewModel.GroupMaster.OnPropertyChanged("HasSelectedElement");
        }

        private void PickedElementsOnCollectionChanged(object? sender, NotifyCollectionChangedEventArgs e)
        {
        }

        public List<SGUIElement> GetElementsBetween(SGUIElement lhsElement, SGUIElement rhsElement, bool expandOnly)
        {
            List<SGUIElement> result = new List<SGUIElement>();

            SGUIElement low = lhsElement;
            SGUIElement high = rhsElement;


            int comp = CompareHeight(lhsElement, rhsElement);

            // 좌측 엘리먼트가 우선순위가 더 큰경우 스왑
            if (comp > 0)
                (low, high) = (high, low); // ㄷㄷ 이런게 되네

            // 0: low를 찾는 상태
            // 1: high를 찾는 상태
            // 2: 둘 모두 찾은 상태 (이후 탐색을 더이상하지 않는다.)
            int state = 0;
            ForEachRecursive(this, ref state);

            void ForEachRecursive(SGUIGroup group, ref int state)
            {
                if (state == 2)
                    return;

                for (int i = 0; i < group.Children.Count; ++i)
                {
                    SGUIElement child = group.Children[i];

                    // 1일때 high 만나기전까지 모든 자식 추가
                    if (child == low && state == 0)
                        state = 1;
                    else if (child == high && state == 1)
                    {
                        state = 2;
                        result.Add(child);
                    }

                    if (state == 1)
                        result.Add(child);

                    if (state == 2)
                        break;

                    if (child.IsGroup && expandOnly && child.Item.IsLoaded && child.Item.IsExpanded)
                    {
                        ForEachRecursive(child.Cast<SGUIGroup>(), ref state);
                    }
                }
            }

            // 밑에껄 선택 후 위에껄 선택한 경우(마지막 선택한게 이전 선택한 것보다 위에있다면), 반대로 뒤짚어준다.
            if (comp > 0)
                result.Reverse();

            return result;
        }

        public void ChangePickedElements(ObservableCollection<SGUIElement> pickedElements)
        {
            _pickedElements = pickedElements;
            OnPropertyChanged(nameof(PickedElements));
        }

        public SGUIElement GetElementByCode(int code)
        {
            int groupCode = code / Constant.GroupCodeInterval * Constant.GroupCodeInterval;
            int codeIndex = code % Constant.GroupCodeInterval;

            if (!_groups.ContainsKey(groupCode))
                throw new Exception($"{code}의 그룹을 찾지 못했습니다.");

            SGUIGroup group = _groups[groupCode];
            if (codeIndex == 0)
                return group;

            if (codeIndex - 1 < group.ChildCount)
                throw new Exception($"{group}에서 {codeIndex - 1}인덱스에 있는 엘리먼트를 찾지못했습니다.");

            return group.Children[codeIndex - 1];
        }


        public static SGUIGroupMaster Create(MainViewModel viewModel)
        {
            var master = new SGUIGroupMaster(viewModel)
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

        public void Clear()
        {
            DeselectAll();
            Depick();
            Children.Clear();

            lock (_groups) _groups.Clear();
            lock (_elements) _elements.Clear();

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

        // 아래 3가지 항목은 멀티쓰레드 접근 가능함, 따라서 락을 수행토록 한다.
        // 코드 어사이너!
        // 코드 수동할당은 에반거 같아서 자동으로 할당하도록 한다.
        private PriorityQueue<int, int> _codeAssigner;
        private SortedList<int, SGUIGroup> _groups;
        private LinkedList<SGUIElement> _elements;
        private ObservableCollection<SGUIElement> _selectedElements;
        private ObservableCollection<SGUIElement> _pickedElements;
        private bool _pickedElementsDisabled;       // 픽된 엘리먼트 전원 비활성화/활성화 변환용도
    }
}
