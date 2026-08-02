/*
 * 작성자: 윤정도
 * =====================
 * sgui - 입력 디스패처 (히트테스트 기반 라우팅 + 마우스 캡처 + 포커스 관리)
 * 기존 "RootGroup에 전달 후 재귀 분배" 방식을 대체한다.
 * - 마우스: 히트테스트로 최종 대상을 찾고 그 대상에서 RaiseEvent (터널링->버블링)
 * - 캡처 중이면 히트테스트 없이 캡처 요소로 직행
 * - Enter/Leave: hover 체인 diff로 Direct 전달
 * - 키보드: 포커스 요소 -> 없으면 최상단 윈도우
 * - 모달 윈도우가 있으면 그 아래 윈도우는 입력 차단
 */

#pragma once

#include "sgcl/Game/UI/Core/UIEvent.h"

#include <vector>

namespace sgui
{

class UIElement;
class UIHost;

class InputDispatcher
{
public:
	static InputDispatcher* Get();

	void SetHost(UIHost* _pHost) { pHost_ = _pHost; }
	UIHost* GetHost() const { return pHost_; }

	// 코코스 이벤트 진입점 (UIHost가 호출)
	bool DispatchMouseDown(const cc::vec2& _worldPos);
	bool DispatchMouseUp(const cc::vec2& _worldPos);
	bool DispatchMouseMove(const cc::vec2& _worldPos);
	bool DispatchMouseWheel(const cc::vec2& _worldPos, float _deltaX, float _deltaY);
	bool DispatchKeyDown(cc::EventKeyboard::KeyCode _keyCode);
	bool DispatchKeyUp(cc::EventKeyboard::KeyCode _keyCode);

	// 캡처
	void Capture(UIElement* _pElement);
	void ReleaseCapture(UIElement* _pElement);
	bool IsCaptured(const UIElement* _pElement) const;

	// 포커스
	void SetFocus(UIElement* _pElement);
	UIElement* GetFocused() const;

	// 요소 파괴 통지 (UIElement 소멸자에서 호출)
	void OnElementDestroyed(_u32 _elementId);

	void Clear();

private:
	UIElement* HitTestHost(const cc::vec2& _worldPos) const;
	void UpdateHoverChain(UIElement* _pHit);

	UIHost* pHost_ = nullptr;

	std::vector<_u32> hoverChain_;
	_u32 captureId_ = 0;
	_u32 focusId_ = 0;
};

} // namespace sgui
