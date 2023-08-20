/*
 * 작성자: 윤정도
 * 생성일: 3/22/2023 12:52:57 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIMasterGroup.h"

UIMasterGroup::UIMasterGroup(UIGroupInfo* groupInfo)
	: UIGroup(this, nullptr, groupInfo, false)	// 마스터 그룹은 기본적으로 주입된 UI정보를 가진다.
{}

bool UIMasterGroup::init() {
	return UIGroup::init();
}

void UIMasterGroup::onLoaded() {}
void UIMasterGroup::onUnloaded() {}
void UIMasterGroup::onAdded() {}
void UIMasterGroup::onRemoved() {}

void UIMasterGroup::onMouseEnterTarget(UIElement* element, SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseMoveTarget(UIElement* element, SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseDownTarget(UIElement* element, SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseUpTarget(UIElement* element, SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseLeaveTarget(UIElement* element, SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseScrollTarget(UIElement* element, SGEventMouse* mouseEvent) {}

void UIMasterGroup::onDragEnter(UIElement* element, const DragState& dragState) {}
void UIMasterGroup::onDragMove(UIElement* element, const DragState& dragState) {}
void UIMasterGroup::onDragEnd(UIElement* element) {}

void UIMasterGroup::onCheckedStateChanged(UICheckBox* checkBox, bool checked) {}
void UIMasterGroup::onToggleStateChanged(UIToggleButton* toggleBtn, ToggleState state) {}

void UIMasterGroup::onScrollBarUpButtonPressed(UIScrollBar* scrollBar, int rowPos) {}
void UIMasterGroup::onScrollBarDownButtonPressed(UIScrollBar* scrollBar, int rowPos) {}
void UIMasterGroup::onScrollBarThumbButtonPressed(UIScrollBar* scrollBar, int rowPos) {}
void UIMasterGroup::onScrollBarPositionChanged(UIScrollBar* scrollBar, int prevRowPos, int curRosPos) {}

void UIMasterGroup::onEditBoxEditingDidBegin(UIEditBox* editBox) {}
void UIMasterGroup::onEditBoxTextChanged(UIEditBox* editBox, const std::string& str) {}
void UIMasterGroup::onEditBoxReturn(UIEditBox* editBox) {}
void UIMasterGroup::onEditBoxEditingDidEndWithAction(UIEditBox* editBox, SGEditBoxEndAction endAction) {}

void UIMasterGroup::onMouseEnter(SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseMove(SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseDown(SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseUp(SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseLeave(SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseScroll(SGEventMouse* mouseEvent) {}