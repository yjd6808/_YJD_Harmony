/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:34 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "UI_Login.h"
#include "SGUIGroup.h"

USING_NS_CC;
USING_NS_JC;


UI_Login::UI_Login(SGUIGroupInfo* groupInfo)
	: SGUIMasterGroup(groupInfo)
{}

void UI_Login::onInit() {

}


bool UI_Login::onUpdate(float dt) {
	return true;
}

bool UI_Login::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	return true;
}

bool UI_Login::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	return true;
}


