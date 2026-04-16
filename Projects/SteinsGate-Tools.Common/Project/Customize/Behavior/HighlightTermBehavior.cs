/*
 * 원본 소스코드: https://stackoverflow.com/questions/751741/wpf-textblock-highlight-certain-parts-based-on-search-condition/5183538#5183538
 * [추가한 기능들]
 *  - 전경/배경색 하이라이팅 추가
 *  - 하이라이팅 된게 있을 경우의 이벤트 라우팅 - TextHighlited
 *  - 하이라이팅 필터를 위한 이벤트 라우팅 - TextPreviewChanged
 *  - 하이라이팅 초기화 기능 - ResetHighlight
 * 생성일: 8/30/2024 4:30:37 PM
 */

using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows;
using System.Linq;

namespace SGToolsCommon.Customize.Behavior
{
    public static class HighlightTermBehavior
    {
        public static readonly DependencyProperty TextProperty = DependencyProperty.RegisterAttached(
            "Text",
            typeof(string),
            typeof(HighlightTermBehavior),
            new FrameworkPropertyMetadata("", OnTextChanged));

        public static string GetText(FrameworkElement _frameworkElement) => (string)_frameworkElement.GetValue(TextProperty);
        public static void SetText(FrameworkElement _frameworkElement, string _value) => _frameworkElement.SetValue(TextProperty, _value);

        public static readonly DependencyProperty TermTextProperty = DependencyProperty.RegisterAttached(
            "TermText",
            typeof(string),
            typeof(HighlightTermBehavior),
            new FrameworkPropertyMetadata("", OnTextChanged));

        public static string GetTermText(FrameworkElement _frameworkElement) => (string)_frameworkElement.GetValue(TermTextProperty);
        public static void SetTermText(FrameworkElement _frameworkElement, string _value) => _frameworkElement.SetValue(TermTextProperty, _value);

        public static readonly DependencyProperty ForegroundProperty = DependencyProperty.RegisterAttached(
            "Foreground",
            typeof(Brush),
            typeof(HighlightTermBehavior),
            new FrameworkPropertyMetadata(Brushes.Black));
        public static Brush GetForeground(FrameworkElement _frameworkElement) => (Brush)_frameworkElement.GetValue(ForegroundProperty);
        public static void SetForeground(FrameworkElement _frameworkElement, Brush _value) => _frameworkElement.SetValue(ForegroundProperty, _value);

        public static readonly DependencyProperty BackgroundProperty = DependencyProperty.RegisterAttached(
            "Background",
            typeof(Brush),
            typeof(HighlightTermBehavior),
            new FrameworkPropertyMetadata(Brushes.Transparent));
        public static Brush GetBackground(FrameworkElement _frameworkElement) => (Brush)_frameworkElement.GetValue(BackgroundProperty);
        public static void SetBackground(FrameworkElement _frameworkElement, Brush _value) => _frameworkElement.SetValue(BackgroundProperty, _value);


        public static readonly RoutedEvent TextHighlightedEvent = EventManager.RegisterRoutedEvent(
            "TextHighlighted", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(HighlightTermBehavior));
        public static void AddTextHighlightedHandler(DependencyObject _d, RoutedEventHandler _handler)
        {
            if (_d is UIElement element)
                element.AddHandler(TextHighlightedEvent, _handler);
        }
        public static void RemoveTextHighlightedHandler(DependencyObject _d, RoutedEventHandler _handler)
        {
            if (_d is UIElement element)
                element.RemoveHandler(TextHighlightedEvent, _handler);
        }

        public static readonly RoutedEvent TextPreviewChangedEvent = EventManager.RegisterRoutedEvent(
            "TextPreviewChanged", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(HighlightTermBehavior));
        public static void AddTextPreviewChangedHandler(DependencyObject _d, RoutedEventHandler _handler)
        {
            if (_d is UIElement element)
                element.AddHandler(TextPreviewChangedEvent, _handler);
        }
        public static void RemoveTextPreviewChangedHandler(DependencyObject _d, RoutedEventHandler _handler)
        {
            if (_d is UIElement element)
                element.RemoveHandler(TextPreviewChangedEvent, _handler);
        }

