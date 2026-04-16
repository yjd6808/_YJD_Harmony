/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 12:34:42 PM
 *
 */

using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using SGToolsCommon.Customize.Control;

namespace SGToolsResearch
{
    public class RTBLogViewerResearch : Research
    {
        public override string Name => "RTBLogViewer";

        public override Window CreateWindow()
        {
            RTBLogViewerResearchWindow window = new RTBLogViewerResearchWindow();
            window.Loaded += (_s, _e) => RunInWindow(window);
            return window;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Run() { }

        //////////////////////////////////////////////////////////////////////////////////
        private void RunInWindow(RTBLogViewerResearchWindow _window)
        {
            RTBLogViewer viewer = _window.LogViewer;
            viewer.Clear();
            viewer.Background = Brushes.Black;

            RTBTextOption baseWhite = new RTBTextOption
            {
                Foreground = Brushes.White,
                FontSize   = 13,
            };

            // ================================================================================
            // [테스트 1] 단순 텍스트 추가 (WriteLine 편의 메서드)
            // ================================================================================
            viewer.WriteLine("===== [테스트 1] 단순 텍스트 추가 =====", baseWhite);
            viewer.WriteLine("단순 텍스트: Hello, RTBLogViewer!", baseWhite);

            // ================================================================================
            // [테스트 2] Builder - baseOption + Text 오버라이드
            // ================================================================================
            viewer.Writer()
                .Option(baseWhite)
                .Text("===== [테스트 2] Builder + Text 오버라이드 =====")
                .Submit();

            viewer.Writer()
                .Option(baseWhite)
                .Text("[INFO] ", new RTBTextOption { Foreground = Brushes.LightGreen, Decoration = RTBTextDecoration.Bold })
                .Text("정상 처리 되었습니다.")
                .Submit();

            // ================================================================================
            // [테스트 3] IsLink=true 의 InlineUIContainer + 밑줄 + 손 모양 커서
            // ================================================================================
            viewer.Writer()
                .Option(baseWhite)
                .Text("===== [테스트 3] IsLink=true (InlineUIContainer) =====")
                .Submit();

            viewer.Writer()
                .Option(baseWhite)
                .Text("[ERROR] ", new RTBTextOption { Foreground = Brushes.Red, Decoration = RTBTextDecoration.Bold })
                .Link("이 링크", new RTBTextOption  // IsLink=true 자동 설정 확인
                {
                    Foreground = Brushes.Yellow,
                    Click      = () => MessageBox.Show("IsLink=true 클릭!", "테스트 3"),
                })
                .Text("에 오류가 발생했습니다.")
                .SetType(5000)
                .Submit();

            // ================================================================================
            // [테스트 4] IsLink=false + Click 의 Run + Paragraph hit-test 처리
            // ================================================================================
            viewer.Writer()
                .Option(baseWhite)
                .Text("===== [테스트 4] IsLink=false + Click (Run hit-test) =====")
                .Submit();

            viewer.Writer()
                .Option(baseWhite)
                .Text("[CLICK] ", new RTBTextOption { Foreground = Brushes.Orange, Decoration = RTBTextDecoration.Bold })
                .Text("이 텍스트를 클릭하세요", new RTBTextOption
                {
                    Foreground = Brushes.Cyan,
                    IsLink     = false,  // 명시적 false (Run에서 동작)
                    Click      = () => MessageBox.Show("IsLink=false Run 클릭!", "테스트 4"),
                })
                .Text(" (IsLink=false, 드래그 범위 불가)")
                .Submit();

            // ================================================================================
            // [테스트 5] Link() 오버로드 메서드 3종류 확인
            // ================================================================================
            viewer.Writer()
                .Option(baseWhite)
                .Text("===== [테스트 5] Link() 오버로드 3종 =====")
                .Submit();

            viewer.Writer()
                .Option(baseWhite)
                .Text("Link(text): ")
                .Link("클릭 없는 링크")                                                  // 오버로드 1
                .Text(" | Link(text, click): ")
                .Link("클릭 있는 링크", () => MessageBox.Show("Link(text, click)!", "테스트 5"))  // 오버로드 2
                .Text(" | Link(text, option): ")
                .Link("옵션 링크", new RTBTextOption                                    // 오버로드 3
                {
                    Foreground = Brushes.Magenta,
                    Click      = () => MessageBox.Show("Link(text, option)!", "테스트 5"),
                })
                .Submit();

            // ================================================================================
            // [테스트 6] RTBTextDecoration 확인 (Bold, Italic, Underline, Strikethrough)
            // ================================================================================
            viewer.Writer()
                .Option(baseWhite)
                .Text("===== [테스트 6] RTBTextDecoration 확인 =====")
                .Submit();

            viewer.Writer()
                .Option(baseWhite)
                .Text("Bold", RTBTextDecoration.Bold)
                .Text(" | ")
                .Text("Italic", RTBTextDecoration.Italic)
                .Text(" | ")
                .Text("Underline", RTBTextDecoration.Underline)
                .Text(" | ")
                .Text("Strikethrough", RTBTextDecoration.Strikethrough)
                .Text(" | ")
                .Text("Bold+Italic", RTBTextDecoration.Bold | RTBTextDecoration.Italic)
                .Submit();

            // ================================================================================
            // [테스트 7] SetType + Find / FindIndex
            // ================================================================================
            viewer.Writer()
                .Option(baseWhite)
                .Text("===== [테스트 7] SetType + Find / FindIndex =====")
                .Submit();

            viewer.Writer()
                .Option(new RTBTextOption { Foreground = Brushes.Gold })
                .Text("[TYPE=9999] 이 항목을 Find/FindIndex로 찾습니다.")
                .SetType(9999)
                .Submit();

            RTBLogItem found = viewer.Find(_item => _item.Type == 9999);
            int foundIdx     = viewer.FindIndex(_item => _item.Type == 9999);

            viewer.Writer()
                .Option(baseWhite)
                .Text($"Find 결과: {(found != null ? $"찾음 (PlainText='{found.PlainText}')" : "없음")}")
                .Submit();

            viewer.Writer()
                .Option(baseWhite)
                .Text($"FindIndex 결과: {foundIdx}")
                .Submit();

            // ================================================================================
            // [테스트 8] FindAll + IsExist
            // ================================================================================
            viewer.Writer()
                .Option(baseWhite)
                .Text("===== [테스트 8] FindAll + IsExist =====")
                .Submit();

            for (int i = 0; i < 3; i++)
            {
                viewer.Writer()
                    .Option(new RTBTextOption { Foreground = Brushes.LightBlue })
                    .Text($"[TYPE=7777] 반복 항목 #{i + 1}")
                    .SetType(7777)
                    .Submit();
            }

            var all7777    = viewer.FindAll(_item => _item.Type == 7777);
            bool exist7777 = viewer.IsExist(_item => _item.Type == 7777);
            bool exist0000 = viewer.IsExist(_item => _item.Type == 1234567);

            viewer.Writer()
                .Option(baseWhite)
                .Text($"FindAll(7777) 결과: {all7777.Count} | IsExist(7777): {exist7777} | IsExist(1234567): {exist0000}")
                .Submit();

            // ================================================================================
            // [테스트 9] GetPlainText (전체 / 특정 항목)
            // ================================================================================
            viewer.Writer()
                .Option(baseWhite)
                .Text("===== [테스트 9] GetPlainText =====")
                .Submit();

            string itemPlain = viewer.GetPlainText(found);
            viewer.Writer()
                .Option(baseWhite)
                .Text($"특정항목 PlainText: '{itemPlain}'")
                .Submit();

            viewer.Writer()
                .Option(baseWhite)
                .Text("전체 PlainText를 MessageBox로 확인: ")
                .Link("클릭", () => MessageBox.Show(viewer.GetPlainText(), "GetPlainText() 전체"))
                .Submit();

            // ================================================================================
            // [테스트 10] Select / SelectAll / CopySelection / CopyAll
            // ================================================================================
            viewer.Writer()
                .Option(baseWhite)
                .Text("===== [테스트 10] Select / SelectAll / Copy =====")
                .Submit();

            viewer.Writer()
                .Option(baseWhite)
                .Link("TYPE=9999 항목 선택", () => { if (found != null) viewer.Select(found); })
                .Text(" | ")
                .Link("전체 선택(SelectAll)", () => viewer.SelectAll())
                .Text(" | ")
                .Link("선택 복사(CopySelection)", () => viewer.CopySelection())
                .Text(" | ")
                .Link("전체 복사(CopyAll)", () => viewer.CopyAll())
                .Submit();

            // ================================================================================
            // [테스트 11] RemoveAt / RemoveAll / Clear (주의: Clear하면 다 사라짐)
            // ================================================================================
            viewer.Writer()
                .Option(baseWhite)
                .Text("===== [테스트 11] RemoveAt / RemoveAll =====")
                .Submit();

            viewer.Writer()
                .Option(new RTBTextOption { Foreground = Brushes.Tomato })
                .Text("[REMOVE_TARGET] 이 항목은 RemoveAll로 제거됩니다.")
                .SetType(8888)
                .Submit();

            viewer.Writer()
                .Option(baseWhite)
                .Link("RemoveAll(TYPE=8888)", () =>
                {
                    viewer.RemoveAll(_item => _item.Type == 8888);
                    MessageBox.Show("TYPE=8888 항목 제거 완료!", "테스트 11");
                })
                .Text(" | ")
                .Link("RemoveAt(0)", () =>
                {
                    if (viewer.GetCount() > 0)
                        viewer.RemoveAt(0);
                    MessageBox.Show("인덱스 0 항목 제거 완료!", "테스트 11");
                })
                .Submit();

            // ================================================================================
            // [테스트 12] Capacity + ScrollTo / ScrollAt
            // ================================================================================
            viewer.Writer()
                .Option(baseWhite)
                .Text("===== [테스트 12] Capacity + Scroll =====")
                .Submit();

            viewer.SetCapacity(500);
            for (int i = 1; i <= 80; i++)
            {
                viewer.Writer()
                    .Option(new RTBTextOption { Foreground = i % 2 == 0 ? Brushes.White : Brushes.LightGray })
                    .Text($"[{i:D3}] ", new RTBTextOption { Foreground = Brushes.DarkCyan })
                    .Text($"Capacity/Scroll 테스트 항목 #{i}")
                    .SetType(6000 + i)
                    .Submit();
            }

            viewer.Writer()
                .Option(baseWhite)
                .Link("ScrollToBegin", () => viewer.ScrollToBegin())
                .Text(" | ")
                .Link("ScrollToEnd", () => viewer.ScrollToEnd())
                .Text(" | ")
                .Link("ScrollAt(5)", () => viewer.ScrollAt(5))
                .Text(" | ")
                .Link("ScrollTo(TYPE=5000)", () =>
                {
                    RTBLogItem t = viewer.Find(_i => _i.Type == 5000);
                    if (t != null) viewer.ScrollTo(t);
                })
                .Submit();

            // ================================================================================
            // [테스트 13] ContextMenu – XAML에서 포함된 메뉴 확인 (우클릭)
            // 코드로 ContextMenu 추가 (XAML 방식도 동일하게 동작)
            // ================================================================================
            viewer.Writer()
                .Option(baseWhite)
                .Text("===== [테스트 13] ContextMenu (우클릭으로 확인) =====")
                .Submit();

            ContextMenu ctxMenu = new ContextMenu();

            MenuItem miCopy = new MenuItem { Header = "선택 복사" };
            miCopy.Click += (_, _) => viewer.CopySelection();

            MenuItem miCopyAll = new MenuItem { Header = "전체 복사" };
            miCopyAll.Click += (_, _) => viewer.CopyAll();

            MenuItem miSelectAll = new MenuItem { Header = "전체 선택" };
            miSelectAll.Click += (_, _) => viewer.SelectAll();

            MenuItem miClear = new MenuItem { Header = "지우기" };
            miClear.Click += (_, _) =>
            {
                viewer.Clear();
                MessageBox.Show("Clear() 완료!", "테스트 13");
            };

            ctxMenu.Items.Add(miCopy);
            ctxMenu.Items.Add(miCopyAll);
            ctxMenu.Items.Add(new Separator());
            ctxMenu.Items.Add(miSelectAll);
            ctxMenu.Items.Add(new Separator());
            ctxMenu.Items.Add(miClear);

            viewer.LogContextMenu = ctxMenu;

            viewer.Writer()
                .Option(baseWhite)
                .Text("뷰어 위에서 우클릭하면 LogContextMenu가 표시됩니다.")
                .Submit();

            // 완료 - 첫 번째 항목으로 스크롤
            viewer.ScrollToBegin();
        }
    }
}
