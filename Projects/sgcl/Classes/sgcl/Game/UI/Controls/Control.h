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

	// ==================== BorderBrush / BorderThickness (WPF Control 동일 기능) ====================
	// 테두리는 렌더 오버레이로 그려지며 레이아웃 크기에는 영향을 주지 않는다.
	void SetBorderBrush(const BrushPtr& _brush);
	void SetBorderBrush(const UIColorF& _color) { SetBorderBrush(SolidColorBrush::Create(_color)); }
	const BrushPtr& GetBorderBrush() const { return borderVisual_.GetBrush(); }

	void SetBorderThickness(const Thickness& _thickness);
	const Thickness& GetBorderThickness() const { return borderVisual_.GetThickness(); }

	// 테마 색상 테이블(UIThemeColorTable) 조회 시 사용할 컨트롤 종류.
	// 명시적 Foreground가 없으면 (themeControl_, 현재 상태, Foreground) 색상이 글자색으로 쓰인다.
	void SetThemeControl(UIThemeControl _control) { themeControl_ = _control; UpdateVisualState(); }
	UIThemeControl GetThemeControl() const { return themeControl_; }

	void SetFocusable(bool _focusable) { focusable_ = _focusable; }
	bool IsFocusable() const { return focusable_; }

	bool IsPressed() const { return isPressed_; }

	virtual void UpdateVisualState() override;
	virtual void RefreshThemeVisuals() override;

	virtual void OnEventRaised(int _id, UIEvent* _pEvent) override;

protected:
	virtual UIVisualState MapVisualState() const;

	virtual void OnRenderSizeChanged(const cc::size& _size) override;

	// 명시적 Foreground 브러시가 없으면 테마 색상 테이블의 상태별 Foreground로 폴백한다.
	virtual bool TryGetForegroundColor(UIColorF& _outColor) const override;

	Thickness padding_;
	BrushVisual backgroundVisual_;
	BorderEdgeVisual borderVisual_;
	UIThemeControl themeControl_ = UIThemeControl::Button;
	bool focusable_ = false;
	bool isPressed_ = false;
};

} // namespace sgui
