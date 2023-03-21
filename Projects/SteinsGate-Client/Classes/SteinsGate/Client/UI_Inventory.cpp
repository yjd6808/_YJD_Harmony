/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:34 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UI_Inventory.h"

USING_NS_CC;
USING_NS_JC;


UI_Inventory::UI_Inventory(SGUIGroupInfo* groupInfo)
	: SGUIMasterGroup(groupInfo)
{
}


bool UI_Inventory::init() {

	if (!SGUIGroup::init()) {
		return false;
	}

	return true;
}


bool UI_Inventory::onUpdate(float dt) {
	return true;
}

bool UI_Inventory::onMouseScroll(SGEventMouse* mouseEvent) {

	return true;
}

bool UI_Inventory::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	return true;
}

bool UI_Inventory::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	return true;
}


