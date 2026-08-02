/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/ProgressBar.h"

namespace sgui
{

ProgressBar* ProgressBar::Create()
{
	ProgressBar* pProgressBar = dbg_new ProgressBar;
	pProgressBar->autorelease();

	pProgressBar->SetBackground(ThemeBrush::Create(UIAssetSemantic::ProgressTrack, UIElementType::ProgressBar, UIComponentSlot::Track));

	pProgressBar->gaugeVisual_.Attach(pProgressBar, -90);
	pProgressBar->gaugeVisual_.SetBrush(ThemeBrush::Create(UIAssetSemantic::ProgressGauge, UIElementType::ProgressBar, UIComponentSlot::Gauge));

	return pProgressBar;
}

void ProgressBar::RefreshThemeVisuals()
{
	gaugeVisual_.RefreshTheme();
	RangeBase::RefreshThemeVisuals();
}

void ProgressBar::OnValueChanged(float _oldValue, float _newValue)
{
	UNUSED(_oldValue);
	UNUSED(_newValue);
	SyncGauge();
}

void ProgressBar::OnRenderSizeChanged(const cc::size& _size)
{
	RangeBase::OnRenderSizeChanged(_size);
	SyncGauge();
}

void ProgressBar::SyncGauge()
{
	gaugeVisual_.Resize(cc::size(renderSize_.width * GetRatio(), renderSize_.height));
}

} // namespace sgui
