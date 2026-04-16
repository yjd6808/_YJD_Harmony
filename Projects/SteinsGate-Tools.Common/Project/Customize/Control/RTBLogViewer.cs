/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 12:34:42 PM
 * created by AI.
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Customize.Control
{
    public class RTBLogViewer : RichTextBox
    {
        private readonly List<RTBLogItem> items_ = new();
        private int capacity_ = 1000;
        private ScrollViewer? scrollViewer_;

        //////////////////////////////////////////////////////////////////////////////////
        // XAML: <ctrl:RTBLogViewer.LogContextMenu> 또는 코드: rtbLog_.LogContextMenu = ... 로 설정
        public static readonly DependencyProperty LogContextMenuProperty =
            DependencyProperty.Register(
                nameof(LogContextMenu),
                typeof(ContextMenu),
                typeof(RTBLogViewer),
                new PropertyMetadata(null));

        public ContextMenu? LogContextMenu
        {
            get => (ContextMenu?)GetValue(LogContextMenuProperty);
            set => SetValue(LogContextMenuProperty, value);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // XAML: <ctrl:RTBLogViewer.TextOption> 또는 코드: rtbLog_.TextOption = ... 로 설정
        // WriteLine / Writer().Text() 호출 시 옵션이 없으면 이 값을 기본으로 적용
        public static readonly DependencyProperty TextOptionProperty =
            DependencyProperty.Register(
                nameof(TextOption),
                typeof(RTBTextOption),
                typeof(RTBLogViewer),
                new PropertyMetadata(null));

        public RTBTextOption TextOption
        {
            get => (RTBTextOption?)GetValue(TextOptionProperty) ?? DefaultTextOption;
            set => SetValue(TextOptionProperty, value);
        }

        private static readonly RTBTextOption DefaultTextOption = new RTBTextOption
        {
            Foreground = System.Windows.Media.Brushes.Black,
        };

        //////////////////////////////////////////////////////////////////////////////////
        public RTBLogViewer()
        {
            IsUndoEnabled        = false;
            IsReadOnly           = false; // InlineUIContainer 마우스 이벤트를 위해 false 설정
            AcceptsReturn        = false;
            AcceptsTab           = false;
            Document.PagePadding = new Thickness(0);

            Loaded += OnLoaded;
            PreviewKeyDown += OnPreviewKeyDown;
            PreviewMouseRightButtonUp += OnPreviewMouseRightButtonUp;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public int GetCount() => items_.Count;

        //////////////////////////////////////////////////////////////////////////////////
        public void SetCapacity(int _capacity) => capacity_ = _capacity;

        //////////////////////////////////////////////////////////////////////////////////
        // Builder를 통한 변환 (한 줄에 여러 항목 추가)
        public RTBLogWriter Writer() => new RTBLogWriter(this);

        //////////////////////////////////////////////////////////////////////////////////
        // 단순 텍스트 한 줄 추가 (편의 메서드)
        // _option이 null이면 TextOption(DependencyProperty)을 기본으로 사용
        public void WriteLine(string _text, RTBTextOption? _option = null)
        {
            RTBTextOption option = TextOption.Clone();
            option.MergeFrom(_option);
            RTBLogItem item = new RTBLogItem(
                new List<RTBLogItemElement> { new RTBLogItemElement(_text, option) },
                0
            );
            AddItem(item);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void RemoveAt(int _index)
        {
            if (_index < 0 || _index >= items_.Count)
                return;

            RemoveItemAt(_index);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void RemoveAll(Func<RTBLogItem, bool> _predicate)
        {
            List<RTBLogItem> targets = items_.Where(_predicate).ToList();
            foreach (RTBLogItem item in targets)
            {
                Document.Blocks.Remove(item.Paragraph);
                items_.Remove(item);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void Clear()
        {
            Document.Blocks.Clear();
            items_.Clear();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public new void ScrollToEnd()
        {
            if (scrollViewer_ != null)
                scrollViewer_.ScrollToBottom();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void ScrollToBegin()
        {
            if (scrollViewer_ != null)
                scrollViewer_.ScrollToTop();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void ScrollTo(RTBLogItem? _item)
        {
            if (scrollViewer_ == null || _item == null)
                return;

            Rect rect = _item.Paragraph.ContentStart.GetCharacterRect(LogicalDirection.Forward);
            scrollViewer_.ScrollToVerticalOffset(scrollViewer_.VerticalOffset + rect.Top);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void ScrollAt(int _index)
        {
            if (_index < 0 || _index >= items_.Count)
                return;

            ScrollTo(items_[_index]);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 특정 항목의 시작 위치로 캐럿을 이동
        public void CursorTo(RTBLogItem? _item)
        {
            if (_item == null)
                return;

            TextPointer pointer = _item.Paragraph.ContentStart;
            CaretPosition = pointer;
            Focus();
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 문서 시작(첫 번째 항목)으로 캐럿을 이동
        public void CursorToHome()
        {
            CaretPosition = Document.ContentStart;
            Focus();
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 문서 끝(마지막 항목)으로 캐럿을 이동
        public void CursorToEnd()
        {
            CaretPosition = Document.ContentEnd;
            Focus();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public bool IsExist(Func<RTBLogItem, bool> _predicate) => items_.Any(_predicate);

        //////////////////////////////////////////////////////////////////////////////////
        public RTBLogItem? Find(Func<RTBLogItem, bool> _predicate) => items_.FirstOrDefault(_predicate);

        //////////////////////////////////////////////////////////////////////////////////
        public List<RTBLogItem> FindAll(Func<RTBLogItem, bool> _predicate) => items_.Where(_predicate).ToList();

        //////////////////////////////////////////////////////////////////////////////////
        // 조건에 맞는 첫 번째 항목의 텍스트 범위를 선택 상태로 설정
        public void Select(Func<RTBLogItem, bool> _predicate)
        {
            RTBLogItem? item = items_.FirstOrDefault(_predicate);
            if (item != null)
                Select(item);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void Select(RTBLogItem? _item)
        {
            if (_item == null)
                return;

            TextPointer start = _item.Paragraph.ContentStart;
            TextPointer end   = _item.Paragraph.ContentEnd;
            Selection.Select(start, end);
            Focus();
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 전체 항목 선택
        public new void SelectAll()
        {
            Selection.Select(Document.ContentStart, Document.ContentEnd);
            Focus();
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 선택된 범위의 텍스트를 클립보드에 복사 (InlineUIContainer 포함)
        public void CopySelection()
        {
            string text = CollectSelectionText();
            if (!string.IsNullOrEmpty(text))
                Clipboard.SetText(text);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 전체 로그를 클립보드에 복사
        public void CopyAll() => Clipboard.SetText(GetPlainText());

        //////////////////////////////////////////////////////////////////////////////////
        // 전체 로그 PlainText 반환
        public string GetPlainText()
        {
            StringBuilder sb = new StringBuilder();
            foreach (RTBLogItem item in items_)
                sb.AppendLine(item.PlainText);
            return sb.ToString();
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 특정 항목 PlainText 반환
        public string GetPlainText(RTBLogItem? _item) => _item?.PlainText ?? string.Empty;

        //////////////////////////////////////////////////////////////////////////////////
        // 조건에 맞는 첫 번째 항목의 인덱스 반환 (없으면 -1)
        public int FindIndex(Func<RTBLogItem, bool> _predicate) => items_.FindIndex(i => _predicate(i));

        //////////////////////////////////////////////////////////////////////////////////
        // RTBLogBuilder에서 호출 (internal 접근자도 좋으나 외부에서도 사용하므로 public)
        public void AddItem(RTBLogItem _item)
        {
            while (items_.Count >= capacity_)
                RemoveItemAt(0);

            items_.Add(_item);
            Document.Blocks.Add(_item.Paragraph);

            ScrollToEnd();
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected virtual void OnLoaded(object _sender, RoutedEventArgs _e)
        {
            scrollViewer_ = this.GetScrollViewerCommon();
            OverrideCopyCommand();
        }

        //////////////////////////////////////////////////////////////////////////////////
        // RichTextBox가 PreviewMouseLeftButtonDown을 consume하기 때문에
        // IsLink=true(InlineUIContainer/TextBlock) 또는 IsLink=false+Click(Run hit-test) 모두
        // Paragraph.MouseLeftButtonDown까지 이벤트가 전달되지 않는 문제를 해결.
        // Preview 단계에서 직접 클릭 대상을 찾아 Click()을 호출한다.
        protected override void OnPreviewMouseLeftButtonDown(MouseButtonEventArgs _e)
        {
            // 클릭 위치에서 TextBlock(IsLink=true) 또는 Run hit-test(IsLink=false+Click) 탐색
            if (TryHandleClickAt(_e.GetPosition(this)))
            {
                _e.Handled = true;
                return;
            }

            base.OnPreviewMouseLeftButtonDown(_e);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 마우스 이동 시 클릭 가능한 요소 위에서 Hand 커서로 변경
        // RichTextBox가 커서를 덮어쓰므로 Preview 단계에서 직접 설정한다.
        protected override void OnPreviewMouseMove(MouseEventArgs _e)
        {
            Cursor = HasClickableElementAt(_e.GetPosition(this)) ? Cursors.Hand : Cursors.IBeam;
            base.OnPreviewMouseMove(_e);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 기본 RichTextBox ContextMenu 표시 억제
        // LogContextMenu가 설정된 경우에만 해당 메뉴 표시
        protected virtual void OnPreviewMouseRightButtonUp(object _sender, MouseButtonEventArgs _e)
        {
            _e.Handled = true;
            if (LogContextMenu != null)
            {
                LogContextMenu.PlacementTarget = this;
                LogContextMenu.IsOpen = true;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        // Ctrl+C / Ctrl+A 이외의 키 입력 차단 (텍스트 편집 방지)
        protected virtual void OnPreviewKeyDown(object _sender, KeyEventArgs _e)
        {
            bool isCtrl = (Keyboard.Modifiers & ModifierKeys.Control) != 0;

            if (isCtrl)
            {
                switch (_e.Key)
                {
                    case Key.C:
                    case Key.A:
                        return; // 허용
                }
            }

            _e.Handled = true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // InlineUIContainer 포함 텍스트를 WPF 기본 복사에서 처리되지 않는 문제 해결
        // 직접 선택된 Inline들을 순회하여 Run + InlineUIContainer 텍스트를 수동 구성
        private void OverrideCopyCommand()
        {
            CommandBindings.Add(new CommandBinding(
                ApplicationCommands.Copy,
                (_, _e) =>
                {
                    string text = CollectSelectionText();
                    if (!string.IsNullOrEmpty(text))
                        Clipboard.SetText(text);
                    _e.Handled = true;
                },
                (_, _e) =>
                {
                    _e.CanExecute = !Selection.IsEmpty;
                    _e.Handled    = true;
                }
            ));
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 선택 범위의 텍스트를 수집
        // Run 은 Text 범위 직접 읽기
        // InlineUIContainer 는 범위 선택이 불가하므로 TextBlock.Text 전체 사용
        private string CollectSelectionText()
        {
            TextPointer start = Selection.Start;
            TextPointer end   = Selection.End;

            if (start == null || end == null || start.CompareTo(end) >= 0)
                return string.Empty;

            StringBuilder sb = new StringBuilder();

            // 선택 범위와 겹치는 Paragraph들을 순회
            foreach (Block block in Document.Blocks)
            {
                if (block is not Paragraph paragraph)
                    continue;

                // Paragraph의 범위가 선택 범위와 겹치는지 확인
                if (paragraph.ContentEnd.CompareTo(start) < 0)
                    continue;

                if (paragraph.ContentStart.CompareTo(end) > 0)
                    break;

                bool firstParagraph = sb.Length > 0;
                if (firstParagraph)
                    sb.AppendLine();

                foreach (Inline inline in paragraph.Inlines)
                {
                    TextPointer inlineStart = inline.ContentStart;
                    TextPointer inlineEnd   = inline.ContentEnd;

                    // Inline의 범위가 선택 범위와 겹치는지 확인
                    if (inlineEnd.CompareTo(start) < 0)
                        continue;

                    if (inlineStart.CompareTo(end) > 0)
                        break;

                    if (inline is Run run)
                    {
                        // 선택 범위와 교차하는 부분만 읽기
                        TextPointer runStart = inlineStart.CompareTo(start) < 0 ? start : inlineStart;
                        TextPointer runEnd   = inlineEnd.CompareTo(end)   > 0 ? end   : inlineEnd;
                        TextRange range = new TextRange(runStart, runEnd);
                        sb.Append(range.Text);
                    }
                    else if (inline is InlineUIContainer container && container.Child is TextBlock tb)
                    {
                        // InlineUIContainer는 범위 측정이 불가하므로 전체 텍스트 사용
                        sb.Append(tb.Text);
                    }
                }
            }

            return sb.ToString();
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 클릭 위치에서 Click 콜백이 있는 요소를 찾아 호출한다.
        // IsLink=true  : VisualTree에서 TextBlock을 찾아 option_.Click() 호출
        // IsLink=false : RTBLogItem의 Run hit-test를 통해 Click() 호출
        private bool TryHandleClickAt(Point _pos)
        {
            // VisualTree hit-test: IsLink=true(InlineUIContainer > TextBlock) 처리
            HitTestResult hitResult = VisualTreeHelper.HitTest(this, _pos);
            if (hitResult?.VisualHit is DependencyObject hitObj)
            {
                // TextBlock까지 VisualTree를 거슬러 올라가며 탐색
                DependencyObject? current = hitObj;
                while (current != null && current != this)
                {
                    if (current is TextBlock textBlock)
                    {
                        // 이 TextBlock이 IsLink=true 요소의 것인지 확인
                        foreach (RTBLogItem item in items_)
                        {
                            foreach (RTBLogItemElement element in item.Elements)
                            {
                                if (element.Option.IsLink &&
                                    element.Option.HasClick &&
                                    element.Inline is InlineUIContainer container &&
                                    container.Child == textBlock)
                                {
                                    element.Option.Click!();
                                    return true;
                                }
                            }
                        }
                    }
                    current = VisualTreeHelper.GetParent(current);
                }
            }

            // IsLink=false + Click 인 Run: RTBLogItem의 hit-test 로 처리
            foreach (RTBLogItem item in items_)
            {
                if (item.TryInvokeRunClickAt(_pos))
                    return true;
            }

            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 해당 좌표에 클릭 가능한 요소(IsLink=true 또는 IsLink=false+Click)가 있는지 확인
        // OnPreviewMouseMove에서 Hand 커서 전환 여부를 판단하기 위해 사용
        private bool HasClickableElementAt(Point _pos)
        {
            // IsLink=true(InlineUIContainer > TextBlock) 확인
            HitTestResult hitResult = VisualTreeHelper.HitTest(this, _pos);
            if (hitResult?.VisualHit is DependencyObject hitObj)
            {
                DependencyObject? current = hitObj;
                while (current != null && current != this)
                {
                    if (current is TextBlock textBlock)
                    {
                        foreach (RTBLogItem item in items_)
                        {
                            foreach (RTBLogItemElement element in item.Elements)
                            {
                                if (element.Option.IsLink &&
                                    element.Inline is InlineUIContainer container &&
                                    container.Child == textBlock)
                                    return true;
                            }
                        }
                    }
                    current = VisualTreeHelper.GetParent(current);
                }
            }

            // IsLink=false + Click 인 Run 확인
            foreach (RTBLogItem item in items_)
            {
                if (item.HasRunClickAt(_pos))
                    return true;
            }

            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void RemoveItemAt(int _index)
        {
            Document.Blocks.Remove(items_[_index].Paragraph);
            items_.RemoveAt(_index);
        }
    }
}
