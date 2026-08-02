/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Core/UIEvent.h"
#include "sgcl/Game/UI/Core/UIElement.h"

namespace sgui
{

// UIEventType 순서와 반드시 1:1로 일치해야 한다.
const UIEventTypeMeta g_uiEventTypeMeta[ueMax] =
{
	{ UIRoutingStrategy::Direct, ueNone, "None" },						// ueNone

	{ UIRoutingStrategy::Tunnel, ueNone, "PreviewMouseDown" },			// uePreviewMouseDown
	{ UIRoutingStrategy::Tunnel, ueNone, "PreviewMouseUp" },			// uePreviewMouseUp
	{ UIRoutingStrategy::Tunnel, ueNone, "PreviewMouseMove" },			// uePreviewMouseMove
	{ UIRoutingStrategy::Tunnel, ueNone, "PreviewMouseWheel" },			// uePreviewMouseWheel
	{ UIRoutingStrategy::Tunnel, ueNone, "PreviewKeyDown" },			// uePreviewKeyDown
	{ UIRoutingStrategy::Tunnel, ueNone, "PreviewKeyUp" },				// uePreviewKeyUp

	{ UIRoutingStrategy::Bubble, uePreviewMouseDown, "MouseDown" },		// ueMouseDown
	{ UIRoutingStrategy::Bubble, uePreviewMouseUp, "MouseUp" },			// ueMouseUp
	{ UIRoutingStrategy::Bubble, uePreviewMouseMove, "MouseMove" },		// ueMouseMove
	{ UIRoutingStrategy::Bubble, uePreviewMouseWheel, "MouseWheel" },	// ueMouseWheel
	{ UIRoutingStrategy::Bubble, uePreviewKeyDown, "KeyDown" },			// ueKeyDown
	{ UIRoutingStrategy::Bubble, uePreviewKeyUp, "KeyUp" },				// ueKeyUp

	{ UIRoutingStrategy::Direct, ueNone, "MouseEnter" },				// ueMouseEnter
	{ UIRoutingStrategy::Direct, ueNone, "MouseLeave" },				// ueMouseLeave
	{ UIRoutingStrategy::Direct, ueNone, "GotFocus" },					// ueGotFocus
	{ UIRoutingStrategy::Direct, ueNone, "LostFocus" },					// ueLostFocus

	{ UIRoutingStrategy::Bubble, ueNone, "Click" },						// ueClick
	{ UIRoutingStrategy::Bubble, ueNone, "CheckedChanged" },			// ueCheckedChanged
	{ UIRoutingStrategy::Bubble, ueNone, "ValueChanged" },				// ueValueChanged
	{ UIRoutingStrategy::Bubble, ueNone, "ScrollChanged" },				// ueScrollChanged
	{ UIRoutingStrategy::Bubble, ueNone, "TextChanged" },				// ueTextChanged
	{ UIRoutingStrategy::Bubble, ueNone, "TextSubmit" },				// ueTextSubmit
	{ UIRoutingStrategy::Bubble, ueNone, "DragStarted" },				// ueDragStarted
	{ UIRoutingStrategy::Bubble, ueNone, "DragDelta" },					// ueDragDelta
	{ UIRoutingStrategy::Bubble, ueNone, "DragCompleted" },				// ueDragCompleted

	{ UIRoutingStrategy::Direct, ueNone, "WindowClosed" }				// ueWindowClosed
};

cc::vec2 UIEventMouse::GetPosition(const UIElement* _pRelativeTo) const
{
	if (_pRelativeTo == nullptr)
	{
		return screenPos_;
	}

	return _pRelativeTo->WorldToLocal(screenPos_);
}

} // namespace sgui
