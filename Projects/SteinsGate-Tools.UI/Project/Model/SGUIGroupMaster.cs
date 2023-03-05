/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 2:38:01 PM
 * 이 그룹의 자식은 트리뷰의 아이템소스
 */

using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Linq;
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
using MoreLinq;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Model
{
    public class SGUIGroupMaster : SGUIGroup
    {
        public const string HasSelectedElementKey = nameof(HasSelectedElement);
        public const string HasPickedElementKey = nameof(HasPickedElement);
        public const string SelectedElementKey = nameof(SelectedElement);
        public const string PickedElementKey = nameof(PickedElement);
        public const string IsMultiSelectedKey = nameof(IsMultiSelected);
        


        // ============================================================
        //            프로파티
        // ============================================================


        public SGUIGroupMaster(MainViewModel viewModel) : base(-1)
        {
            ViewModel = viewModel;
            SelectedElements = new ObservableElementsCollection(120, viewModel);
            SelectedElements.CollectionChanged += SelectedElementsOnCollectionChanged;
            PickedElements = new ObservableElementsCollection(120, viewModel);
            PickedElements.CollectionChanged += PickedElementsOnCollectionChanged;
        }


        public SGUIElement PickedElement => PickedElements.Count > 0 ? PickedElements[0] : null;
        public bool HasSelectedElement => SelectedElements.Count > 0;
        public bool HasPickedElement => PickedElements.Count > 0;
        public bool HasPickedGroup => PickedElement != null && PickedElement.IsGroup;

        // 선택된 엘리먼트가 없을 경우 마스터
        // 선택된 엘리먼트가 그룹일 경우 최상위 그룹
        public SGUIGroup SelectedGroup
        {
            get
            {
                if (!HasSelectedElement)
                    return this;

                IEnumerable<SGUIElement> selectedGroups = SelectedElements.Where(element => element.IsGroup);

                if (!selectedGroups.Any())
                    return null;

                return selectedGroups.OrderBy(element => element.Depth).First() as SGUIGroup;  // 오름차순 정렬 후 처음 그중에서 처음 그룹을 가져옴
            }
        }

        public SGUIElement SelectedElement
        {
            get
            {
                if (SelectedElements.Count == 0)
                    return null;

                return SelectedElements[SelectedElements.Count - 1];
            }
        }

        public SGUIElement PrevSelectedElement
        {
            get
            {
                if (SelectedElements.Count <= 1)
                    return null;

                return SelectedElements[SelectedElements.Count - 2];
            }
        }

        public bool IsMultiSelected => SelectedElements.Count >= 2;


        public ObservableCollection<SGUIElement> SelectedElements { get; }
        public ObservableCollection<SGUIElement> PickedElements { get; }

        // ============================================================
        //            기능
        // ============================================================

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

            if (PickedElements.Count > 0)
                throw new Exception("모두 선택해제 했음에도 불구하고 아직 선택목록에서 제거안된 대상이 있습니다.");

            if (Picked)
                throw new Exception("마스터 그룹이 픽드 상태입니다.");

            int pickedCount = 0;

            ForEachRecursive(element =>
            {
                if (element.Selected)
                    pickedCount++;
            });

            if (pickedCount > 0)
                throw new Exception("자식 요소중에 픽된 원소가 있습니다.");
        }


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

        public List<SGUIElement> GetElementsBetween(SGUIElement lhsElement, SGUIElement rhsElement)
        {
            List<SGUIElement> result = new List<SGUIElement>();

            SGUIElement low = lhsElement;
            SGUIElement high = rhsElement;

            
            int comp = Compare(lhsElement, rhsElement);

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

                    if (child.IsGroup)
                    {
                        ForEachRecursive(child.Cast<SGUIGroup>(), ref state);
                    }
                }
            }

            // 밑에껄 선택 후 위에껄 선택한 경우, 반대로 뒤짚어준다.
            if (comp > 0)
                result.Reverse();

            return result;
        }

#if DEBUG
        private static int PickSeq = 0;
        public void PrintPick()
        {
            Debug.WriteLine($"[{PickSeq++}] =======================================");
            PickedElements.ForEach(element =>
            {
                if (!element.Picked)
                    throw new Exception("픽트 않은 대상이 있습니다.");

                Debug.WriteLine($"PICK \t{element.VisualName} 렉트:{element.VisualRect} 깊이:{element.Depth} 부모:{element.Parent.VisualName}");
            });
        }

        private static int SelectSeq = 0;
        public void PrintSelect()
        {
            Debug.WriteLine($"[{SelectSeq++}] =======================================");
            SelectedElements.ForEach(element =>
            {
                if (!element.Selected)
                    throw new Exception("선택되지 않은 대상이 있습니다.");

                Debug.WriteLine($"SEL \t{element.VisualName} 렉트:{element.VisualRect} 깊이:{element.Depth} 부모:{element.Parent.VisualName}");
            });
        }
    }
#endif
}
