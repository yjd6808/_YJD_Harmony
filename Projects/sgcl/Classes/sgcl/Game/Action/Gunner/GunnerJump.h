/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */


#pragma once

#include "sgcl/Game/Action/Gunner/GunnerAction.h"

class Character;

class GunnerJump : public GunnerAction
{
public:
	GunnerJump(HostPlayer* _pPlayer, ActionInfo* _pActionInfo);

	void OnUpdate(float _dt) override;
	void OnActionBegin() override;
	void OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void OnKeyPressed(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode) override;

	void UpdateJumpUp(Character* _pChar, float _dt);
	void UpdateJumpDown(Character* _pChar, float _dt);

	bool Shot(Character* _pChar);

	void ReboundXLeft(Character* _pChar);
	void ReboundXRight(Character* _pChar);
	void ReboundX(Character* _pChar);
	void ReboundY(Character* _pChar);
	void CreateBullet();

private:
	bool isJumpUpBegin_{};
	bool isJumpDownBegin_{};
	bool isFireable_{};
	bool isFireMoveable_{};
	bool isWaitingForFire_{};
	bool isRightFire_{};

	int chargedShotCount_{};
	int shotCount_{};
	int maxShotCount_{};
};
