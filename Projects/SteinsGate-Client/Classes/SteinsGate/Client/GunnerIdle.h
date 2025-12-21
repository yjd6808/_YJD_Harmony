/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/GunnerAction.h>

class GunnerIdle : public GunnerAction
{
public:
	GunnerIdle(HostPlayer* _pPlayer, ActionInfo* _pActionInfo);

	void Init() override;
	void OnActionBegin() override;
	void OnActionEnd() override;

	void OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void OnKeyPressed(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode) override;
	void OnKeyReleasedBefore(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode) override;
	void FixFreezedState(PlayerController* _pController, ControlKey_t _releasedKey);

private:
	int idleCount_;
};
