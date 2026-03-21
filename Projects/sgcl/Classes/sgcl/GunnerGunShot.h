/*
 * 
 * 
 * =====================
 *
 */

#pragma once

#include "sgcl/GunnerAction.h"

class ActorSprite;

class GunnerGunShot : public GunnerAction
{
public:
	GunnerGunShot(HostPlayer* _pPlayer, ActionInfo* _pActionInfo);

	bool OnConditionCheck() override;
	void OnActionBegin() override;
	void OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void OnKeyPressed(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode) override;

	void OnUpdate(float _deltaTime) override;
	void UpdateDownKeyCheck(PlayerController* _pController);
	void UpdateRightShotTime(PlayerController* _pController, float _deltaTime);

	void OnEnemySingleHit(HitInfo& _info);
	void OnEnemyMultiHit(SGHitInfoList& _hitList, int _newHitCount);

	bool RunRightShotAnimation(ActorSprite* _pCharacter);
	void RunLeftShotAnimation(ActorSprite* _pCharacter, int _animationCode);

	void ShotRight(ActorSprite* _pActorSprite);
	void ShotRightDown(ActorSprite* _pActorSprite);
	void ShotLeft(ActorSprite* _pActorSprite);
	void ShotLeftDown(ActorSprite* _pActorSprite);

private:
	bool downShotKeyPressedFirst_;
	// 아래 방향키가 Shot[Down]Begin <--> ShotRight[Down]Begin 사이에 눌렸는지 체크용 처음에 한번만 체크함 / x를 누르고 아래방향을 누르는 경우가 잦음
	bool downShotKeyPressed_; // 아래 방향키가 눌렸는지
	bool nextFireCheck_;
	bool shotEnd_; // 오른쪽 총 쏠때 키입력이 없어서 왼쪽 총을 꺼내지 않을지

	bool waitRightShot_;
	float waitRightShotTime_; // 오른쪽 총을 1발 쏜 후부터 시간 경과 체크 후 일정시간 입력이 없으면 애니메이션 총쏘기 중지토록함.

	int continuosCount_; // 총을 든 후 x 키입력을 몇번했는지
	int rightShotCount_; // 오른쪽 총으로 몇번 쏠지
};
