/*
 * 
 * 
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/GunnerAction.h>

class ActorSprite;

class GunnerGunShot : public GunnerAction
{
public:
	GunnerGunShot(HostPlayer* _pPlayer, ActionInfo* _pActionInfo);

	bool onConditionCheck() override;
	void onActionBegin() override;
	void onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void onKeyPressed(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode) override;

	void onUpdate(float _deltaTime) override;
	void updateDownKeyCheck(PlayerController* _pController);
	void updateRightShotTime(PlayerController* _pController, float _deltaTime);

	void onEnemySingleHit(HitInfo& _info);
	void onEnemyMultiHit(SGHitInfoList& _hitList, int _newHitCount);

	bool runRightShotAnimation(ActorSprite* _pCharacter);
	void runLeftShotAnimation(ActorSprite* _pCharacter, int _animationCode);

	void shotRight(ActorSprite* _pActorSprite);
	void shotRightDown(ActorSprite* _pActorSprite);
	void shotLeft(ActorSprite* _pActorSprite);
	void shotLeftDown(ActorSprite* _pActorSprite);

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
