/*
 * 작성자: 윤정도
 * 생성일: 3/5/2025 10:01:08 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Customize.Control
{
    public partial class LineNumberTextBox : UserControl
    {
        private int lineCount_ = 0;
        private ScrollBarVisibility prevScrollBarVisibility_ = ScrollBarVisibility.Hidden;
        private bool updateLineNumber_ = true;

        public new bool IsLoaded = false;

        public LineNumberTextBox()
        {
            InitializeComponent();

            MainTextBox.TextChanged += MainTextBox_TextChanged;
            MainTextBox.Loaded += MainTextBox_Loaded;
            MainTextBox.PreviewKeyDown += MainTextBoxOnPreviewKeyDown;
            Loaded += LineNumberTextBox_Loaded;
        }

        private void LineNumberTextBox_Loaded(object sender, RoutedEventArgs e)
        {
            IsLoaded = true;
            UpdateLineNumbers();
        }

        // 디펜던시 프로퍼티 등록
        // 공통 프로퍼티
        public new static readonly DependencyProperty FontFamilyProperty =
            DependencyProperty.Register(nameof(FontFamily), typeof(FontFamily), typeof(LineNumberTextBox), new PropertyMetadata(new FontFamily("Courier New")));
        public new static readonly DependencyProperty FontSizeProperty =
            DependencyProperty.Register(nameof(FontSize), typeof(double), typeof(LineNumberTextBox), new PropertyMetadata(18.0));
        public static readonly DependencyProperty UseSaveEffectProperty =
            DependencyProperty.Register(nameof(UseSaveEffect), typeof(bool), typeof(LineNumberTextBox), new PropertyMetadata(true));

        // 라인넘버 프로퍼티
        public static readonly DependencyProperty LineNumberForegroundProperty =
            DependencyProperty.Register(nameof(LineNumberForeground), typeof(Brush), typeof(LineNumberTextBox), new PropertyMetadata(Brushes.DarkOliveGreen));
        public static readonly DependencyProperty LineNumberBackgroundProperty =
            DependencyProperty.Register(nameof(LineNumberBackground), typeof(Brush), typeof(LineNumberTextBox), new PropertyMetadata(Brushes.LightGray));
        public static readonly DependencyProperty LineNumberMarginProperty =
            DependencyProperty.Register(nameof(LineNumberMargin), typeof(Thickness), typeof(LineNumberTextBox), new PropertyMetadata(new Thickness(2, 0, 2, 0)));
        public static readonly DependencyProperty LineNumberStartProperty =
            DependencyProperty.Register(nameof(LineNumberStart), typeof(int), typeof(LineNumberTextBox), new PropertyMetadata(1, OnLineNumberStartChanged));
        public static readonly DependencyProperty LineNumberMaxCountProperty =
            DependencyProperty.Register(nameof(LineNumberMaxCount), typeof(int), typeof(LineNumberTextBox), new PropertyMetadata(int.MaxValue, OnLineNumberMaxCountChanged));
        public static readonly DependencyProperty LineNumberTextWidthProperty =
            DependencyProperty.Register(nameof(LineNumberTextWidth), typeof(int), typeof(LineNumberTextBox), new PropertyMetadata(1, OnLineNumberTextWidthChanged));

        // 텍박 프로퍼티
        public static readonly DependencyProperty TextBoxForegroundProperty =
            DependencyProperty.Register(nameof(TextBoxForeground), typeof(Brush), typeof(LineNumberTextBox), new PropertyMetadata(Brushes.Black));
        public static readonly DependencyProperty TextBoxBackgroundProperty =
            DependencyProperty.Register(nameof(TextBoxBackground), typeof(Brush), typeof(LineNumberTextBox), new PropertyMetadata(Brushes.White));
        public static readonly DependencyProperty TextProperty =
            DependencyProperty.Register(nameof(Text), typeof(string), typeof(LineNumberTextBox), new PropertyMetadata(string.Empty));
        public static readonly DependencyProperty TabSizeProperty =
            DependencyProperty.Register(nameof(TabSize), typeof(int), typeof(LineNumberTextBox), new PropertyMetadata(4));
        public static readonly DependencyProperty TextBoxVerticalScrollBarVisibilityProperty =
            DependencyProperty.Register(nameof(TextBoxVerticalScrollBarVisibility), typeof(ScrollBarVisibility), typeof(LineNumberTextBox), new PropertyMetadata(ScrollBarVisibility.Visible));
        

        public int TabSize
        {
            get => (int)GetValue(TabSizeProperty);
            set => SetValue(TabSizeProperty, value);
        }

        public new FontFamily FontFamily
        {
            get => (FontFamily)GetValue(FontFamilyProperty);
            set => SetValue(FontFamilyProperty, value);
        }
        public new double FontSize
        {
            get => (double)GetValue(FontSizeProperty);
            set => SetValue(FontSizeProperty, value);
        }
        public bool UseSaveEffect
        {
            get => (bool)GetValue(UseSaveEffectProperty);
            set => SetValue(UseSaveEffectProperty, value);
        }

        public Brush LineNumberForeground
        {
            get => (Brush)GetValue(LineNumberForegroundProperty);
            set => SetValue(LineNumberForegroundProperty, value);
        }
        public Brush LineNumberBackground
        {
            get => (Brush)GetValue(LineNumberBackgroundProperty);
            set => SetValue(LineNumberBackgroundProperty, value);
        }
        public Thickness LineNumberMargin
        {
            get => (Thickness)GetValue(LineNumberMarginProperty);
            set => SetValue(LineNumberMarginProperty, value);
        }
        public int LineNumberStart // 시작 라인번호를 조작한다.
        {
            get => (int)GetValue(LineNumberStartProperty);
            set => SetValue(LineNumberStartProperty, value);
        }
        // 이건 현재 종료 라인번호가 뭔지 확인할 때 사용함. 종료 라인 번호를 지정하는게 아님.
        public int LineNumberEnd => LineNumberStart + lineCount_ - 1;
        public int LineNumberEndPlusOne => LineNumberStart + lineCount_;

        public int LineNumberMaxCount
        {
            get => (int)GetValue(LineNumberMaxCountProperty);
            set => SetValue(LineNumberMaxCountProperty, value);
        }

        public Brush TextBoxForeground
        {
            get => (Brush)GetValue(TextBoxForegroundProperty);
            set => SetValue(TextBoxForegroundProperty, value);
        }
        public Brush TextBoxBackground
        {
            get => (Brush)GetValue(TextBoxBackgroundProperty);
            set => SetValue(TextBoxBackgroundProperty, value);
        }
        public string Text
        {
            get => (string)GetValue(TextProperty);
            set => SetValue(TextProperty, value);
        }
        public ScrollBarVisibility TextBoxVerticalScrollBarVisibility
        {
            get => (ScrollBarVisibility)GetValue(TextBoxVerticalScrollBarVisibilityProperty);
            set => SetValue(TextBoxVerticalScrollBarVisibilityProperty, value);
        }

        public int LineNumberTextWidth
        {
            get => (int)GetValue(LineNumberTextWidthProperty);
            set => SetValue(LineNumberTextWidthProperty, value);
        }

        public static readonly RoutedEvent SaveRequestedEvent = EventManager.RegisterRoutedEvent(
            nameof(SaveRequested), RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(LineNumberTextBox));
        public event RoutedEventHandler SaveRequested
        {
            add { AddHandler(SaveRequestedEvent, value); }
            remove { RemoveHandler(SaveRequestedEvent, value); }
        }

        // LineCountChanged RoutedEvent 등록
        public static readonly RoutedEvent LineCountChangedEvent = EventManager.RegisterRoutedEvent(
            nameof(LineCountChanged), RoutingStrategy.Bubble, typeof(EventHandler<LineCountChangedEventArgs>), typeof(LineNumberTextBox));
        public event EventHandler<LineCountChangedEventArgs> LineCountChanged
        {
            add { AddHandler(LineCountChangedEvent, value); }
            remove { RemoveHandler(LineCountChangedEvent, value); }
        }

        private void MainTextBox_Loaded(object sender, RoutedEventArgs e)
        {
            UpdateLineNumbers();
        }

        private void MainTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!updateLineNumber_)
                return;

            TextChange? change = e.Changes.Count > 0 ? e.Changes.Last() : null;
            if (change != null && change.AddedLength > 0)
            {
                string mainText = MainTextBox.Text;
                for (int i = change.Offset; i < change.Offset + change.AddedLength; ++i)
                {
                    if (mainText[i] == '\n')
                    {
                        UpdateLineNumbers();
                        break;
                    }
                }
            }
            else
            {
                UpdateLineNumbers();
            }
        }

        private static void OnLineNumberStartChanged(DependencyObject _d, DependencyPropertyChangedEventArgs _e)
        {
            var a = (LineNumberTextBox)_d;
            if (a.IsLoaded)
            {
                a.UpdateLineNumbers();
            }
            
        }
        private static void OnLineNumberMaxCountChanged(DependencyObject _d, DependencyPropertyChangedEventArgs _e)
        {
            var a = (LineNumberTextBox)_d;
            if (a.IsLoaded)
            {
                a.UpdateLineNumbers();
            }
        }
        private static void OnLineNumberTextWidthChanged(DependencyObject _d, DependencyPropertyChangedEventArgs e)
        {
            var a = (LineNumberTextBox)_d;
            if (a.IsLoaded)
            {
                a.UpdateLineNumbers();
            }
        }

        private void UpdateLineNumbers(int _modifyLineCount = int.MinValue + 1)
        {
            int prevLineCount = lineCount_;
            int lineCount = lineCount_;
            if (_modifyLineCount == int.MinValue + 1) // 매직 넘버. 전체 업데이트 수행
            {
                string text = MainTextBox.Text;
                lineCount = text.Count(_x => _x == '\n') + 1;
            }
            else // 부분 업데이트
            {
                lineCount += _modifyLineCount;
            }

            // LineNumberMaxCount 적용
            int maxCount = LineNumberMaxCount;
            if (lineCount > maxCount)
                lineCount = maxCount;

            int start = LineNumberStart;
            int width = LineNumberTextWidth;
            StringBuilder sb = new StringBuilder(lineCount + 10);
            int i = 0;
            for (i = 0; i < lineCount - 1; i++)
            {
                string num = (start + i).ToString();
                sb.AppendLine(num.PadLeft(width, ' '));
            }

            if (i == lineCount - 1)
            {
                string num = (start + i).ToString();
                sb.Append(num.PadLeft(width, ' '));
            }

            LineNumberedTextBox.Text = sb.ToString();
            lineCount_ = lineCount;

            // LineCountChanged 이벤트 발생
            if (prevLineCount != lineCount)
            {
                var visibility = TextBoxVerticalScrollBarVisibility;
                if (lineCount == 1 && visibility != ScrollBarVisibility.Hidden)
                {
                    prevScrollBarVisibility_ = visibility;
                    TextBoxVerticalScrollBarVisibility = ScrollBarVisibility.Hidden;
                }
                else if (lineCount >= 2)
                {
                    if (prevScrollBarVisibility_ != ScrollBarVisibility.Hidden)
                    {
                        TextBoxVerticalScrollBarVisibility = prevScrollBarVisibility_;
                    }
                }

                var args = new LineCountChangedEventArgs(LineCountChangedEvent, prevLineCount, lineCount_);
                RaiseEvent(args);
            }
        }

        // tracking MainTextBox 수행
        private void TextBox_ScrollChanged(object sender, ScrollChangedEventArgs e)
        {
            LineNumberedTextBox.ScrollToVerticalOffset(MainTextBox.VerticalOffset);
        }

        /*
          Visual Studio의 소스코드 IndentationStyle을 따라가도록 한다.
          하.. 구현 쉬울거라고 생각했는데.. 몇시간이나 걸림. 예상외의 복병이다 ㅠ

          1. 다음 기능 구현이 필요하다.
          Shift + Tab
            - 싱글라인 멀티라인 모두 해당 라인의 시작지점의 공백을 tabSize 갯수만큼 제거해준다.
          Tab
            1. 선택된 텍스트가 있는 경우
               1. 싱글라인인 경우
                    - 텍스트를 Tab으로 교체한다.
               2. 멀티라인인 경우
                    - 선택된 라인의 시작부분에 Tab을 삽입한다.
            2. 선택된 텍스트가 없는 경우
               - 현재 커서위치에 탭을 삽입한다.
         */
        private void MainTextBoxOnPreviewKeyDown(object _sender, KeyEventArgs _e)
        {
            if (_e.Key == Key.Tab)
            {
                ProcessTab(_e);
            }
            else if (_e.Key == Key.Delete)
            {
                ProcessDelete(_e);
            }
            else if (_e.Key == Key.S)
            {
                ProcessSave(_e);
            }
        }

        private void ProcessSave(KeyEventArgs _e)
        {
            if (Keyboard.Modifiers.HasFlag(ModifierKeys.Control))
            {
                if (UseSaveEffect)
                {
                    AnimationManager.BeginFadeoutAnimation(MainBorder, 30);
                }

                RoutedEventArgs args = new RoutedEventArgs(SaveRequestedEvent);
                RaiseEvent(args);
                _e.Handled = true;
            }
        }

        private void ProcessDelete(KeyEventArgs _e)
        {
            var textBox = MainTextBox;

            // Shift + Delete 구현 (한 줄 제거)
            if (Keyboard.Modifiers.HasFlag(ModifierKeys.Shift) && textBox.SelectionLength == 0)
            {
                var text = textBox.Text;
                int selectionStart = textBox.SelectionStart;

                // 현재 라인의 시작 위치 찾기
                int lineStartIndex = text.LastIndexOf('\n', Math.Min(selectionStart, text.Length - 1));
                if (lineStartIndex == -1) 
                    lineStartIndex = 0; // 첫 번째 라인

                // 현재 라인의 끝 위치 찾기
                int lineEndIndex = text.IndexOf('\n', selectionStart);
                if (lineEndIndex == -1) 
                    lineEndIndex = text.Length; // 마지막 라인

                // 텍스트 갱신 (라인 삭제)
                if (lineStartIndex == 0)
                {
                    int removeCount = Math.Min((lineEndIndex - lineStartIndex) + 1, text.Length);
                    text = text.Remove(0, removeCount); // 첫 번째 줄
                }
                else
                {
                    if (lineStartIndex >= 0 && lineEndIndex == text.Length) // 라인이 1개라도 존재하는 상태에서 마지막줄을 삭제하는 경우
                    {
                        text = text.Remove(lineStartIndex - 1, lineEndIndex - lineStartIndex + 1); // 이전 개행 문자까지 제거
                    }
                    else
                    {
                        text = text.Remove(lineStartIndex, lineEndIndex - lineStartIndex);
                    }

                        
                }
                // 커서 위치 조정
                // BeginChange/EndChange로 감싸줘야 Clear/AppendText로 수행되는 변경점이 undo stack 카운트 1개로 취급된다.
                // 뿐만 아니라 단순 Text 대입으로는 undo stack 카운트 취급이 안됨.
                textBox.BeginChange();
                textBox.Clear();
                textBox.AppendText(text);
                textBox.EndChange();
                textBox.SelectionStart = lineStartIndex;
                _e.Handled = true;
            }
        }

        private void ProcessTab(KeyEventArgs _e)
        {
            updateLineNumber_ = false;
            int tabSize = TabSize;
            string tabString = new string(' ', tabSize);    

            int selectionStart = MainTextBox.SelectionStart;
            int selectionLength = MainTextBox.SelectionLength;
            string text = MainTextBox.Text;

            // https://learn.microsoft.com/en-us/dotnet/api/system.windows.controls.primitives.textboxbase.beginchange?view=windowsdesktop-9.0
            // selection, content 변경 이벤트가 발생하지 않는다.
            MainTextBox.BeginChange();

            if (Keyboard.Modifiers.HasFlag(ModifierKeys.Shift)) // Shift + Tab (내어쓰기)
            {
                StringBuilder sb = new StringBuilder(text);

                // 선택된 텍스트가 없다.
                if (selectionLength == 0)
                {
                    int lineStart = text.LastIndexOf('\n', Math.Min(selectionStart, text.Length - 1)) + 1;
                    int spaceCount = 0;

                    while (spaceCount < tabSize && lineStart + spaceCount < text.Length &&
                           text[lineStart + spaceCount] == ' ')
                        spaceCount++;

                    if (spaceCount > 0)
                    {
                        sb.Remove(lineStart, spaceCount);
                        selectionStart = Math.Max(selectionStart - spaceCount, lineStart);
                        MainTextBox.Clear();
                        MainTextBox.AppendText(sb.ToString());
                        MainTextBox.SelectionStart = selectionStart;
                        MainTextBox.SelectionLength = 0;
                    }
                }
                else // 선택된 텍스트가 있을 때
                {
                    List<int> indexes = new();
                    int lineCount = text.CountOf('\n', selectionStart, selectionLength, indexes) + 1;

                    if (lineCount > 1)
                    {
                        int otherSpaceCount = 0;

                        for (int i = indexes.Count - 1; i >= 0; --i)
                        {
                            int lineStart = indexes[i] + 1;
                            int spaceCount = 0;

                            while (spaceCount < tabSize && lineStart + spaceCount < text.Length &&
                                   text[lineStart + spaceCount] == ' ')
                                spaceCount++;

                            if (spaceCount > 0)
                                sb.Remove(lineStart, spaceCount);

                            otherSpaceCount += spaceCount;
                        }

                        // 첫 번째 라인의 공백 제거
                        int start = indexes[0] - 1;
                        int firstLineStart = text.LastIndexOf('\n', start) + 1;
                        int firstSpaceCount = 0;

                        while (firstSpaceCount < tabSize && firstLineStart + firstSpaceCount < text.Length &&
                               text[firstLineStart + firstSpaceCount] == ' ')
                            firstSpaceCount++;

                        if (firstSpaceCount > 0)
                            sb.Remove(firstLineStart, firstSpaceCount);

                        if (otherSpaceCount > 0 || firstSpaceCount > 0)
                        {
                            int removedFirstSpaceCount = firstSpaceCount;
                            if (firstLineStart + firstSpaceCount >= selectionStart)
                            {
                                firstSpaceCount = firstLineStart + firstSpaceCount - selectionStart;
                            }
                            else
                            {
                                firstSpaceCount = 0;
                            }

                            MainTextBox.Clear();
                            MainTextBox.AppendText(sb.ToString());
                            MainTextBox.SelectionStart = Math.Max(selectionStart - removedFirstSpaceCount,
                                firstLineStart);
                            MainTextBox.SelectionLength = selectionLength - otherSpaceCount - firstSpaceCount;
                        }
                    }
                    else
                    {
                        // 한 줄만 선택된 경우, 앞 공백을 제거 (선택된 텍스트가 없는 경우와 로직 동일)
                        int lineStart = text.LastIndexOf('\n', Math.Min(selectionStart, text.Length - 1)) + 1;
                        int spaceCount = 0;

                        while (spaceCount < tabSize && lineStart + spaceCount < text.Length &&
                               text[lineStart + spaceCount] == ' ')
                            spaceCount++;

                        if (spaceCount > 0)
                        {
                            sb.Remove(lineStart, spaceCount);
                            selectionStart = Math.Max(selectionStart - spaceCount, lineStart);
                            MainTextBox.Clear();
                            MainTextBox.AppendText(sb.ToString());
                            MainTextBox.SelectionStart = selectionStart;
                            MainTextBox.SelectionLength = selectionLength;
                        }
                    }
                }
            }
            else // Tab (들여쓰기)
            {
                StringBuilder sb = new StringBuilder(text);

                if (selectionLength == 0)
                {
                    // 선택된 텍스트가 없으면 해당 위치에 탭 삽입
                    sb.Insert(selectionStart, tabString);
                    MainTextBox.Clear();
                    MainTextBox.AppendText(sb.ToString());
                    MainTextBox.SelectionStart = selectionStart + tabSize;
                    MainTextBox.SelectionLength = 0;
                }
                else // 선택된 텍스트가 있을 때
                {
                    List<int> indexes = new();
                    int lineCount = text.CountOf('\n', selectionStart, selectionLength, indexes) + 1;

                    if (lineCount > 1)
                    {
                        for (int i = indexes.Count - 1; i >= 0; --i)
                            sb.Insert(indexes[i] + 1, tabString);

                        int start = indexes[0] - 1;
                        int firstLineIndex =
                            text.LastIndexOf('\n', start);
                        sb.Insert(firstLineIndex + 1, tabString);

                        MainTextBox.Clear();
                        MainTextBox.AppendText(sb.ToString());
                        MainTextBox.SelectionStart = selectionStart + tabSize;
                        MainTextBox.SelectionLength = selectionLength + (tabSize * (lineCount - 1));
                    }
                    else
                    {
                        // 선택된 텍스트가 한 줄에 있을 경우, 선택된 텍스트를 tabString으로 교체
                        sb.Remove(selectionStart, selectionLength);
                        sb.Insert(selectionStart, tabString);
                        MainTextBox.Clear();
                        MainTextBox.AppendText(sb.ToString());
                        MainTextBox.SelectionStart = selectionStart + tabSize;
                        MainTextBox.SelectionLength = 0;
                    }
                }
            }

            MainTextBox.EndChange();
            updateLineNumber_ = true;
            _e.Handled = true;
        }

        public void SetText(string _text, bool _undo = true)
        {
            MainTextBox.BeginChange();
            MainTextBox.Clear();
            MainTextBox.AppendText(_text);
            MainTextBox.EndChange();
        }
    }

    public class LineCountChangedEventArgs : RoutedEventArgs
    {
        public int PrevLineCount { get; }
        public int CurrentLineCount { get; }

        public LineCountChangedEventArgs(RoutedEvent routedEvent, int prevLineCount, int currentLineCount)
            : base(routedEvent)
        {
            PrevLineCount = prevLineCount;
            CurrentLineCount = currentLineCount;
        }
    }
}
