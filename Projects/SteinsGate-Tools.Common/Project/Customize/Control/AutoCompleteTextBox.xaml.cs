// jdyun 24/10/04(금)

using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Customize.Control
{
    public class AutoCompletionCandidateItem : Bindable
    {
        private string text_;
        public string Text
        {
            get => text_;
            set
            {
                if (text_ != value)
                {
                    text_ = value;
                    OnPropertyChanged();
                }
            }
        }

        public AutoCompleteTextBox Parent { get; }
        public int Index { get; }
        public bool IsSelected => Parent.HighlightedIndex == Index;

        public AutoCompletionCandidateItem(AutoCompleteTextBox _parent, string _text, int _index)
        {
            Parent = _parent;
            text_ = _text;
            Index = _index;
        }

        public void RefreshSelection()
        {
            OnPropertyChanged(nameof(IsSelected));
        }
    }

    public partial class AutoCompleteTextBox : UserControl
    {
        public enum CandidateItemNavigation
        {
            Next,
            Previous,
            First,
        }

        public AutoCompleteTextBox()
        {
            InitializeComponent();
        }

        private AutoCompletionCandidateItem? prevItem_ = null;
        private AutoCompletionCandidateItem? currItem_ = null;

        public static readonly DependencyProperty HighlightedIndexProperty =
            DependencyProperty.Register(nameof(HighlightedIndex), typeof(int), typeof(AutoCompleteTextBox),
                new PropertyMetadata(-1));
        public int HighlightedIndex
        {
            get => (int)GetValue(HighlightedIndexProperty);
            set => SetValue(HighlightedIndexProperty, value);
        }

        public static readonly DependencyProperty CandidatesProperty =
            DependencyProperty.Register(nameof(Candidates), typeof(List<string>), typeof(AutoCompleteTextBox),
                new PropertyMetadata(new List<string>(), OnCandidatesChanged));
        public List<string> Candidates
        {
            get => (List<string>)GetValue(CandidatesProperty);
            set => SetValue(CandidatesProperty, value);
        }

        private static readonly DependencyProperty FilteredCandidatesProperty =
            DependencyProperty.Register(nameof(FilteredCandidates), typeof(List<AutoCompletionCandidateItem>), typeof(AutoCompleteTextBox),
                new PropertyMetadata(new List<AutoCompletionCandidateItem>()));
        private List<AutoCompletionCandidateItem> FilteredCandidates
        {
            get => (List<AutoCompletionCandidateItem>)GetValue(FilteredCandidatesProperty);
            set => SetValue(FilteredCandidatesProperty, value);
        }
        public static readonly DependencyProperty TextProperty =
            DependencyProperty.Register(nameof(Text), typeof(string), typeof(AutoCompleteTextBox),
                new PropertyMetadata(string.Empty, OnTextChanged));

        public string Text
        {
            get => (string)GetValue(TextProperty);
            set => SetValue(TextProperty, value);
        }

        public static readonly DependencyProperty PlaceholderProperty =
            DependencyProperty.Register(nameof(Placeholder), typeof(string), typeof(AutoCompleteTextBox), new PropertyMetadata(string.Empty));

        public string Placeholder
        {
            get { return (string)GetValue(PlaceholderProperty); }
            set { SetValue(PlaceholderProperty, value); }
        }

        public static readonly DependencyProperty IsEmptyProperty =
            DependencyProperty.Register(nameof(IsEmpty), typeof(bool), typeof(AutoCompleteTextBox), new PropertyMetadata(true));

        public bool IsEmpty
        {
            get { return (bool)GetValue(IsEmptyProperty); }
            private set { SetValue(IsEmptyProperty, value); }
        }

        // RoutedEvent 정의
        public static readonly RoutedEvent TextChangedEvent = EventManager.RegisterRoutedEvent(
            nameof(TextChanged), RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(AutoCompleteTextBox));

        // TextChanged 이벤트를 위한 CLR 이벤트 래퍼
        public event RoutedEventHandler TextChanged
        {
            add { AddHandler(TextChangedEvent, value); }
            remove { RemoveHandler(TextChangedEvent, value); }
        }

        private static void OnTextChanged(DependencyObject _d, DependencyPropertyChangedEventArgs _e)
        {
            var control = (AutoCompleteTextBox)_d;

            if (control.IsFocused)
            {
                control.FilterCandidates(true);
            }

            control.IsEmpty = control.Text.Length == 0;
            control.RaiseEvent(new RoutedEventArgs(TextChangedEvent));
        }

        private static void OnCandidatesChanged(DependencyObject _d, DependencyPropertyChangedEventArgs _e)
        {
            var control = (AutoCompleteTextBox)_d;
            control.FilterCandidates(false);
        }

        // 후보 필터링 메서드
        private void FilterCandidates(bool _dropDown)
        {
            HighlightedIndex = -1;
            prevItem_ = null;
            currItem_ = null;

            // TODO: jdyun 병목구간
            // 엄청단순하게 구현한거라 성능이 많이 안좋을 것 같다.
            // 프로퍼티 바꾸면서 콤보박스아이템을 새로 다 만들려고 시도할 것 같은데..
            // 성능 좋게 만들려면 시간 좀 투자해야해서 일단 이렇게 넘어감.
            if (string.IsNullOrEmpty(Text))
            {
                FilteredCandidates = Candidates.Select((_x, _i)  => new AutoCompletionCandidateItem(this, _x, _i)).ToList();
            }
            else
            {
                // Text로 시작하는 원소들을 앞에 정렬시키고 나머진 뒤에 사전편찬 순으로 정렬함
                var list = Candidates
                    .Where(_s => _s.Contains(Text))
                    .OrderBy(_s => _s.StartsWith(Text) ? 0 : 1)
                    .ThenBy(_s => _s) 
                    .ToList();
                FilteredCandidates = list.Select((_x, _i) => new AutoCompletionCandidateItem(this, _x, _i)).ToList();
                if (_dropDown)
                {
                    AutoCompleteComboBox.IsDropDownOpen = list.Count > 0;
                }
            }

            Navigate(CandidateItemNavigation.First);
        }

        private void AutoCompleteTextBox_OnPreviewKeyDown(object _sender, KeyEventArgs _e)
        {
            // 콤보박스로 자체 키 이벤트 처리를 overriding 하기 위함.
            // 아랫 방향키 누르면 DropDown 보이면서 아이템 자동 선택됨 이것도 막음
            bool isDropDownOpen = AutoCompleteComboBox.IsDropDownOpen;
            if (isDropDownOpen)
            {
                if (_e.Key == Key.Down)
                {
                    Navigate(CandidateItemNavigation.Next);
                    _e.Handled = true;
                }
                else if (_e.Key == Key.Up)
                {
                    Navigate(CandidateItemNavigation.Previous);
                    _e.Handled = true;
                }
                else if (_e.Key == Key.Enter)
                {
                    SelectCurrentItem();
                    _e.Handled = true;
                }
            }
            else if (_e.Key == Key.Down) // 드랍다운 안열려있을 때 방향아랫키 누르면 자동으로 아이템선택하는 거 막음
            {
                _e.Handled = true;
            }
        }

        public void Navigate(CandidateItemNavigation _navigation)
        {
            if (!FilteredCandidates.Any())
                return;

            int prevHighlightedIndex = HighlightedIndex;
            int currHighlightedIndex = -1;

            switch (_navigation)
            {
            case CandidateItemNavigation.Next:
                currHighlightedIndex = (prevHighlightedIndex + 1) % FilteredCandidates.Count;
                break;
            case CandidateItemNavigation.Previous:
                currHighlightedIndex = (prevHighlightedIndex - 1 + FilteredCandidates.Count) % FilteredCandidates.Count;
                break;
            case CandidateItemNavigation.First:
                currHighlightedIndex = 0;
                break;
            }

            HighlightedIndex = currHighlightedIndex;
            currItem_ = FilteredCandidates[HighlightedIndex];
            prevItem_?.RefreshSelection();
            currItem_.RefreshSelection();
            prevItem_ = currItem_;

            ScrollViewer? sv = AutoCompleteComboBox.GetDropDownScrollViewer();
            if (sv != null)
            {
                // sv.ViewportHeight는 스크롤뷰에 보이는 아이템 갯수
                // sv.VerticalOffset는 스크롤뷰의 맨위 아이템의 인덱스값

                int maxIndex = AutoCompleteComboBox.Items.Count - 1;
                int startIndex = (int)sv.VerticalOffset;
                int endIndex = startIndex + (int)sv.ViewportHeight - 1;

                // 스크롤이 바닥까지 내려옴
                if (endIndex > maxIndex)
                {
                    endIndex = maxIndex;
                }

                switch (_navigation)
                {
                case CandidateItemNavigation.Next:
                    if (currHighlightedIndex > endIndex)
                    {
                        double viewportIndex = startIndex + 1;
                        sv.ScrollToVerticalOffset(viewportIndex);
                    }
                    else if (currHighlightedIndex < startIndex)
                    {
                        sv.ScrollToVerticalOffset(0);
                    }
                    break;
                case CandidateItemNavigation.Previous:
                    if (currHighlightedIndex < startIndex)
                    {
                        double viewportIndex = startIndex - 1;
                        sv.ScrollToVerticalOffset(viewportIndex);
                    }
                    else if (currHighlightedIndex > endIndex)
                    {
                        sv.ScrollToBottom();
                    }
                    break;
                case CandidateItemNavigation.First:
                    sv.ScrollToVerticalOffset(0);
                    break;
                }
            }

            // 드랍다운 펼쳐질때 찰나에 텍스트박스 셀렉트 되던데.. 왜 그런지 몰겠다.
            // 추정하기로 드랍다운될 때 맨 처음 아이템 select 되는 이벤트 발생하면서 그런거 같은데..
            // 일단 강제로 select 해제 처리함
            TextBox? tb = AutoCompleteComboBox.GetEditableTextBox();
            if (tb != null)
            {
                tb.Select(tb.Text.Length, 0);
            }
        }

        public void SelectCurrentItem()
        {
            if (currItem_ == null)
                return;

            Text = currItem_.Text;
            AutoCompleteComboBox.IsDropDownOpen = false;

            TextBox? tb = AutoCompleteComboBox.GetEditableTextBox();
            if (tb == null)
                return;
            tb.CaretIndex = int.MaxValue;
        }

        private void AutoCompleteComboBox_OnPreviewMouseDown(object _sender, System.Windows.Input.MouseButtonEventArgs _e)
        {
            // 콤보박스 하단 Border를 클릭하면 DropDown박스가 열리는데 이걸 막기 위함
            _e.Handled = true;
        }
    }
}
