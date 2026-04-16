/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 12:34:42 PM
 *
 */

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;

namespace SGToolsCommon.Customize.Control
{
    public class RTBTextOption
    {
        public Brush? Foreground { get; set; } = null;
        public Brush? Background { get; set; } = null;
        public double FontSize { get; set; } = double.NaN;
        public FontFamily? FontFamily { get; set; } = null;
        public RTBTextDecoration Decoration { get; set; } = RTBTextDecoration.None;
        public Action? Click { get; set; } = null;
        public bool IsLink { get; set; } = false;

        public bool HasClick => Click != null;

        //////////////////////////////////////////////////////////////////////////////////
        public RTBTextOption Clone()
        {
            return new RTBTextOption
            {
                Foreground = Foreground,
                Background = Background,
                FontSize   = FontSize,
                FontFamily = FontFamily,
                Decoration = Decoration,
                Click      = Click,
                IsLink     = IsLink,
            };
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void MergeFrom(RTBTextOption? _override)
        {
            if (_override == null)
                return;

            if (_override.Foreground != null)
                Foreground = _override.Foreground;

            if (_override.Background != null)
                Background = _override.Background;

            if (!double.IsNaN(_override.FontSize))
                FontSize = _override.FontSize;

            if (_override.FontFamily != null)
                FontFamily = _override.FontFamily;

            if (_override.Decoration != RTBTextDecoration.None)
                Decoration = _override.Decoration;

            if (_override.Click != null)
                Click = _override.Click;

            if (_override.IsLink)
                IsLink = true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void ApplyTo(Run _run)
        {
            if (Foreground != null)
                _run.Foreground = Foreground;

            if (Background != null)
                _run.Background = Background;

            if (!double.IsNaN(FontSize))
                _run.FontSize = FontSize;

            if (FontFamily != null)
                _run.FontFamily = FontFamily;

            if (Decoration.HasFlag(RTBTextDecoration.Bold))
                _run.FontWeight = FontWeights.Bold;

            if (Decoration.HasFlag(RTBTextDecoration.Italic))
                _run.FontStyle = FontStyles.Italic;

            if (Decoration.HasFlag(RTBTextDecoration.Underline))
                _run.TextDecorations = System.Windows.TextDecorations.Underline;

            if (Decoration.HasFlag(RTBTextDecoration.Strikethrough))
                _run.TextDecorations = System.Windows.TextDecorations.Strikethrough;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void ApplyTo(TextBlock _textBlock)
        {
            if (Foreground != null)
                _textBlock.Foreground = Foreground;

            if (Background != null)
                _textBlock.Background = Background;

            if (!double.IsNaN(FontSize))
                _textBlock.FontSize = FontSize;

            if (FontFamily != null)
                _textBlock.FontFamily = FontFamily;

            if (Decoration.HasFlag(RTBTextDecoration.Bold))
                _textBlock.FontWeight = FontWeights.Bold;

            if (Decoration.HasFlag(RTBTextDecoration.Italic))
                _textBlock.FontStyle = FontStyles.Italic;

            if (Decoration.HasFlag(RTBTextDecoration.Underline))
                _textBlock.TextDecorations = System.Windows.TextDecorations.Underline;

            if (Decoration.HasFlag(RTBTextDecoration.Strikethrough))
                _textBlock.TextDecorations = System.Windows.TextDecorations.Strikethrough;
        }
    }
}
