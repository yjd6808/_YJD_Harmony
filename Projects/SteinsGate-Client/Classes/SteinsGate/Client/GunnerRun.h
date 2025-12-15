/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/GunnerAction.h>

class GunnerRun : public GunnerAction
{
public:
	GunnerRun(HostPlayer* _pPlayer, ActionInfo* _pActionInfo);

	void onActionBegin() override;
	void onKeyPressed(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode) override;
	void onKeyReleased(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode) override;
	void onKeyReleasedBefore(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode) override;
};
