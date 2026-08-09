/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:22:00 AM
 * =====================
 * 키보드/마우스 입력 관리자 구현부
 */

#include "Core.h"
#include "sgf/Input/InputManager.h"

#include <cstring>

NS_SGF_BEGIN

// 생성자: 모든 상태를 "안 눌림"으로 초기화한다.
InputManager::InputManager()
	: m_MouseX(0)
	, m_MouseY(0)
	, m_WheelDelta(0)
{
	memset(m_bKeyDown, 0, sizeof(m_bKeyDown));
	memset(m_bPrevKeyDown, 0, sizeof(m_bPrevKeyDown));
	memset(m_bMouseDown, 0, sizeof(m_bMouseDown));
	memset(m_bPrevMouseDown, 0, sizeof(m_bPrevMouseDown));
}

// 소멸자: 등록된 이벤트는 jc::Event 소멸자가 알아서 정리한다.
InputManager::~InputManager()
{
}

// 윈도우 메시지 -> 입력 상태/이벤트 반영
// [메시지 해설]
//  WM_KEYDOWN   : 키가 눌림. 즐, 누르고 있으면 자동반복(repeat)으로 계속 온다.
//                 _lParam의 30번 비트가 1이면 반복 입력이므로 첫 입력만 이벤트로 보낸다.
//  WM_MOUSEMOVE : 커서 이동. _lParam에 클라이언트 좌표가 담겨 있다.
//  WM_MOUSEWHEEL: _wParam 상위 16비트에 휠 이동량(보통 ±120)이 담겨 있다.
bool InputManager::HandleMessage(UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	switch (_msg)
	{
	// === 키보드 ===
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:		// ALT 조합 키도 동일하게 처리
	{
		const int vkCode = int(_wParam);
		if (vkCode >= 0 && vkCode < KeyCount_v)
		{
			// 30번 비트 == 0 일 때만 "처음 눌린 것"
			const bool bFirstPress = (_lParam & (1 << 30)) == 0;
			m_bKeyDown[vkCode] = true;
			if (bFirstPress)
			{
				onKeyPressed.Invoke(vkCode);
			}
		}
		return true;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		const int vkCode = int(_wParam);
		if (vkCode >= 0 && vkCode < KeyCount_v)
		{
			m_bKeyDown[vkCode] = false;
			onKeyReleased.Invoke(vkCode);
		}
		return true;
	}

	// === 마우스 버튼 ===
	// GET_X_LPARAM/GET_Y_LPARAM은 windowsx.h의 매크로로,
	// _lParam에 묶인 16비트씩의 x/y를 부호 있게 꺼낸다.
	case WM_LBUTTONDOWN:
		m_bMouseDown[int(MouseButton::Left)] = true;
		onMousePressed.Invoke(MouseButton::Left, GET_X_LPARAM(_lParam), GET_Y_LPARAM(_lParam));
		return true;
	case WM_LBUTTONUP:
		m_bMouseDown[int(MouseButton::Left)] = false;
		onMouseReleased.Invoke(MouseButton::Left, GET_X_LPARAM(_lParam), GET_Y_LPARAM(_lParam));
		return true;
	case WM_RBUTTONDOWN:
		m_bMouseDown[int(MouseButton::Right)] = true;
		onMousePressed.Invoke(MouseButton::Right, GET_X_LPARAM(_lParam), GET_Y_LPARAM(_lParam));
		return true;
	case WM_RBUTTONUP:
		m_bMouseDown[int(MouseButton::Right)] = false;
		onMouseReleased.Invoke(MouseButton::Right, GET_X_LPARAM(_lParam), GET_Y_LPARAM(_lParam));
		return true;
	case WM_MBUTTONDOWN:
		m_bMouseDown[int(MouseButton::Middle)] = true;
		onMousePressed.Invoke(MouseButton::Middle, GET_X_LPARAM(_lParam), GET_Y_LPARAM(_lParam));
		return true;
	case WM_MBUTTONUP:
		m_bMouseDown[int(MouseButton::Middle)] = false;
		onMouseReleased.Invoke(MouseButton::Middle, GET_X_LPARAM(_lParam), GET_Y_LPARAM(_lParam));
		return true;

	// === 마우스 이동/휠 ===
	case WM_MOUSEMOVE:
		m_MouseX = GET_X_LPARAM(_lParam);
		m_MouseY = GET_Y_LPARAM(_lParam);
		onMouseMoved.Invoke(m_MouseX, m_MouseY);
		return true;
	case WM_MOUSEWHEEL:
		// GET_WHEEL_DELTA_WPARAM: 휠 한 랍크 = 120. 랍크 단위로 변환해서 전달한다.
		// 한 프레임에 여러 번 굴리면 델타가 누적되어야 정확하다. (고해상도 휠)
		m_WheelDelta += GET_WHEEL_DELTA_WPARAM(_wParam) / WHEEL_DELTA;
		onMouseWheel.Invoke(m_WheelDelta);
		return true;

	default:
		return false;
	}
}

// 매 프레임 끝에 호출: 현재 상태를 이전 프레임 상태로 복사
void InputManager::NextFrame()
{
	memcpy(m_bPrevKeyDown, m_bKeyDown, sizeof(m_bKeyDown));
	memcpy(m_bPrevMouseDown, m_bMouseDown, sizeof(m_bMouseDown));
	m_WheelDelta = 0;	// 휠은 "이번 프레임 이동량"이므로 매 프레임 리셋
}

// 지금 키가 눌려있는가?
bool InputManager::IsKeyDown(int _vkCode) const
{
	if (_vkCode < 0 || _vkCode >= KeyCount_v) { return false; }
	return m_bKeyDown[_vkCode];
}

// 이번 프레임에 막 눌렸는가? (현재는 눌림 + 이전엔 안 눌림)
bool InputManager::IsKeyPressed(int _vkCode) const
{
	if (_vkCode < 0 || _vkCode >= KeyCount_v) { return false; }
	return m_bKeyDown[_vkCode] && !m_bPrevKeyDown[_vkCode];
}

// 이번 프레임에 막 떼졌는가? (현재는 안 눌림 + 이전엔 눌림)
bool InputManager::IsKeyReleased(int _vkCode) const
{
	if (_vkCode < 0 || _vkCode >= KeyCount_v) { return false; }
	return !m_bKeyDown[_vkCode] && m_bPrevKeyDown[_vkCode];
}

// 지금 버튼이 눌려있는가?
bool InputManager::IsMouseDown(MouseButton _button) const
{
	if (int(_button) < 0 || int(_button) >= int(MouseButton::Max)) { return false; }
	return m_bMouseDown[int(_button)];
}

// 이번 프레임에 막 클릭되었는가?
bool InputManager::IsMousePressed(MouseButton _button) const
{
	if (int(_button) < 0 || int(_button) >= int(MouseButton::Max)) { return false; }
	return m_bMouseDown[int(_button)] && !m_bPrevMouseDown[int(_button)];
}

// 이번 프레임에 막 떼졌는가?
bool InputManager::IsMouseReleased(MouseButton _button) const
{
	if (int(_button) < 0 || int(_button) >= int(MouseButton::Max)) { return false; }
	return !m_bMouseDown[int(_button)] && m_bPrevMouseDown[int(_button)];
}

NS_SGF_END
