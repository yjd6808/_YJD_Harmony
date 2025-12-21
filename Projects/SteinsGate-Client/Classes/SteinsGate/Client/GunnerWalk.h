/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/GunnerAction.h>

class GunnerWalk : public GunnerAction
{
public:
	GunnerWalk(HostPlayer* _pHostPlayer, ActionInfo* _pActionInfo);

	void OnActionBegin() override;
	void OnKeyPressed(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode) override;
	void OnKeyReleased(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode) override;
	bool IsHorizontalKeyPressed(PlayerController* _pController);
	bool IsVerticalKeyPressed(PlayerController* _pController);
};
