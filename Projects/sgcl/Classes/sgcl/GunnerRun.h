/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */

#pragma once

#include <sgcl/GunnerAction.h>

class GunnerRun : public GunnerAction
{
public:
	GunnerRun(HostPlayer* _pPlayer, ActionInfo* _pActionInfo);

	void OnActionBegin() override;
	void OnKeyPressed(PlayerController* _pController, c2d::EventKeyboard::KeyCode _keyCode) override;
	void OnKeyReleased(PlayerController* _pController, c2d::EventKeyboard::KeyCode _keyCode) override;
	void OnKeyReleasedBefore(PlayerController* _pController, c2d::EventKeyboard::KeyCode _keyCode) override;
};
