/*
 * 작성자: 윤정도
 * =====================
 * sgui - 라우티드 이벤트 (터널링/버블링/다이렉트)
 * 모든 이벤트는 UIEventType enum으로 정의되고, 단일 진입점 OnEventRaised(int _id, UIEvent* _pEvent)로 수신한다.
 */

#pragma once

#include "sgcl/Game/UI/Core/UITypes.h"

#include <string>

namespace sgui
{

class UIElement;

enum UIEventType : int
{
	ueNone = 0,

	// 프리뷰(터널링) 이벤트: 루트 -> 소스 방향으로 전달
	uePreviewMouseDown,
	uePreviewMouseUp,
	uePreviewMouseMove,
	uePreviewMouseWheel,
	uePreviewKeyDown,
	uePreviewKeyUp,

	// 버블링 이벤트: 소스 -> 루트 방향으로 전달
	ueMouseDown,
	ueMouseUp,
	ueMouseMove,
	ueMouseWheel,
	ueKeyDown,
	ueKeyUp,

	// 다이렉트 이벤트: 대상 요소에만 전달
	ueMouseEnter,
	ueMouseLeave,
	ueGotFocus,
	ueLostFocus,

	// 컨트롤 이벤트 (버블링)
	ueClick,
	ueCheckedChanged,
	ueValueChanged,
	ueScrollChanged,
	ueTextChanged,
	ueTextSubmit,
	ueDragStarted,
	ueDragDelta,
	ueDragCompleted,

	// 윈도우 이벤트 (다이렉트)
	ueWindowClosed,

	ueMax
};

enum class UIRoutingStrategy : uint8_t { Direct, Bubble, Tunnel };
enum class UIRoutingPhase : uint8_t { Direct, Tunnel, Bubble };
enum class UIMouseButton : uint8_t { None, Left, Right, Middle };

struct UIEventTypeMeta
{
	UIRoutingStrategy strategy_;
	UIEventType previewPair_;	// 버블링 이벤트에 대응하는 프리뷰(터널링) 이벤트. 없으면 ueNone
	const char* name_;
};

// UIEventType 순서와 1:1로 일치하는 메타 테이블
extern const UIEventTypeMeta g_uiEventTypeMeta[ueMax];

struct UIEvent
{
	virtual ~UIEvent() = default;

	UIEventType type_ = ueNone;
	UIRoutingPhase phase_ = UIRoutingPhase::Direct;
	UIElement* pSource_ = nullptr;			// 현재 이벤트를 전달받고 있는 요소
	UIElement* pOriginalSource_ = nullptr;	// 이벤트를 최초 발생시킨 요소
	_u32 originalSourceId_ = 0;
	bool handled_ = false;

	template <typename TEvent>
	TEvent* Cast() { return static_cast<TEvent*>(this); }
};

struct UIEventMouse : UIEvent
{
	cc::vec2 screenPos_;		// 코코스 월드(GL) 좌표
	UIMouseButton button_ = UIMouseButton::Left;

	// _pRelativeTo 요소 기준 로컬 좌표(y-down)로 변환해서 반환한다.
	cc::vec2 GetPosition(const UIElement* _pRelativeTo) const;
};

struct UIEventScroll : UIEventMouse
{
	float deltaX_ = 0.0f;
	float deltaY_ = 0.0f;
};

struct UIEventKeyboard : UIEvent
{
	cc::EventKeyboard::KeyCode keyCode_ = cc::EventKeyboard::KeyCode::KEY_NONE;
};

struct UIEventText : UIEvent
{
	std::string text_;
};

struct UIEventFocus : UIEvent
{
	UIElement* pOpposite_ = nullptr;	// 포커스를 뺏긴/뺏은 상대 요소
};

struct UIEventChecked : UIEvent
{
	bool checked_ = false;
};

struct UIEventRange : UIEvent
{
	float oldValue_ = 0.0f;
	float newValue_ = 0.0f;
};

struct UIEventScrollChanged : UIEvent
{
	int rowPos_ = 0;
};

struct UIEventDrag : UIEventMouse
{
	cc::vec2 delta_;	// 직전 이벤트 대비 이동량 (y-down)
	cc::vec2 total_;	// 드래그 시작점 대비 총 이동량 (y-down)
};

} // namespace sgui
