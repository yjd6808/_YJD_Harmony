/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/GunnerAction.h>

class ActorSprite;
class GunnerGunShot : public GunnerAction {
public:
	GunnerGunShot(HostPlayer* player, ActionInfo* actionInfo);

	bool onConditionCheck() override;
	void onActionBegin() override;
	void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* frame) override;
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* frame) override;
	void onFrameBegin(ActorPartAnimation* animation, FrameTexture* frame) override;
	void onKeyPressed(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) override;
	
	void onUpdate(float dt);
	void updateDownKeyCheck(PlayerController* controller);
	void updateRightShotTime(PlayerController* controller, float dt);

	void onEnemySingleHit(HitInfo& info);
	void onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount);

	bool runRightShotAnimation(ActorSprite* character);
	void runLeftShotAnimation(ActorSprite* character, int animationCode);

	void shotRight(ActorSprite* actorSprite);
	void shotRightDown(ActorSprite* actorSprite);
	void shotLeft(ActorSprite* actorSprite);
	void shotLeftDown(ActorSprite* actorSprite);
private:
	bool m_bDownShotKeyPressedFirst;	// 아래 방향키가 Shot[Down]Begin <--> ShotRight[Down]Begin 사이에 눌렸는지 체크용 처음에 한번만 체크함 / x를 누르고 아래방향을 누르는 경우가 잦음
	bool m_bDownShotKeyPressed;			// 아래 방향키가 눌렸는지
	bool m_bNextFireCheck;
	bool m_bShotEnd;					// 오른쪽 총 쏠때 키입력이 없어서 왼쪽 총을 꺼내지 않을지

	bool m_bWaitRightShot;		
	float m_fWaitRightShotTime;			// 오른쪽 총을 1발 쏜 후부터 시간 경과 체크 후 일정시간 입력이 없으면 애니메이션 총쏘기 중지토록함.

	int m_iContinuosCount;				// 총을 든 후 x 키입력을 몇번했는지
	int m_iRightShotCount;				// 오른쪽 총으로 몇번 쏠지
};


