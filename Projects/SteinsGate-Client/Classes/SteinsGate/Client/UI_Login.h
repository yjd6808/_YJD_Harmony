/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:08 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGUIMasterGroup.h>

class UI_Login : public SGUIMasterGroup
{
public:
	UI_Login(SGUIGroupInfo* groupInfo);

	void onInit() override;
	bool onUpdate(float dt) override;
	bool onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
	bool onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) override;
};


