/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGGunnerAction.h>

class SGGunnerIdle : public SGGunnerAction {
public:
	SGGunnerIdle(SGHostPlayer* player, SGActionInfo* actionInfo);

	void init() override;
	void onActionBegin() override;
	void onActionEnd() override;

	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
	void onKeyPressed(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) override;
	void onKeyReleasedBefore(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) override;
	void fixFreezedState(SGPlayerController* controller, ControlKey_t releasedKey);
private:
	int m_iIdleCount;
};




