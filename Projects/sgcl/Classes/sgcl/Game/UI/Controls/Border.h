/*
 * 작성자: 윤정도
 * =====================
 * sgui - 테두리 + 배경 + 단일 자식 (WPF Border)
 */

#pragma once

#include "sgcl/Game/UI/Core/UIElement.h"
#include "sgcl/Game/UI/Core/BrushVisual.h"

namespace sgui
{

class Border : public UIElement
{
public:
	static Border* Create();

	void SetChild(UIElement* _pChild);
	UIElement* GetChild() const { return pChild_; }

	void SetBackground(const BrushPtr& _brush);
	void SetBorderBrush(const UIColorF& _color);
	void SetBorderThickness(const Thickness& _thickness);
	void SetPadding(const Thickness& _padding) { padding_ = _padding; InvalidateLayout(); }

	virtual int GetChildElementCount() const override { return pChild_ ? 1 : 0; }
	virtual UIElement* GetChildElementAt(int _index) const override { return _index == 0 ? pChild_ : nullptr; }

	virtual bool IsSelfHitTestVisible() const override { return isHitTestVisible_ && backgroundVisual_.GetBrush() != nullptr; }

	virtual void RefreshThemeVisuals() override;

protected:
	virtual cc::size MeasureOverride(const cc::size& _constraint) override;
	virtual void ArrangeOverride(const cc::size& _finalSize) override;
	virtual void OnRenderSizeChanged(const cc::size& _size) override;

	void SyncBorderEdges();

	UIElement* pChild_ = nullptr;
	Thickness padding_;
	Thickness borderThickness_;
	UIColorF borderColor_ { 1.0f, 1.0f, 1.0f, 1.0f };

	BrushVisual backgroundVisual_;
	cc::LayerColor* pEdges_[4] = { nullptr, nullptr, nullptr, nullptr };	// left, top, right, bottom
};

} // namespace sgui
