/*
 * 작성자: 윤정도
 * 생성일: 3/22/2023 12:52:57 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGUIMasterGroup.h"

SGUIMasterGroup::SGUIMasterGroup(SGUIGroupInfo* groupInfo)
	: SGUIGroup(this, nullptr, groupInfo)
{}

bool SGUIMasterGroup::init() {
	return SGUIGroup::init();
}

void SGUIMasterGroup::onLoaded() {}
void SGUIMasterGroup::onUnloaded() {}
void SGUIMasterGroup::onAdded() {}
void SGUIMasterGroup::onRemoved() {}

void SGUIMasterGroup::onMouseEnter(SGUIElement* element, SGEventMouse* mouseEvent) {}
void SGUIMasterGroup::onMouseMove(SGUIElement* element, SGEventMouse* mouseEvent) {}
void SGUIMasterGroup::onMouseDown(SGUIElement* element, SGEventMouse* mouseEvent) {}
void SGUIMasterGroup::onMouseUp(SGUIElement* element, SGEventMouse* mouseEvent) {}
void SGUIMasterGroup::onMouseLeave(SGUIElement* element, SGEventMouse* mouseEvent) {}
void SGUIMasterGroup::onMouseScroll(SGUIElement* element, SGEventMouse* mouseEvent) {}
void SGUIMasterGroup::onCheckedStateChanged(SGUICheckBox* checkBox) {}
void SGUIMasterGroup::onScrollBarUpButtonPressed(SGUIScrollBar* scrollBar, int rowPos) {}
void SGUIMasterGroup::onScrollBarDownButtonPressed(SGUIScrollBar* scrollBar, int rowPos) {}
void SGUIMasterGroup::onScrollBarThumbButtonPressed(SGUIScrollBar* scrollBar, int rowPos) {}
void SGUIMasterGroup::onScrollBarPositionChanged(SGUIScrollBar* scrollBar, int prevRowPos, int curRosPos) {}

void SGUIMasterGroup::onEditBoxEditingDidBegin(SGUIEditBox* editBox) {}
void SGUIMasterGroup::onEditBoxTextChanged(SGUIEditBox* editBox, const std::string& str) {}
void SGUIMasterGroup::onEditBoxReturn(SGUIEditBox* editBox) {}
void SGUIMasterGroup::onEditBoxEditingDidEndWithAction(SGUIEditBox* editBox) {}
