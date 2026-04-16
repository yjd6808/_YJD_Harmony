/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 12:34:42 PM
 *
 */

using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Documents;
using System.Windows.Media;

namespace SGToolsCommon.Customize.Control
{
    public class RTBLogItem
    {
        private readonly List<RTBLogItemElement> elements_;
        public IReadOnlyList<RTBLogItemElement> Elements => elements_;
        public Paragraph Paragraph { get; }
        public int Type { get; }

        public string PlainText
        {
            get
            {
                StringBuilder sb = new StringBuilder();
                foreach (RTBLogItemElement element in elements_)
                    sb.Append(element.Text);
                return sb.ToString();
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public RTBLogItem(List<RTBLogItemElement> _elements, int _type)
        {
            elements_  = _elements;
            Type      = _type;
            Paragraph = BuildParagraph();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private Paragraph BuildParagraph()
        {
            Paragraph paragraph = new Paragraph();
            paragraph.Margin = new Thickness(0);

            foreach (RTBLogItemElement element in elements_)
                paragraph.Inlines.Add(element.BuildInline());

            return paragraph;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // RTBLogViewer.OnPreviewMouseLeftButtonDown에서 호출
        // IsLink=false + Click 인 Run 요소를 hit-test하여 Click() 호출
        // Run의 ContentStart/End를 화면 좌표로 변환하여 좌표 비교
        public bool TryInvokeRunClickAt(Point _pos)
        {
            foreach (RTBLogItemElement element in elements_)
            {
                if (element.Inline is not Run run)
                    continue;

                if (!element.Option.HasClick || element.Option.IsLink)
                    continue;

                if (IsPointInRun(run, _pos))
                {
                    element.Option.Click!();
                    return true;
                }
            }

            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // RTBLogViewer.HasClickableElementAt에서 호출
        // IsLink=false + Click 인 Run 요소가 해당 좌표에 있는지만 확인
        public bool HasRunClickAt(Point _pos)
        {
            foreach (RTBLogItemElement element in elements_)
            {
                if (element.Inline is not Run run)
                    continue;

                if (!element.Option.HasClick || element.Option.IsLink)
                    continue;

                if (IsPointInRun(run, _pos))
                    return true;
            }

            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static bool IsPointInRun(Run _run, Point _pos)
        {
            Rect startRect = _run.ContentStart.GetCharacterRect(LogicalDirection.Forward);
            Rect endRect   = _run.ContentEnd.GetCharacterRect(LogicalDirection.Backward);

            // 단순 한 줄 기준 비교 (여러 줄 Run 미지원)
            double top    = System.Math.Min(startRect.Top, endRect.Top);
            double bottom = System.Math.Max(startRect.Bottom, endRect.Bottom);
            double left   = startRect.Left;
            double right  = endRect.Right;

            return _pos.X >= left && _pos.X <= right && _pos.Y >= top && _pos.Y <= bottom;
        }
    }
}
