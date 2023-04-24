/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/GunnerAction.h>

class GunnerIdle : public GunnerAction {
public:
	GunnerIdle(HostPlayer* player, ActionInfo* actionInfo);

	void init() override;
	void onActionBegin() override;
	void onActionEnd() override;

	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* frame) override;
	void onKeyPressed(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) override;
	void onKeyReleasedBefore(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) override;
	void fixFreezedState(PlayerController* controller, ControlKey_t releasedKey);
private:
	int m_iIdleCount;
};




