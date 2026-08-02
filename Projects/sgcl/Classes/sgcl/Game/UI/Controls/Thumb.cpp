/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/Thumb.h"

namespace sgui
{

Thumb* Thumb::Create()
{
	Thumb* pThumb = dbg_new Thumb;
	pThumb->autorelease();
	return pThumb;
}

void Thumb::OnEventRaised(int _id, UIEvent* _pEvent)
{
	Control::OnEventRaised(_id, _pEvent);

	switch (_pEvent->type_)
	{
	case ueMouseDown:
	{
		if (_pEvent->phase_ != UIRoutingPhase::Bubble || !IsEffectivelyEnabled())
			break;

		auto* pMouseEvent = _pEvent->Cast<UIEventMouse>();
		isDragging_ = true;
		isPressed_ = true;
		dragStartWorld_ = pMouseEvent->screenPos_;
		lastWorld_ = pMouseEvent->screenPos_;
		CaptureMouse();
		UpdateVisualState();
		_pEvent->handled_ = true;

		RaiseDragEvent(ueDragStarted, pMouseEvent->screenPos_);
		break;
	}
	case ueMouseMove:
	{
		if (!isDragging_ || _pEvent->phase_ != UIRoutingPhase::Bubble)
			break;

		auto* pMouseEvent = _pEvent->Cast<UIEventMouse>();
		_pEvent->handled_ = true;

		RaiseDragEvent(ueDragDelta, pMouseEvent->screenPos_);
		lastWorld_ = pMouseEvent->screenPos_;
		break;
	}
	case ueMouseUp:
	{
		if (!isDragging_ || _pEvent->phase_ != UIRoutingPhase::Bubble)
			break;

		auto* pMouseEvent = _pEvent->Cast<UIEventMouse>();
		isDragging_ = false;
		isPressed_ = false;
		ReleaseMouseCapture();
		UpdateVisualState();
		_pEvent->handled_ = true;

		RaiseDragEvent(ueDragCompleted, pMouseEvent->screenPos_);
		break;
	}
	default:
		break;
	}
}

void Thumb::RaiseDragEvent(UIEventType _type, const cc::vec2& _worldPos)
{
	UIEventDrag event;
	event.type_ = _type;
	event.screenPos_ = _worldPos;

	// 코코스 월드 좌표는 y-up이므로 sgui 규약(y-down)으로 변환해 전달한다.
	event.delta_ = cc::vec2(_worldPos.x - lastWorld_.x, lastWorld_.y - _worldPos.y);
	event.total_ = cc::vec2(_worldPos.x - dragStartWorld_.x, dragStartWorld_.y - _worldPos.y);
	RaiseEvent(&event);
}

} // namespace sgui
