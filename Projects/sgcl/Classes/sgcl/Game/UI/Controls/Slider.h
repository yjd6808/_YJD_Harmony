/*
 * 작성자: 윤정도
 * =====================
 * sgui - 수평 슬라이더 (WPF Slider)
 */

#pragma once

#include "sgcl/Game/UI/Controls/RangeBase.h"

namespace sgui
{

class Thumb;

class Slider : public RangeBase
{
public:
	static constexpr float ThumbSize_v = 16.0f;
	static constexpr float TrackHeight_v = 6.0f;

	static Slider* Create();

	virtual int GetChildElementCount() const override { return 1; }
	virtual UIElement* GetChildElementAt(int _index) const override;

	virtual void OnEventRaised(int _id, UIEvent* _pEvent) override;
	virtual void RefreshThemeVisuals() override;

protected:
	Slider() = default;

	virtual cc::size MeasureOverride(const cc::size& _constraint) override;
	virtual void ArrangeOverride(const cc::size& _finalSize) override;
	virtual void OnValueChanged(float _oldValue, float _newValue) override;
	virtual void OnRenderSizeChanged(const cc::size& _size) override;

	void BuildParts();
	void SyncVisuals();
	void SetValueFromWorld(const cc::vec2& _worldPos);

	Thumb* pThumb_ = nullptr;
	BrushVisual fillVisual_;
};

} // namespace sgui
