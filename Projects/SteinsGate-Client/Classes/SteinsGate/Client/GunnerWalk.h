/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/GunnerAction.h>

class GunnerWalk : public GunnerAction {
public:
	GunnerWalk(HostPlayer* player, ActionInfo* actionInfo);

	void onActionBegin() override;
	void onKeyPressed(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) override;
	void onKeyReleased(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) override;
	bool isHorizontalKeyPressed(PlayerController* controller);
	bool isVerticalKeyPressed(PlayerController* controller);
};

