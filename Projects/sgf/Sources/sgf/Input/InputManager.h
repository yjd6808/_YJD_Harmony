/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:20:00 AM
 * =====================
 * 키보드/마우스 입력 관리자
 *
 * [입력 처리의 두 가지 방식]
 * 1. 폴링(Polling): 매 프레임 "지금 키가 눌려있나?"를 직접 묻는다.
 * 이동 같은 지속 동작에 적합. (예: IsKeyDown(VK_UP) 동안 계속 전진)
 * 2. 이벤트(Event): 키가 눌리는 "순간"에 콜백을 받는다.
 * 점프/공격 같은 1회성 동작에 적합. (Cocos2d-x의 EventListener와 같은 역할)
 * 이 클래스는 둘 다 지원한다.
 *
 * [동작 원리]
 * Window의 WndProc이 받은 입력 메시지를 HandleMessage로 넘겨받아
 * 키 상태 배열을 갱신한다. 매 프레임 끝에 NextFrame으로 "이전 프레임 상태"를
 * 복사해두면 "이번 프레임에 막 눌렸는지(IsKeyPressed)"를 판별할 수 있다.
 */

#pragma once

#include "jc/Math.h"

NS_SGF_BEGIN

// 마우스 버튼 번호
enum class MouseButton
{
	Left = 0,		// 왼쪽 버튼
	Right = 1,		// 오른쪽 버튼
	Middle = 2,		// 휠 버튼
	Max = 3,
};

class InputManager
{
public:
	// 키 이벤트 콜백 타입 (인자: 가상 키코드)
	using KeyEvent = jc::Event<_s32>;
	// 마우스 버튼 이벤트 콜백 타입 (인자: 버튼, x, y)
	using MouseButtonEvent = jc::Event<MouseButton, _s32, _s32>;
	// 마우스 이동 이벤트 콜백 타입 (인자: x, y)
	using MouseMoveEvent = jc::Event<_s32, _s32>;
	// 마우스 휠 이벤트 콜백 타입 (인자: 휠 랍크, +위로/-아래로)
	using MouseWheelEvent = jc::Event<_s32>;

public:
	InputManager();
	~InputManager();

	// Window::WndProc이 호출한다. 입력 메시지를 해석해 상태/이벤트를 갱신한다.
	// @return 처리한 메시지면 true
	bool HandleMessage(UINT _msg, WPARAM _wParam, LPARAM _lParam);

	// 매 프레임 끝에 호출. 현재 상태를 "이전 프레임 상태"로 복사한다.
	void NextFrame();

	// === 폴링 API (키보드) ===

	// 지금 키가 눌려있는가? (누르고 있는 내내 true)
	bool IsKeyDown(_s32 _vkCode) const;

	// 이번 프레임에 막 눌렸는가? (누른 순간 한 프레임만 true)
	bool IsKeyPressed(_s32 _vkCode) const;

	// 이번 프레임에 막 떼졌는가? (떼는 순간 한 프레임만 true)
	bool IsKeyReleased(_s32 _vkCode) const;

	// === 폴링 API (마우스) ===

	// 지금 버튼이 눌려있는가?
	bool IsMouseDown(MouseButton _button) const;

	// 이번 프레임에 막 클릭되었는가?
	bool IsMousePressed(MouseButton _button) const;

	// 이번 프레임에 막 떼졌는가?
	bool IsMouseReleased(MouseButton _button) const;

	// 마우스 커서 위치 (클라이언트 영역 기준 픽셀 좌표, 좌상단 원점)
	_s32 MouseX() const { return mouseX_; }
	_s32 MouseY() const { return mouseY_; }

	// 이번 프레임의 휠 이동량 (+위로 / -아래로 / 0이면 안 굴림)
	_s32 WheelDelta() const { return wheelDelta_; }

	// === 이벤트 API ===
	// 사용 예: Input()->onKeyPressed.Register(1, [](int vk) { ... });

	KeyEvent onKeyPressed;				// 키가 눌린 순간
	KeyEvent onKeyReleased;				// 키가 떨어진 순간
	MouseButtonEvent onMousePressed;	// 버튼이 눌린 순간
	MouseButtonEvent onMouseReleased;	// 버튼이 떨어진 순간
	MouseMoveEvent onMouseMoved;		// 커서가 움직인 순간
	MouseWheelEvent onMouseWheel;		// 휠이 굴러간 순간

private:
	static const _s32 KEY_COUNT = 256;				// 가상 키코드 범위 (0~255)

	bool keyDown_[KEY_COUNT];					// 현재 프레임 키 상태
	bool prevKeyDown_[KEY_COUNT];				// 이전 프레임 키 상태
	bool mouseDown_[_s32(MouseButton::Max)];		// 현재 프레임 버튼 상태
	bool prevMouseDown_[_s32(MouseButton::Max)];	// 이전 프레임 버튼 상태
	_s32 mouseX_;									// 커서 X (클라이언트 좌표)
	_s32 mouseY_;									// 커서 Y (클라이언트 좌표)
	_s32 wheelDelta_;								// 이번 프레임 휠 이동량
};

NS_SGF_END
