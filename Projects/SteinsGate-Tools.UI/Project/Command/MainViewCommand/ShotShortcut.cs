/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 6:26:46 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
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
using SGToolsCommon.Extension;
using SGToolsCommon.Resource;
using SGToolsUI.ViewModel;
using Path = System.IO.Path;

namespace SGToolsUI.Command.MainViewCommand
{
    public class ShowShortcut : MainCommandAbstract
    {
        public ShowShortcut(MainViewModel viewModel) 
            : base(viewModel, "사용가능한 단축키 정보를 보여준다.")
        {
        }

        public override void Execute(object? parameter)
        {
            ViewModel.LogBox.AddLog("[글로벌 단축키 목록]");
            ViewModel.LogBox.AddLog("Ctrl + S: 저장");
            ViewModel.LogBox.AddLog("Ctrl + L: 로그뷰");
            
            ViewModel.LogBox.AddLog("F6: 파일 다시 로딩");
            ViewModel.LogBox.AddLog("F7: 백업 텍스트 박스 포커싱");
            ViewModel.LogBox.AddLog("F8: 백업 폴더 열기");
            ViewModel.LogBox.AddLog("ESC: 선택, 픽된 엘리먼트 모두 해제");
            ViewModel.LogBox.AddLog("", null, IconCommonType.Transparent);
            ViewModel.LogBox.AddLog("[트리뷰 단축키 목록]");
            ViewModel.LogBox.AddLog("Delete: 선택한 엘리먼트 삭제");
            ViewModel.LogBox.AddLog("Ctrl + X, C, V: 클립보드 기능");
            ViewModel.LogBox.AddLog("Shift + 클릭: 범위 다중 선택");
            ViewModel.LogBox.AddLog("Ctrl + 클릭: 단일 다중 선택");
            ViewModel.LogBox.AddLog("Shift + 방향키: 다중 선택");
            ViewModel.LogBox.AddLog("방향키: 단일 선택");
            ViewModel.LogBox.AddLog("", null, IconCommonType.Transparent);
            ViewModel.LogBox.AddLog("[캔버스 단축키 목록]");
            ViewModel.LogBox.AddLog("Z: 셀렉션 숨기기|보이기");
            ViewModel.LogBox.AddLog("X: 이벤트 모드, 캔버스상 선택, 움직임등이 모두 차단되고 버튼의 클릭등 컨트롤 이벤트를 시각적으로 확인할 수 있도록 하는 모드이다.");
            ViewModel.LogBox.AddLog("C: 이벤트 모드에서만 사용가능, 모든 엘리먼트를 Disabled 상태로 바꾼다. (다시 돌리기 위해서는 이벤트 모드 껏다 키면됨)");
            ViewModel.LogBox.AddLog("드래그: 범위 다중 선택");
            ViewModel.LogBox.AddLog("스페이스바 + 드래그: 클릭시 선택안됨, 어디서든 드래그 가능");
            ViewModel.LogBox.AddLog("클릭: 제일 위 엘리먼트 선택");
            ViewModel.LogBox.AddLog("Shift + 클릭: 중복 선택");
            ViewModel.LogBox.AddLog("Ctrl + 클릭: 중복 선택(재중복 선택취소)");
            ViewModel.LogBox.AddLog("Alt + 클릭: 겹친 순서대로 클릭할때마다 변경 선택");
            ViewModel.LogBox.AddLog("Alt + Shift + 클릭: 겹친 순서대로 클릭시 중복선택");
            ViewModel.LogBox.AddLog("", null, IconCommonType.Transparent);

            if (!ViewModel.LogView.IsVisible)
                ViewModel.LogView.Show();
            else
            {
                ViewModel.LogView.Activate();
                ViewModel.LogView.Focus();
            }
        }
    }
}
