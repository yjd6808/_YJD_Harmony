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
	: UIGroup(this, nullptr, groupInfo)
{}

bool UIMasterGroup::init() {
	return UIGroup::init();
}

void UIMasterGroup::onLoaded() {}
void UIMasterGroup::onUnloaded() {}
void UIMasterGroup::onAdded() {}
void UIMasterGroup::onRemoved() {}

void UIMasterGroup::onMouseEnter(UIElement* element, SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseMove(UIElement* element, SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseDown(UIElement* element, SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseUp(UIElement* element, SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseLeave(UIElement* element, SGEventMouse* mouseEvent) {}
void UIMasterGroup::onMouseScroll(UIElement* element, SGEventMouse* mouseEvent) {}

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
