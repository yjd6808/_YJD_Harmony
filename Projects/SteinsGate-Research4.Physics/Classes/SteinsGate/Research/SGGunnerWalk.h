/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/SGGunnerAction.h>

class SGGunnerWalk : public SGGunnerAction {
public:
	SGGunnerWalk(SGPlayer* player, SGActionInfo* actionInfo);

	void onActionBegin() override;
	void onKeyPressed(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) override;
	void onKeyReleased(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) override;
	bool isHorizontalKeyPressed(SGPlayerController* controller);
	bool isVerticalKeyPressed(SGPlayerController* controller);
};

