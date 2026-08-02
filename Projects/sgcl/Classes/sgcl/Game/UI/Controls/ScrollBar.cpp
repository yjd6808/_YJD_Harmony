/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/ScrollBar.h"
#include "sgcl/Game/UI/Controls/Button.h"
#include "sgcl/Game/UI/Controls/Thumb.h"
#include "sgcl/Game/UI/Core/ElementRegistry.h"
#include <cmath>

namespace sgui
{

ScrollBar* ScrollBar::Create()
{
	ScrollBar* pScrollBar = dbg_new ScrollBar;
	pScrollBar->autorelease();
	pScrollBar->SetBackground(ThemeBrush::Create(UIAssetSemantic::ScrollBarTrack, UIElementType::ScrollBar, UIComponentSlot::Track));
	pScrollBar->BuildParts();
	return pScrollBar;
}

void ScrollBar::BuildParts()
{
	pUpButton_ = Button::Create();
	pUpButton_->setName("up");
	pUpButton_->SetParentElement(this);
	pUpButton_->SetClickCallback([this](ButtonBase*) { SetRowPos(rowPos_ - 1); });
	addChild(pUpButton_);

	pDownButton_ = Button::Create();
	pDownButton_->setName("down");
	pDownButton_->SetParentElement(this);
	pDownButton_->SetClickCallback([this](ButtonBase*) { SetRowPos(rowPos_ + 1); });
	addChild(pDownButton_);

	pThumb_ = Thumb::Create();
	pThumb_->setName("thumb");
	pThumb_->SetParentElement(this);
	pThumb_->SetBackground(ThemeBrush::Create(UIAssetSemantic::ScrollBarThumb, UIElementType::ScrollBar, UIComponentSlot::Thumb));
	addChild(pThumb_);

	// Thumb 드래그로 행 위치 계산
	pThumb_->AddHandler(ueDragStarted, [this](UIElement*, UIEvent*)
	{
		dragStartRatio_ = GetEndPos() > 0 ? (float)rowPos_ / GetEndPos() : 0.0f;
	});

	pThumb_->AddHandler(ueDragDelta, [this](UIElement*, UIEvent* _pEvent)
	{
		const int endPos = GetEndPos();

		if (endPos <= 0)
			return;

		const float trackHeight = renderSize_.height - ButtonHeight_v * 2.0f;
		const float thumbHeight = pThumb_->GetRenderSize().height;
		const float movable = trackHeight - thumbHeight;

		if (movable <= 0.0f)
			return;

		auto* pDragEvent = _pEvent->Cast<UIEventDrag>();
		const float ratio = dragStartRatio_ + pDragEvent->total_.y / movable;
		SetRowPos((int)std::lround(ratio * endPos));
	});
}

void ScrollBar::SetRowCount(int _rowCount)
{
	rowCount_ = std::max(0, _rowCount);
	SetRowPos(rowPos_);
	SyncThumb();
}

void ScrollBar::SetRowCountPerPage(int _rowCountPerPage)
{
	rowCountPerPage_ = std::max(1, _rowCountPerPage);
	SetRowPos(rowPos_);
	SyncThumb();
}

void ScrollBar::SetRowPos(int _rowPos)
{
	const int clamped = std::min(std::max(0, _rowPos), GetEndPos());

	if (clamped == rowPos_)
	{
		return;
	}

	rowPos_ = clamped;
	SyncThumb();
	RaiseScrollChanged();
}

void ScrollBar::SetLinkElement(UIElement* _pLink)
{
	if (linkElementId_ != 0)
	{
		if (UIElement* pOldLink = ElementRegistry::Get()->Find(linkElementId_))
		{
			pOldLink->RemoveHandler(ueMouseWheel, linkHandlerToken_);
		}

		linkElementId_ = 0;
		linkHandlerToken_ = 0;
	}

	if (_pLink == nullptr)
	{
		return;
	}

	linkElementId_ = _pLink->GetElementId();
	linkHandlerToken_ = _pLink->AddHandler(ueMouseWheel, [this](UIElement*, UIEvent* _pEvent)
	{
		auto* pScrollEvent = _pEvent->Cast<UIEventScroll>();
		SetRowPos(rowPos_ + (pScrollEvent->deltaY_ > 0.0f ? -1 : 1));
		_pEvent->handled_ = true;
	});
}

UIElement* ScrollBar::GetChildElementAt(int _index) const
{
	switch (_index)
	{
	case 0: return pUpButton_;
	case 1: return pDownButton_;
	case 2: return pThumb_;
	default: return nullptr;
	}
}

void ScrollBar::OnEventRaised(int _id, UIEvent* _pEvent)
{
	Control::OnEventRaised(_id, _pEvent);

	if (_pEvent->type_ == ueMouseWheel && _pEvent->phase_ == UIRoutingPhase::Bubble && !_pEvent->handled_)
	{
		auto* pScrollEvent = _pEvent->Cast<UIEventScroll>();
		SetRowPos(rowPos_ + (pScrollEvent->deltaY_ > 0.0f ? -1 : 1));
		_pEvent->handled_ = true;
	}
}

void ScrollBar::RefreshThemeVisuals()
{
	Control::RefreshThemeVisuals();
}

cc::size ScrollBar::MeasureOverride(const cc::size& _constraint)
{
	const cc::size buttonSize(16.0f, ButtonHeight_v);
	pUpButton_->Measure(buttonSize);
	pDownButton_->Measure(buttonSize);
	pThumb_->Measure(_constraint);
	return cc::size(16.0f, 64.0f);
}

void ScrollBar::ArrangeOverride(const cc::size& _finalSize)
{
	pUpButton_->Arrange(Rect(0.0f, 0.0f, _finalSize.width, ButtonHeight_v));
	pDownButton_->Arrange(Rect(0.0f, _finalSize.height - ButtonHeight_v, _finalSize.width, ButtonHeight_v));

	// Thumb는 SyncThumb에서 계산된 위치/크기를 사용한다.
	SyncThumb();
}

void ScrollBar::SyncThumb()
{
	if (pThumb_ == nullptr)
	{
		return;
	}

	const float trackHeight = std::max(0.0f, renderSize_.height - ButtonHeight_v * 2.0f);
	const int endPos = GetEndPos();

	if (rowCount_ <= 0 || endPos <= 0)
	{
		// 스크롤이 불필요하면 트랙 전체를 Thumb이 채운다.
		pThumb_->Arrange(Rect(0.0f, ButtonHeight_v, renderSize_.width, trackHeight));
		return;
	}

	const float pageRatio = std::min(1.0f, (float)rowCountPerPage_ / rowCount_);
	const float thumbHeight = std::max(MinThumbHeight_v, trackHeight * pageRatio);
	const float movable = std::max(0.0f, trackHeight - thumbHeight);
	const float posRatio = (float)rowPos_ / endPos;

	pThumb_->Arrange(Rect(0.0f, ButtonHeight_v + movable * posRatio, renderSize_.width, thumbHeight));
}

void ScrollBar::RaiseScrollChanged()
{
	UIEventScrollChanged event;
	event.type_ = ueScrollChanged;
	event.rowPos_ = rowPos_;
	RaiseEvent(&event);
}

} // namespace sgui
