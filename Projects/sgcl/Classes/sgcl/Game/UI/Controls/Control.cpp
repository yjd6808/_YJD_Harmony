/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/Control.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"

namespace sgui
{

Control::Control()
{
	backgroundVisual_.Attach(this, -100);
	borderVisual_.Attach(this, -50);
}

void Control::SetBackground(const BrushPtr& _brush)
{
	backgroundVisual_.SetBrush(_brush);
	backgroundVisual_.Resize(renderSize_);
	backgroundVisual_.SetVisualState(MapVisualState());
}

void Control::SetBorderBrush(const BrushPtr& _brush)
{
	borderVisual_.SetBrush(_brush);
	borderVisual_.SetVisualState(MapVisualState());
	borderVisual_.Resize(renderSize_);
}

void Control::SetBorderThickness(const Thickness& _thickness)
{
	borderVisual_.SetThickness(_thickness);
}

void Control::UpdateVisualState()
{
	const UIVisualState state = MapVisualState();

	backgroundVisual_.SetVisualState(state);
	borderVisual_.SetVisualState(state);

	// 상태별 Foreground(hover-foreground 등)를 자식 텍스트에 재동기화한다.
	RefreshForegroundVisuals();
}

bool Control::TryGetForegroundColor(UIColorF& _outColor) const
{
	if (hasForeground_)
	{
		_outColor = ResolveBrushColor(foregroundBrush_, MapVisualState());
		return true;
	}

	// WPF처럼 컨트롤은 항상 테마 기본 Foreground를 갖는다. (예: ButtonHoverForeground)
	_outColor = UIThemeManager::Get()->GetColors().Get(themeControl_, MapVisualState(), UIThemeColorRole::Foreground);
	return true;
}

void Control::RefreshThemeVisuals()
{
	backgroundVisual_.RefreshTheme();
	borderVisual_.RefreshTheme();
	UIElement::RefreshThemeVisuals();
}

void Control::OnEventRaised(int _id, UIEvent* _pEvent)
{
	UNUSED(_id);

	switch (_pEvent->type_)
	{
	case ueMouseDown:
		if (_pEvent->phase_ == UIRoutingPhase::Bubble && focusable_ && IsEffectivelyEnabled())
		{
			Focus();
		}
		break;
	case ueMouseEnter:
	case ueMouseLeave:
	case ueGotFocus:
	case ueLostFocus:
		UpdateVisualState();
		break;
	default:
		break;
	}
}

UIVisualState Control::MapVisualState() const
{
	if (!IsEffectivelyEnabled())
		return UIVisualState::Disabled;

	if (isPressed_)
		return UIVisualState::Pressed;

	if (isMouseOver_)
		return UIVisualState::Hover;

	if (IsFocused())
		return UIVisualState::Focused;

	return UIVisualState::Normal;
}

void Control::OnRenderSizeChanged(const cc::size& _size)
{
	backgroundVisual_.Resize(_size);
	borderVisual_.Resize(_size);
}

} // namespace sgui
