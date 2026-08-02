/*
 * 작성자: 윤정도
 * =====================
 * sgui - 진행 바 (WPF ProgressBar)
 */

#pragma once

#include "sgcl/Game/UI/Controls/RangeBase.h"

namespace sgui
{

class ProgressBar : public RangeBase
{
public:
	static ProgressBar* Create();

	// 기존 UIProgressBar 호환 API: 0 ~ 100
	void SetPercent(float _percent) { SetValue(_percent); }
	float GetPercent() const { return GetValue(); }

	virtual void RefreshThemeVisuals() override;

protected:
	virtual void OnValueChanged(float _oldValue, float _newValue) override;
	virtual void OnRenderSizeChanged(const cc::size& _size) override;

	void SyncGauge();

	BrushVisual gaugeVisual_;
};

} // namespace sgui