        private static void OnTextChanged(DependencyObject _d, DependencyPropertyChangedEventArgs _e)
        {
            if (_d is TextBlock textBlock)
            {
                var args = new RoutedEventArgs(TextPreviewChangedEvent);
                textBlock.RaiseEvent(args);
                if (args.Handled)
                    return;
                SetTextBlockTextAndHighlightTerm(textBlock, GetText(textBlock), GetTermText(textBlock));
            }
        }

        private static void SetTextBlockTextAndHighlightTerm(TextBlock _textBlock, string _text, string _termText)
        {
            _textBlock.Text = string.Empty;

            if (TextIsEmpty(_text))
                return;

            if (TextIsNotContainingTermToBeHighlighted(_text, _termText))
            {
                AddPartToTextBlock(_textBlock, _text);
                return;
            }

            var textParts = SplitTextIntoTermAndNotTermParts(_text, _termText);
            bool isHiglitedtextExist = false;

            foreach (var textPart in textParts)
            {
                if (AddPartToTextBlockAndHighlightIfNecessary(_textBlock, _termText, textPart))
                {
                    isHiglitedtextExist = true;
                }
            }

            if (isHiglitedtextExist)
            {
                var args = new RoutedEventArgs(TextHighlightedEvent);
                _textBlock.RaiseEvent(args);
            }
        }

        private static bool TextIsEmpty(string _text)
        {
            return _text.Length == 0;
        }

        private static bool TextIsNotContainingTermToBeHighlighted(string _text, string _termToBeHighlighted)
        {
            return _text.Contains(_termToBeHighlighted) == false;
        }

        private static bool AddPartToTextBlockAndHighlightIfNecessary(TextBlock _textBlock, string _termToBeHighlighted, string _textPart)
        {
            if (_textPart == _termToBeHighlighted)
            {
                AddHighlightedPartToTextBlock(_textBlock, _textPart);
                return true;
            }
            else
            {
                AddPartToTextBlock(_textBlock, _textPart);
                return false;
            }
        }

        private static void AddPartToTextBlock(TextBlock _textBlock, string _part)
        {
            _textBlock.Inlines.Add(new Run
            {
                Text = _part, 
                //FontWeight = FontWeights.Light,
            });
        }

        private static void AddHighlightedPartToTextBlock(TextBlock _textBlock, string _part)
        {
            _textBlock.Inlines.Add(new Run
            {
                Text = _part, 
                //FontWeight = FontWeights.ExtraBold,
                Foreground = GetForeground(_textBlock),
                Background = GetBackground(_textBlock),
            });
        }

        public static List<string> SplitTextIntoTermAndNotTermParts(string _text, string _term)
        {
            if (_text.Length == 0)
                return new List<string>() { string.Empty };

            return Regex.Split(_text, $@"({Regex.Escape(_term)})")
                        .Where(_p => _p != string.Empty)
                        .ToList();
        }

        public static void ResetHighlight(TextBlock _tb)
        {
            _tb.Text = string.Empty;
            _tb.Text = GetText(_tb);
        }

        public static void Highlight(TextBlock _tb, bool _call = false)
        {
            _tb.Text = string.Empty;
            string text = GetText(_tb);
            AddHighlightedPartToTextBlock(_tb, text);

            if (_call)
            {
                var args = new RoutedEventArgs(TextHighlightedEvent);
                _tb.RaiseEvent(args);
            }
        }

        // 하이라이팅된 텍스트와 원본 텍스트가 같은지
        public static bool IsPerfectHighlighted(TextBlock? _tb)
        {
            if (_tb == null)
                return false;

            string termText = GetTermText(_tb);
            string text = GetText(_tb);
            return termText == text;
        }
    }
}
