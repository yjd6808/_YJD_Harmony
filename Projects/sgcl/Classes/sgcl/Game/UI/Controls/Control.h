/*
 * 작성자: 윤정도
 * =====================
 * sgui - 테마/상태를 가지는 컨트롤의 베이스 (WPF Control)
 * Padding + Background 브러시 + VisualState(Normal/Hover/Pressed/Focused/Disabled) 관리를 담당한다.
 */

#pragma once

#include "sgcl/Game/UI/Core/UIElement.h"
#include "sgcl/Game/UI/Core/BrushVisual.h"

namespace sgui
{

class Control : public UIElement
{
public:
	Control();

	void SetPadding(const Thickness& _padding) { padding_ = _padding; InvalidateLayout(); }
	const Thickness& GetPadding() const { return padding_; }

	void SetBackground(const BrushPtr& _brush);
	const BrushPtr& GetBackground() const { return backgroundVisual_.GetBrush(); }

	void SetFocusable(bool _focusable) { focusable_ = _focusable; }
	bool IsFocusable() const { return focusable_; }

	bool IsPressed() const { return isPressed_; }

	virtual void UpdateVisualState() override;
	virtual void RefreshThemeVisuals() override;

	virtual void OnEventRaised(int _id, UIEvent* _pEvent) override;

protected:
	virtual UIVisualState MapVisualState() const;

	virtual void OnRenderSizeChanged(const cc::size& _size) override;

	Thickness padding_;
	BrushVisual backgroundVisual_;
	bool focusable_ = false;
	bool isPressed_ = false;
};

} // namespace sgui
