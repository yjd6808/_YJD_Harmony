/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Core/InputDispatcher.h"
#include "sgcl/Game/UI/Core/UIElement.h"
#include "sgcl/Game/UI/Core/ElementRegistry.h"
#include "sgcl/Game/UI/Controls/Window.h"
#include "sgcl/Game/UI/Host/UIHost.h"

#include <algorithm>

namespace sgui
{

InputDispatcher* InputDispatcher::Get()
{
	static InputDispatcher s_dispatcher;
	return &s_dispatcher;
}

bool InputDispatcher::DispatchMouseDown(const cc::vec2& _worldPos)
{
	UIElement* pTarget = captureId_ != 0 ? ElementRegistry::Get()->Find(captureId_) : HitTestHost(_worldPos);

	if (pTarget == nullptr)
	{
		// 빈 영역 클릭 시 포커스 해제
		SetFocus(nullptr);
		return false;
	}

	// 포커스 이동: 히트 체인에서 가장 가까운 focusable 요소를 찾는다 (Control이 Focus 처리)
	UIEventMouse event;
	event.type_ = ueMouseDown;
	event.screenPos_ = _worldPos;
	event.button_ = UIMouseButton::Left;
	pTarget->RaiseEvent(&event);
	return event.handled_ || pTarget->GetWindow() != nullptr;
}

bool InputDispatcher::DispatchMouseUp(const cc::vec2& _worldPos)
{
	UIElement* pTarget = captureId_ != 0 ? ElementRegistry::Get()->Find(captureId_) : HitTestHost(_worldPos);

	if (pTarget == nullptr)
	{
		return false;
	}

	UIEventMouse event;
	event.type_ = ueMouseUp;
	event.screenPos_ = _worldPos;
	event.button_ = UIMouseButton::Left;
	pTarget->RaiseEvent(&event);
	return event.handled_ || pTarget->GetWindow() != nullptr;
}

bool InputDispatcher::DispatchMouseMove(const cc::vec2& _worldPos)
{
	UIElement* pCaptured = captureId_ != 0 ? ElementRegistry::Get()->Find(captureId_) : nullptr;
	UIElement* pHit = HitTestHost(_worldPos);

	UpdateHoverChain(pCaptured ? pCaptured : pHit);

	UIElement* pTarget = pCaptured ? pCaptured : pHit;

	if (pTarget == nullptr)
	{
		return false;
	}

	UIEventMouse event;
	event.type_ = ueMouseMove;
	event.screenPos_ = _worldPos;
	event.button_ = UIMouseButton::None;
	pTarget->RaiseEvent(&event);
	return event.handled_ || pHit != nullptr;
}

bool InputDispatcher::DispatchMouseWheel(const cc::vec2& _worldPos, float _deltaX, float _deltaY)
{
	UIElement* pTarget = captureId_ != 0 ? ElementRegistry::Get()->Find(captureId_) : HitTestHost(_worldPos);

	if (pTarget == nullptr)
	{
		return false;
	}

	UIEventScroll event;
	event.type_ = ueMouseWheel;
	event.screenPos_ = _worldPos;
	event.button_ = UIMouseButton::None;
	event.deltaX_ = _deltaX;
	event.deltaY_ = _deltaY;
	pTarget->RaiseEvent(&event);
	return true;
}

bool InputDispatcher::DispatchKeyDown(cc::EventKeyboard::KeyCode _keyCode)
{
	UIElement* pTarget = GetFocused();

	if (pTarget == nullptr && pHost_)
	{
		pTarget = pHost_->GetTopmostWindow();
	}

	if (pTarget == nullptr)
	{
		return false;
	}

	UIEventKeyboard event;
	event.type_ = ueKeyDown;
	event.keyCode_ = _keyCode;
	pTarget->RaiseEvent(&event);
	return event.handled_;
}

bool InputDispatcher::DispatchKeyUp(cc::EventKeyboard::KeyCode _keyCode)
{
	UIElement* pTarget = GetFocused();

	if (pTarget == nullptr && pHost_)
	{
		pTarget = pHost_->GetTopmostWindow();
	}

	if (pTarget == nullptr)
	{
		return false;
	}

	UIEventKeyboard event;
	event.type_ = ueKeyUp;
	event.keyCode_ = _keyCode;
	pTarget->RaiseEvent(&event);
	return event.handled_;
}

void InputDispatcher::Capture(UIElement* _pElement)
{
	captureId_ = _pElement ? _pElement->GetElementId() : 0;
}

void InputDispatcher::ReleaseCapture(UIElement* _pElement)
{
	if (_pElement == nullptr || captureId_ == _pElement->GetElementId())
	{
		captureId_ = 0;
	}
}

bool InputDispatcher::IsCaptured(const UIElement* _pElement) const
{
	return _pElement && captureId_ == _pElement->GetElementId();
}

void InputDispatcher::SetFocus(UIElement* _pElement)
{
	UIElement* pOld = GetFocused();

	if (pOld == _pElement)
	{
		return;
	}

	focusId_ = _pElement ? _pElement->GetElementId() : 0;

	if (pOld)
	{
		UIEventFocus event;
		event.type_ = ueLostFocus;
		event.pOpposite_ = _pElement;
		pOld->RaiseEvent(&event);
		pOld->UpdateVisualState();
	}

	if (_pElement)
	{
		UIEventFocus event;
		event.type_ = ueGotFocus;
		event.pOpposite_ = pOld;
		_pElement->RaiseEvent(&event);
		_pElement->UpdateVisualState();
	}
}

UIElement* InputDispatcher::GetFocused() const
{
	return focusId_ != 0 ? ElementRegistry::Get()->Find(focusId_) : nullptr;
}

void InputDispatcher::OnElementDestroyed(_u32 _elementId)
{
	if (captureId_ == _elementId)
	{
		captureId_ = 0;
	}

	if (focusId_ == _elementId)
	{
		focusId_ = 0;
	}

	hoverChain_.erase(std::remove(hoverChain_.begin(), hoverChain_.end(), _elementId), hoverChain_.end());
}

void InputDispatcher::Clear()
{
	hoverChain_.clear();
	captureId_ = 0;
	focusId_ = 0;
}

UIElement* InputDispatcher::HitTestHost(const cc::vec2& _worldPos) const
{
	if (pHost_ == nullptr)
	{
		return nullptr;
	}

	// 상단 윈도우부터 히트테스트. 모달 윈도우를 만나면 그 아래로는 내려가지 않는다.
	for (int idx = pHost_->GetWindowCount() - 1; idx >= 0; --idx)
	{
		Window* pWindow = pHost_->GetWindowAt(idx);

		if (pWindow == nullptr)
			continue;

		if (UIElement* pHit = pWindow->HitTest(_worldPos))
		{
			return pHit;
		}

		if (pWindow->IsModal())
		{
			return pWindow;	// 모달 윈도우 밖 클릭은 모달 윈도우 자신이 흡수
		}
	}

	return nullptr;
}

void InputDispatcher::UpdateHoverChain(UIElement* _pHit)
{
	ElementRegistry* pRegistry = ElementRegistry::Get();

	// 새 hover 체인 (히트 대상부터 루트까지)
	std::vector<_u32> newChain;
	UIElement* pCur = _pHit;
	int depth = 0;

	while (pCur && depth++ < 64)
	{
		newChain.push_back(pCur->GetElementId());
		pCur = pCur->GetParentElement();
	}

	// Leave: 이전 체인에만 있는 요소
	for (size_t i = 0; i < hoverChain_.size(); ++i)
	{
		const _u32 id = hoverChain_[i];

		if (std::find(newChain.begin(), newChain.end(), id) != newChain.end())
			continue;

		if (UIElement* pElement = pRegistry->Find(id))
		{
			pElement->isMouseOver_ = false;

			UIEventMouse event;
			event.type_ = ueMouseLeave;
			pElement->RaiseEvent(&event);
			pElement->UpdateVisualState();
		}
	}

	// Enter: 새 체인에만 있는 요소
	for (size_t i = 0; i < newChain.size(); ++i)
	{
		const _u32 id = newChain[i];

		if (std::find(hoverChain_.begin(), hoverChain_.end(), id) != hoverChain_.end())
			continue;

		if (UIElement* pElement = pRegistry->Find(id))
		{
			pElement->isMouseOver_ = true;

			UIEventMouse event;
			event.type_ = ueMouseEnter;
			pElement->RaiseEvent(&event);
			pElement->UpdateVisualState();
		}
	}

	hoverChain_.swap(newChain);
}

} // namespace sgui
