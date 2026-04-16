/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 12:34:42 PM
 *
 */

using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;

namespace SGToolsCommon.Customize.Control
{
    public class RTBLogItemElement
    {
        private readonly string text_;
        private readonly RTBTextOption option_;
        private Inline? inline_;

        public string Text => text_;
        public RTBTextOption Option => option_;
        public Inline? Inline => inline_;

        //////////////////////////////////////////////////////////////////////////////////
        public RTBLogItemElement(string _text, RTBTextOption _option)
        {
            text_   = _text;
            option_ = _option;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public Inline BuildInline()
        {
            inline_ = option_.IsLink ? BuildLinkInline() : BuildRunInline();
            return inline_;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private Run BuildRunInline()
        {
            Run run = new Run(text_);
            option_.ApplyTo(run);
            return run;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // IsLink = true인 경우 InlineUIContainer + TextBlock으로 생성
        // - 커서를 손 모양으로 변경
        // - 밑줄 자동 추가
        // - Click이 있으면 마우스 클릭 이벤트 연결
        // 단, 드래그 범위 선택에서는 텍스트가 제외되는 문제가 있음
        // RTBLogViewer의 Copy 커맨드 오버라이드에서 처리함
        private InlineUIContainer BuildLinkInline()
        {
            TextBlock textBlock = new TextBlock { Text = text_ };
            option_.ApplyTo(textBlock);
            textBlock.Cursor = Cursors.Hand;
            textBlock.TextDecorations = System.Windows.TextDecorations.Underline;

            if (option_.HasClick)
                textBlock.MouseLeftButtonDown += (_, _) => option_.Click!();

            return new InlineUIContainer(textBlock);
        }
    }
}
