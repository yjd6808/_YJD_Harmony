/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:20:00 AM
 * =====================
 * 02. 윈도우와 메시지 루프 (Window & Message Loop)
 *
 * [이 튜토리얼에서 배우는 것]
 *  1. 윈도우는 어떻게 만들어지나? (클래스 등록 -> CreateWindowEx -> HWND)
 *  2. 메시지 루프란? (OS가 보낸 이벤트를 매 프레임 꺼내 처리하는 반복문)
 *  3. 게임 루프의 기본 골격: "메시지 처리 -> 입력 확인 -> 갱신 -> (그리기)"
 *
 * [메시지 루프가 왜 필요한가?]
 *  Windows는 키 입력/마우스/창 크기 변경 같은 사건을 "메시지 큐"에 쌓는다.
 *  프로그램이 이 큐를 꺼내서 처리하지 않으면 창이 "응답 없음" 상태가 된다.
 *  그래서 게임은 매 프레임 PumpMessage()로 큐를 비우며 동작한다.
 *
 * [조작법]
 *  - 아무 키나 누르면 콘솔에 키 이름이 출력된다.
 *  - ESC 또는 창 닫기로 종료한다.
 */

#include "Core.h"
#include "sgfr/Tutorial/02_WindowAndMessageLoop/02_WindowAndMessageLoop_Main.h"
#include "sgfr/Tutorial/02_WindowAndMessageLoop/02_WindowAndMessageLoop_Function.h"

using namespace sgf;

// 윈도우 생성과 메시지 루프 튜토리얼을 실행한다. (ESC 또는 창 닫기로 종료)
void WindowAndMessageLoop_Main()
{
	// 1. 윈도우 생성
	//    Window::Create 내부에서 일어나는 일:
	//    (1) RegisterClassEx : "이런 종류의 창을 만들겠다"고 OS에 양식 등록
	//    (2) AdjustWindowRect: 요청한 800x600이 '그림 영역' 크기가 되도록 테두리 포함 크기 계산
	//    (3) CreateWindowEx : 실제 창 생성, 식별자(HWND) 발급
	Window window;
	if (!window.Create(L"02. 윈도우와 메시지 루프 (ESC로 종료)", 800, 600))
	{
		jc::Console::WriteLine("윈도우 생성 실패!");
		return;
	}

	// 2. 입력 관리자 연결
	//    Window가 받은 WM_KEYDOWN 등의 메시지를 InputManager에게 전달하게 한다.
	InputManager input;
	window.ConnectInput(&input);

	// 3. 이벤트 방식 입력 처리 데모
	//    "키가 눌리는 순간" 콜백이 호출된다. (폴링 방식과 비교해보기)
	input.onKeyPressed.Register(1, [](_s32 _vkCode)
	{
		jc::Console::Write("[이벤트] 키 누름: %s (코드: 0x%02X)\n", VkCodeToName(_vkCode), _vkCode);
	});

	input.onMousePressed.Register(1, [](MouseButton _button, _s32 _x, _s32 _y)
	{
		const char* szName = (_button == MouseButton::Left) ? "왼쪽"
			: (_button == MouseButton::Right) ? "오른쪽" : "가운데";
		jc::Console::Write("[이벤트] 마우스 %s 버튼 클릭: (%d, %d)\n", szName, _x, _y);
	});

	jc::Console::WriteLine("윈도우가 생성되었습니다. 아무 키나 마우스를 눌러보세요. (ESC로 종료)");

	// 4. 메시지 루프 = 게임 루프의 뼈대
	//    PumpMessage()가 false를 반환하면 WM_QUIT이 도착한 것이다. (창 닫힘)
	while (window.PumpMessage())
	{
		// 폴링 방식 입력 처리: "지금 이 순간 키가 눌려있나?"를 직접 묻는다.
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			jc::Console::WriteLine("ESC 입력 -> 종료");
			break;
		}

		// 입력 상태를 다음 프레임으로 넘긴다.
		// (이걸 호출해야 IsKeyPressed가 "눌린 첫 프레임"을 구분할 수 있다)
		input.NextFrame();

		// 아직 그리기가 없으므로 CPU를 양보한다.
		// (이게 없으면 빈 루프가 CPU 한 코어를 100% 점유한다)
		Sleep(1);
	}

	// 5. 정리: 창을 파괴한다.
	window.Destroy();

	// 이벤트 등록 해제 (지역 변수 input이 사라지므로 관례적 의미지만, 좋은 습관이다)
	input.onKeyPressed.Unregister(1);
	input.onMousePressed.Unregister(1);
}
