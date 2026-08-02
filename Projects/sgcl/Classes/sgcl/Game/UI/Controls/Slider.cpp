/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/Slider.h"
#include "sgcl/Game/UI/Controls/Thumb.h"

namespace sgui
{

Slider* Slider::Create()
{
	Slider* pSlider = dbg_new Slider;
	pSlider->autorelease();
	pSlider->SetThemeControl(UIThemeControl::Slider);

	// 슬라이더 전용 UIElementType이 없으므로 스크롤바 스타일로 Resolve한다.
	pSlider->SetBackground(ThemeBrush::Create(UIAssetSemantic::SliderTrack, UIElementType::ScrollBar, UIComponentSlot::Track));

	pSlider->fillVisual_.Attach(pSlider, -90);
	pSlider->fillVisual_.SetBrush(ThemeBrush::Create(UIAssetSemantic::SliderFill, UIElementType::ScrollBar, UIComponentSlot::Gauge));

	pSlider->BuildParts();
	return pSlider;
}

void Slider::BuildParts()
{
	pThumb_ = Thumb::Create();
	pThumb_->setName("thumb");
	pThumb_->SetParentElement(this);
	pThumb_->SetBackground(ThemeBrush::Create(UIAssetSemantic::SliderThumb, UIElementType::ScrollBar, UIComponentSlot::Thumb));
	addChild(pThumb_);

	pThumb_->AddHandler(ueDragDelta, [this](UIElement*, UIEvent* _pEvent)
	{
		auto* pDragEvent = _pEvent->Cast<UIEventDrag>();
		SetValueFromWorld(pDragEvent->screenPos_);
	});
}

UIElement* Slider::GetChildElementAt(int _index) const
{
	return _index == 0 ? pThumb_ : nullptr;
}

void Slider::OnEventRaised(int _id, UIEvent* _pEvent)
{
	RangeBase::OnEventRaised(_id, _pEvent);

	// 트랙 클릭 시 해당 위치로 값 이동
	if (_pEvent->type_ == ueMouseDown && _pEvent->phase_ == UIRoutingPhase::Bubble && !_pEvent->handled_ && IsEffectivelyEnabled())
	{
		SetValueFromWorld(_pEvent->Cast<UIEventMouse>()->screenPos_);
		_pEvent->handled_ = true;
	}
}

void Slider::RefreshThemeVisuals()
{
	fillVisual_.RefreshTheme();
	RangeBase::RefreshThemeVisuals();
}

cc::size Slider::MeasureOverride(const cc::size& _constraint)
{
	pThumb_->Measure(_constraint);
	return cc::size(120.0f, ThumbSize_v);
}

void Slider::ArrangeOverride(const cc::size& _finalSize)
{
	const float movable = std::max(0.0f, _finalSize.width - ThumbSize_v);
	pThumb_->Arrange(Rect(movable * GetRatio(), (_finalSize.height - ThumbSize_v) * 0.5f, ThumbSize_v, ThumbSize_v));
}

void Slider::OnValueChanged(float _oldValue, float _newValue)
{
	UNUSED(_oldValue);
	UNUSED(_newValue);
	SyncVisuals();
}

void Slider::OnRenderSizeChanged(const cc::size& _size)
{
	RangeBase::OnRenderSizeChanged(_size);
	SyncVisuals();
}

void Slider::SyncVisuals()
{
	fillVisual_.Resize(cc::size(renderSize_.width * GetRatio(), renderSize_.height));

	const float movable = std::max(0.0f, renderSize_.width - ThumbSize_v);
	pThumb_->Arrange(Rect(movable * GetRatio(), (renderSize_.height - ThumbSize_v) * 0.5f, ThumbSize_v, ThumbSize_v));
}

void Slider::SetValueFromWorld(const cc::vec2& _worldPos)
{
	const float localX = WorldToLocal(_worldPos).x;
	const float movable = std::max(1.0f, renderSize_.width - ThumbSize_v);
	const float ratio = ClampF((localX - ThumbSize_v * 0.5f) / movable, 0.0f, 1.0f);
	SetValue(minimum_ + (maximum_ - minimum_) * ratio);
}

} // namespace sgui
