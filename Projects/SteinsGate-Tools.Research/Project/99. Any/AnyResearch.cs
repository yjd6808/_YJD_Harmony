/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 12:34:42 PM
 *
 */

using System.Windows;

namespace SGToolsResearch
{
    public class AnyResearch : Research
    {
        public override string Name => "Any";

        //////////////////////////////////////////////////////////////////////////////////
        public override Window CreateWindow()
        {
            return new AnyResearchWindow();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Run()
        {
            // 임시로 테스트 코드를 작성하는 용도
        }
    }
}