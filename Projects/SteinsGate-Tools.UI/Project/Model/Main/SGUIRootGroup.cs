/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 2:38:01 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Linq;
using SGToolsCommon;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Model.Main
{
    public class SGUIRootGroup : SGUIGroup
    {
        public const string HasSelectedElementKey = nameof(HasSelectedElement);
        public const string HasPickedElementKey = nameof(HasPickedElement);
        public const string HasPickedSelectedElementKey = nameof(HasPickedSelectedElement);
        public const string SelectedElementKey = nameof(SelectedElement);
        public const string PickedElementKey = nameof(PickedElement);
        public const string IsMultiSelectedKey = nameof(IsMultiSelected);

        private class ObservableElementsCollection : ObservableCollection<SGUIElement>
        {
            public MainViewModel? ViewModel { get; set; }
        }

        private ObservableCollection<SGUIElement> selectedElements_;
        private ObservableCollection<SGUIElement> pickedElements_;
        private bool pickedElementsDisabled_;

        // ============================================================
        //            프로파티
        // ============================================================

        //////////////////////////////////////////////////////////////////////////////////
        public SGUIRootGroup() : base(0)
        {
            selectedElements_ = new ObservableElementsCollection { ViewModel = null };
            selectedElements_.CollectionChanged += SelectedElementsOnCollectionChanged;
            pickedElements_ = new ObservableElementsCollection { ViewModel = null };
            pickedElements_.CollectionChanged += PickedElementsOnCollectionChanged;
        }

        public SGUIElement PickedElement => pickedElements_.Count > 0 ? pickedElements_[0] : null!;
        public bool HasSelectedElement => selectedElements_.Count > 0;
        public bool HasPickedElement => pickedElements_.Count > 0;
        public bool HasPickedGroup => PickedElement != null && PickedElement.IsGroup;

        public SGUIGroup SelectedGroup
        {
            get
            {
                if (!HasSelectedElement)
                    return this;

                IEnumerable<SGUIElement> selectedGroups = selectedElements_.Where(element => element.IsGroup);

                if (!selectedGroups.Any())
                    return null!;

                return (selectedGroups.OrderBy(element => element.Depth).First() as SGUIGroup)!;
            }
        }

        public SGUIElement SelectedElement
        {
            get
            {
                if (selectedElements_.Count == 0)
                    return null!;

                return selectedElements_[selectedElements_.Count - 1];
            }
        }

        public bool IsMultiSelected => selectedElements_.Count >= 2;

        public ObservableCollection<SGUIElement> SelectedElements => selectedElements_;
        public ObservableCollection<SGUIElement> PickedElements => pickedElements_;

        public SGUIGroup PickedGroup => HasPickedElement ? pickedElements_[0].Cast<SGUIGroup>() : null!;
        public IEnumerable<SGUIElement> PickedSelectedElements => pickedElements_.Where(element => element.Selected);
        public SGUIElement PickedSelectedElement => pickedElements_.LastOrDefault(element => element.Selected)!;
        public bool HasPickedSelectedElement => pickedElements_.FirstOrDefault(element => element.Selected) != null;

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
        public static SGUIRootGroup Create(MainViewModel _viewModel)
        {
            var root = new SGUIRootGroup();
            root.ViewModel = _viewModel;
            ((ObservableElementsCollection)root.selectedElements_).ViewModel = _viewModel;
            ((ObservableElementsCollection)root.pickedElements_).ViewModel = _viewModel;
            root.VisualSize = new IntSize(Constant.ResolutionWidth, Constant.ResolutionHeight);
            root.visualName_ = "RootGroup";
            root.Selected = false;
            root.VerticalAlignment = VAlignment.Top;
            root.HorizontalAlignment = HAlignment.Left;
            root.SetDepth(-1);
            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void DeselectAll()
        {
            SelectedElements.ToList().ForEach(element => element.Selected = false);

            if (SelectedElements.Count > 0)
                throw new Exception("모두 선택해제 했음에도 불구하고 아직 선택목록에서 제거안된 대상이 있습니다.");

            if (Selected)
                throw new Exception("루트 그룹이 셀렉티드 상태입니다.");

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

            Debug.Assert(!Picked, "루트 그룹이 픽드 상태입니다.");
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static void SelectedElementsOnCollectionChanged(object? _sender, NotifyCollectionChangedEventArgs _e)
        {
            ObservableElementsCollection? collection = _sender as ObservableElementsCollection;

            if (collection == null)
                throw new Exception("말도안됩니다.");

            collection.ViewModel.RootGroup.OnPropertyChanged("HasSelectedElement");
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

            if (comp > 0)
                (low, high) = (high, low);

            int state = 0;
            ForEachRecursive(this, ref state);

            void ForEachRecursive(SGUIGroup group, ref int s)
            {
                if (s == 2)
                    return;

                for (int i = 0; i < group.Children.Count; ++i)
                {
                    SGUIElement child = group.Children[i];

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

            ViewModel.View.CanvasShapesControl.AdjustAnchor(null);
            ViewModel.View.UIElementPropertyGrid.SelectedObject = null;
#if DEBUG
            Debug.Assert(!HasSelectedElement);
            Debug.Assert(!HasPickedElement);
            Debug.Assert(!ViewModel.View.CanvasShapesControl.HasSelection);
#endif
        }
    }
}