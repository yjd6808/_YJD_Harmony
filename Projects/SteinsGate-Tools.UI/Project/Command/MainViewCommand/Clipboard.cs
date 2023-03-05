/*
 * 작성자: 윤정도
 * 생성일: 3/5/2023 6:19:58 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using SGToolsUI.Model;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command.MainViewCommand
{

    public enum ClipboardOperation
    {
        Cut,
        Copy,
        Paste
    }

    public class Clipboard : MainCommandAbstract
    {
        private List<SGUIElement> _copy = new();

        public Clipboard(MainViewModel viewModel, string description) : base(viewModel, description)
        {
        }

        public override void Execute(object? parameter)
        {
            bool parsed = Enum.TryParse((string)parameter, out ClipboardOperation operation);

            if (!parsed)
            {
                MessageBox.Show("클립보드 오퍼레이션 커맨드 실행실패\n오퍼레이션 정보 파싱에 실패했습니다.");
                return;
            }
        }
    }
}
