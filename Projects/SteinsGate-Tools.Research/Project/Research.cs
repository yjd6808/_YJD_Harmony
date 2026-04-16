/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 12:34:42 PM
 *
 */

using System.Windows;
using System.Windows.Input;

namespace SGToolsResearch
{
    public abstract class Research
    {
        public abstract string Name { get; }
        public abstract void Run();
        public abstract Window CreateWindow();

        //////////////////////////////////////////////////////////////////////////////////
        public Window OpenWindow()
        {
            Window window = CreateWindow();
            window.PreviewKeyDown += (_s, _e) =>
            {
                if (_e.Key == Key.Escape)
                {
                    window.Close();
                    _e.Handled = true;
                }
            };
            return window;
        }
    }
}