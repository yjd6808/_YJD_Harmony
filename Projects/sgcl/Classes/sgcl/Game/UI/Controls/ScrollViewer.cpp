/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/ScrollViewer.h"

USING_NS_CC;

namespace sgui
{

ScrollViewer* ScrollViewer::Create()
{
	ScrollViewer* pViewer = dbg_new ScrollViewer;
	pViewer->autorelease();

	pViewer->pClipNode_ = ClippingRectangleNode::create();
	pViewer->pClipNode_->setAnchorPoint(cc::vec2::ZERO);
	pViewer->addChild(pViewer->pClipNode_);

	return pViewer;
}

void ScrollViewer::SetContent(UIElement* _pContent)
{
	if (pContent_ == _pContent)
	{
		return;
	}

	if (pContent_)
	{
		pContent_->SetParentElement(nullptr);
		pContent_->removeFromParent();
	}

	pContent_ = _pContent;

	if (pContent_)
	{
		pContent_->SetParentElement(this);
		pClipNode_->addChild(pContent_);
	}

	verticalOffset_ = 0.0f;
	InvalidateLayout();
}

void ScrollViewer::SetVerticalOffset(float _offset)
{
	verticalOffset_ = ClampF(_offset, 0.0f, GetScrollableHeight());
	SyncContentPosition();
}

float ScrollViewer::GetScrollableHeight() const
{
	if (pContent_ == nullptr)
	{
		return 0.0f;
	}

	return std::max(0.0f, pContent_->GetRenderSize().height - renderSize_.height);
}

void ScrollViewer::OnEventRaised(int _id, UIEvent* _pEvent)
{
	Control::OnEventRaised(_id, _pEvent);

	if (_pEvent->type_ == ueMouseWheel && _pEvent->phase_ == UIRoutingPhase::Bubble && !_pEvent->handled_)
	{
		auto* pScrollEvent = _pEvent->Cast<UIEventScroll>();
		SetVerticalOffset(verticalOffset_ + (pScrollEvent->deltaY_ > 0.0f ? -wheelStep_ : wheelStep_));
		_pEvent->handled_ = true;
	}
}

cc::size ScrollViewer::MeasureOverride(const cc::size& _constraint)
{
	if (pContent_ == nullptr)
	{
		return cc::size(0.0f, 0.0f);
	}

	// 세로는 무제한 공간을 줌 (스크롤 영역)
	pContent_->Measure(cc::size(_constraint.width, Infinity_v));

	const cc::size& desired = pContent_->GetDesiredSize();
	return cc::size(desired.width, std::min(desired.height, _constraint.height));
}

void ScrollViewer::ArrangeOverride(const cc::size& _finalSize)
{
	if (pContent_ == nullptr)
	{
		return;
	}

	const float contentHeight = std::max(pContent_->GetDesiredSize().height, _finalSize.height);
	pContent_->Arrange(Rect(0.0f, 0.0f, _finalSize.width, contentHeight));
	SyncContentPosition();
}

void ScrollViewer::OnRenderSizeChanged(const cc::size& _size)
{
	Control::OnRenderSizeChanged(_size);

	if (pClipNode_)
	{
		pClipNode_->setContentSize(_size);
		pClipNode_->setClippingRegion(cc::rect(0.0f, 0.0f, _size.width, _size.height));
	}

	SyncContentPosition();
}

void ScrollViewer::SyncContentPosition()
{
	if (pContent_ == nullptr)
	{
		return;
	}

	// sgui는 y-down 레이아웃, 코코스는 y-up이므로 콘텐츠를 위로 밀어올린다.
	const float contentHeight = pContent_->GetRenderSize().height;
	pContent_->setPosition(0.0f, renderSize_.height - contentHeight + verticalOffset_);
}

} // namespace sgui
