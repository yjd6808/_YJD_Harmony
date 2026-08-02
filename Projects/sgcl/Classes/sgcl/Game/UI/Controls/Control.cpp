/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/Control.h"

namespace sgui
{

Control::Control()
{
	backgroundVisual_.Attach(this, -100);
}

void Control::SetBackground(const BrushPtr& _brush)
{
	backgroundVisual_.SetBrush(_brush);
	backgroundVisual_.Resize(renderSize_);
	backgroundVisual_.SetVisualState(MapVisualState());
}

void Control::UpdateVisualState()
{
	backgroundVisual_.SetVisualState(MapVisualState());
}

void Control::RefreshThemeVisuals()
{
	backgroundVisual_.RefreshTheme();
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
}

} // namespace sgui
