/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Core/UIElement.h"
#include "sgcl/Game/UI/Core/ElementRegistry.h"
#include "sgcl/Game/UI/Core/InputDispatcher.h"
#include "sgcl/Game/UI/Controls/Window.h"

#include <algorithm>

USING_NS_CC;
USING_NS_JC;

namespace sgui
{

static constexpr int MaxRouteDepth_v = 64;

UIElement::UIElement()
{
	elementId_ = ElementRegistry::Get()->Allocate(this);
}

UIElement::~UIElement()
{
	InputDispatcher::Get()->OnElementDestroyed(elementId_);
	ElementRegistry::Get()->Release(elementId_, this);
}

Window* UIElement::GetWindow() const
{
	const UIElement* pCur = this;

	while (pCur)
	{
		const Window* pWindow = dynamic_cast<const Window*>(pCur);

		if (pWindow)
		{
			return const_cast<Window*>(pWindow);
		}

		pCur = pCur->pParentElement_;
	}

	return nullptr;
}

UIElement* UIElement::FindElementByName(const char* _name)
{
	if (_name == nullptr)
	{
		return nullptr;
	}

	if (getName() == _name)
	{
		return this;
	}

	const int childCount = GetChildElementCount();

	for (int idx = 0; idx < childCount; ++idx)
	{
		UIElement* pChild = GetChildElementAt(idx);

		if (pChild == nullptr)
		{
			continue;
		}

		if (UIElement* pFound = pChild->FindElementByName(_name))
		{
			return pFound;
		}
	}

	return nullptr;
}

void UIElement::ForEachRecursiveContainedSelf(const std::function<void(UIElement*)>& _fn)
{
	_fn(this);

	const int childCount = GetChildElementCount();

	for (int idx = 0; idx < childCount; ++idx)
	{
		UIElement* pChild = GetChildElementAt(idx);

		if (pChild)
		{
			pChild->ForEachRecursiveContainedSelf(_fn);
		}
	}
}

void UIElement::SetVisibility(Visibility _visibility)
{
	if (visibility_ == _visibility)
	{
		return;
	}

	visibility_ = _visibility;
	setVisible(visibility_ == Visibility::Visible);
	InvalidateLayout();
}

void UIElement::SetIsEnabled(bool _enabled)
{
	if (isEnabled_ == _enabled)
	{
		return;
	}

	isEnabled_ = _enabled;

	ForEachRecursiveContainedSelf([](UIElement* _pElement)
	{
		_pElement->UpdateVisualState();
	});
}

bool UIElement::IsEffectivelyEnabled() const
{
	const UIElement* pCur = this;

	while (pCur)
	{
		if (!pCur->isEnabled_)
		{
			return false;
		}

		pCur = pCur->pParentElement_;
	}

	return true;
}

UIColorF UIElement::GetEffectiveForeground() const
{
	const UIElement* pCur = this;

	while (pCur)
	{
		if (pCur->hasForeground_)
		{
			return pCur->foreground_;
		}

		pCur = pCur->pParentElement_;
	}

	return UIColorF{ 1.0f, 1.0f, 1.0f, 1.0f };
}

int UIElement::GetEffectiveFontCode() const
{
	const UIElement* pCur = this;

	while (pCur)
	{
		if (pCur->fontCode_ >= 0)
		{
			return pCur->fontCode_;
		}

		pCur = pCur->pParentElement_;
	}

	return 1;
}

float UIElement::GetEffectiveFontSize() const
{
	const UIElement* pCur = this;

	while (pCur)
	{
		if (pCur->fontSize_ > 0.0f)
		{
			return pCur->fontSize_;
		}

		pCur = pCur->pParentElement_;
	}

	return 14.0f;
}

void UIElement::Measure(const cc::size& _available)
{
	if (visibility_ == Visibility::Collapsed)
	{
		desiredSize_ = cc::size(0.0f, 0.0f);
		return;
	}

	const float marginW = margin_.Width();
	const float marginH = margin_.Height();

	cc::size constraint(
		std::max(0.0f, _available.width - marginW),
		std::max(0.0f, _available.height - marginH));

	if (!IsAutoSize(width_))
	{
		constraint.width = ClampF(width_, minWidth_, maxWidth_);
	}
	else
	{
		constraint.width = std::min(constraint.width, maxWidth_);
	}

	if (!IsAutoSize(height_))
	{
		constraint.height = ClampF(height_, minHeight_, maxHeight_);
	}
	else
	{
		constraint.height = std::min(constraint.height, maxHeight_);
	}

	cc::size core = MeasureOverride(constraint);

	if (!IsAutoSize(width_))
	{
		core.width = width_;
	}

	if (!IsAutoSize(height_))
	{
		core.height = height_;
	}

	core.width = ClampF(core.width, minWidth_, maxWidth_);
	core.height = ClampF(core.height, minHeight_, maxHeight_);

	desiredSize_ = cc::size(core.width + marginW, core.height + marginH);
}

void UIElement::Arrange(const Rect& _finalRect)
{
	if (visibility_ == Visibility::Collapsed)
	{
		renderSize_ = cc::size(0.0f, 0.0f);
		layoutRect_ = Rect(_finalRect.x_, _finalRect.y_, 0.0f, 0.0f);
		setVisible(false);
		return;
	}

	const float marginW = margin_.Width();
	const float marginH = margin_.Height();
	const float slotW = std::max(0.0f, _finalRect.width_ - marginW);
	const float slotH = std::max(0.0f, _finalRect.height_ - marginH);

	float width;
	if (!IsAutoSize(width_))
	{
		width = width_;
	}
	else if (hAlignment_ == HorizontalAlignment::Stretch)
	{
		width = slotW;
	}
	else
	{
		width = std::min(std::max(0.0f, desiredSize_.width - marginW), slotW);
	}

	float height;
	if (!IsAutoSize(height_))
	{
		height = height_;
	}
	else if (vAlignment_ == VerticalAlignment::Stretch)
	{
		height = slotH;
	}
	else
	{
		height = std::min(std::max(0.0f, desiredSize_.height - marginH), slotH);
	}

	width = ClampF(width, minWidth_, maxWidth_);
	height = ClampF(height, minHeight_, maxHeight_);

	renderSize_ = cc::size(width, height);

	// 자식 배치 (자식 CommitToCocos는 부모 renderSize_를 참조하므로 반드시 renderSize_ 확정 이후여야 한다)
	ArrangeOverride(renderSize_);

	float x = _finalRect.x_ + margin_.left_;
	switch (hAlignment_)
	{
	case HorizontalAlignment::Center: x += (slotW - width) * 0.5f; break;
	case HorizontalAlignment::Right: x += slotW - width; break;
	default: break;
	}

	float y = _finalRect.y_ + margin_.top_;
	switch (vAlignment_)
	{
	case VerticalAlignment::Center: y += (slotH - height) * 0.5f; break;
	case VerticalAlignment::Bottom: y += slotH - height; break;
	default: break;
	}

	layoutRect_ = Rect(x, y, width, height);

	setContentSize(renderSize_);
	CommitToCocos();
	OnRenderSizeChanged(renderSize_);
}

void UIElement::InvalidateLayout()
{
	Window* pWindow = GetWindow();

	if (pWindow)
	{
		pWindow->MarkLayoutDirty();
	}
}

void UIElement::CommitToCocos()
{
	float parentHeight = 0.0f;

	if (pParentElement_)
	{
		parentHeight = pParentElement_->renderSize_.height;
	}
	else if (getParent())
	{
		parentHeight = getParent()->getContentSize().height;
	}

	setAnchorPoint(cc::vec2::ZERO);
	ignoreAnchorPointForPosition(false);
	setPosition(layoutRect_.x_, parentHeight - layoutRect_.y_ - renderSize_.height);
	setVisible(visibility_ == Visibility::Visible);
}

int UIElement::AddHandler(UIEventType _type, const EventHandler& _handler, bool _handledEventsToo /* = false */)
{
	HandlerEntry entry;
	entry.token_ = nextHandlerToken_++;
	entry.type_ = _type;
	entry.handledEventsToo_ = _handledEventsToo;
	entry.handler_ = _handler;
	handlers_.push_back(entry);
	return entry.token_;
}

void UIElement::RemoveHandler(UIEventType _type, int _token)
{
	for (auto it = handlers_.begin(); it != handlers_.end(); ++it)
	{
		if (it->type_ == _type && it->token_ == _token)
		{
			handlers_.erase(it);
			return;
		}
	}
}

void UIElement::RaiseEvent(UIEvent* _pEvent)
{
	if (_pEvent == nullptr || _pEvent->type_ <= ueNone || _pEvent->type_ >= ueMax)
	{
		_LogWarn_("[UIElement] 잘못된 이벤트 타입으로 RaiseEvent가 호출되었습니다.");
		return;
	}

	if (_pEvent->pOriginalSource_ == nullptr)
	{
		_pEvent->pOriginalSource_ = this;
	}

	_pEvent->originalSourceId_ = _pEvent->pOriginalSource_->elementId_;

	const UIEventTypeMeta& meta = g_uiEventTypeMeta[_pEvent->type_];

	// 라우트 스냅샷 (라우팅 중 요소 파괴에 대비해 ID로 보관)
	_u32 route[MaxRouteDepth_v];
	int routeCount = 0;
	UIElement* pCur = this;

	while (pCur && routeCount < MaxRouteDepth_v)
	{
		route[routeCount++] = pCur->elementId_;
		pCur = pCur->pParentElement_;
	}

	ElementRegistry* pRegistry = ElementRegistry::Get();

	if (meta.strategy_ == UIRoutingStrategy::Direct)
	{
		_pEvent->phase_ = UIRoutingPhase::Direct;
		DeliverEvent(_pEvent);
		return;
	}

	if (meta.strategy_ == UIRoutingStrategy::Tunnel)
	{
		// 터널링 전략 이벤트를 직접 Raise한 경우: 루트 -> 소스
		_pEvent->phase_ = UIRoutingPhase::Tunnel;

		for (int idx = routeCount - 1; idx >= 0; --idx)
		{
			if (UIElement* pTarget = pRegistry->Find(route[idx]))
			{
				pTarget->DeliverEvent(_pEvent);
			}
		}
		return;
	}

	// 버블링 전략: 먼저 프리뷰(터널링) 페이즈 수행
	const UIEventType mainType = _pEvent->type_;

	if (meta.previewPair_ != ueNone)
	{
		_pEvent->type_ = meta.previewPair_;
		_pEvent->phase_ = UIRoutingPhase::Tunnel;

		for (int idx = routeCount - 1; idx >= 0; --idx)
		{
			if (UIElement* pTarget = pRegistry->Find(route[idx]))
			{
				pTarget->DeliverEvent(_pEvent);
			}
		}

		_pEvent->type_ = mainType;
	}

	// 버블링 페이즈: 소스 -> 루트
	_pEvent->phase_ = UIRoutingPhase::Bubble;

	for (int idx = 0; idx < routeCount; ++idx)
	{
		if (UIElement* pTarget = pRegistry->Find(route[idx]))
		{
			pTarget->DeliverEvent(_pEvent);
		}
	}
}

void UIElement::DeliverEvent(UIEvent* _pEvent)
{
	_pEvent->pSource_ = this;

	if (!_pEvent->handled_)
	{
		OnEventRaised((int)_pEvent->originalSourceId_, _pEvent);
	}

	if (handlers_.empty())
	{
		return;
	}

	// 핸들러 내부에서 Add/RemoveHandler가 호출될 수 있으므로 스냅샷으로 순회한다.
	std::vector<HandlerEntry> snapshot = handlers_;

	for (size_t i = 0; i < snapshot.size(); ++i)
	{
		const HandlerEntry& entry = snapshot[i];

		if (entry.type_ != _pEvent->type_)
			continue;

		if (_pEvent->handled_ && !entry.handledEventsToo_)
			continue;

		if (entry.handler_)
		{
			entry.handler_(this, _pEvent);
		}
	}
}

void UIElement::CaptureMouse()
{
	InputDispatcher::Get()->Capture(this);
}

void UIElement::ReleaseMouseCapture()
{
	InputDispatcher::Get()->ReleaseCapture(this);
}

bool UIElement::IsMouseCaptured() const
{
	return InputDispatcher::Get()->IsCaptured(this);
}

void UIElement::Focus()
{
	InputDispatcher::Get()->SetFocus(this);
}

bool UIElement::IsFocused() const
{
	return InputDispatcher::Get()->GetFocused() == this;
}

UIElement* UIElement::HitTest(const cc::vec2& _worldPos)
{
	if (visibility_ != Visibility::Visible)
	{
		return nullptr;
	}

	// 나중에 추가된 자식(위에 그려지는 쪽)부터 검사
	for (int idx = GetChildElementCount() - 1; idx >= 0; --idx)
	{
		UIElement* pChild = GetChildElementAt(idx);

		if (pChild == nullptr)
			continue;

		if (UIElement* pHit = pChild->HitTest(_worldPos))
		{
			return pHit;
		}
	}

	if (IsSelfHitTestVisible() && ContainsWorldPoint(_worldPos))
	{
		return this;
	}

	return nullptr;
}

bool UIElement::ContainsWorldPoint(const cc::vec2& _worldPos) const
{
	const cc::vec2 local = convertToNodeSpace(_worldPos);
	return local.x >= 0.0f && local.x < renderSize_.width && local.y >= 0.0f && local.y < renderSize_.height;
}

cc::vec2 UIElement::WorldToLocal(const cc::vec2& _worldPos) const
{
	const cc::vec2 nodeSpace = convertToNodeSpace(_worldPos);
	return cc::vec2(nodeSpace.x, renderSize_.height - nodeSpace.y);
}

void UIElement::RefreshThemeVisuals()
{
	const int childCount = GetChildElementCount();

	for (int idx = 0; idx < childCount; ++idx)
	{
		UIElement* pChild = GetChildElementAt(idx);

		if (pChild)
		{
			pChild->RefreshThemeVisuals();
		}
	}
}

void UIElement::OnInheritedPropertyChanged()
{
	InvalidateLayout();
}

jc::String UIElement::ToString()
{
	return jc::StringUtil::Format("UIElement(#%u, %s)", elementId_, getName().c_str());
}

float UIElement::ClampF(float _value, float _min, float _max)
{
	if (_value < _min) return _min;
	if (_value > _max) return _max;
	return _value;
}

} // namespace sgui
