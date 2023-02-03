/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Research/SGGunnerAction.h>

class SGActorSprite;
class SGGunnerGunShot : public SGGunnerAction {
public:
	SGGunnerGunShot(SGPlayer* player, SGActionInfo* actionInfo);

	bool onConditionCheck() override;
	void onActionBegin() override;
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
	void onKeyPressed(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) override;
	
	void onUpdate(float dt);
	void updateDownKeyCheck(SGPlayerController* controller);
	void updateRightShotTime(SGPlayerController* controller, float dt);

	void onEnemySingleHit(SGHitInfo& info);
	void onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount);

	bool runRightShotAnimation(SGActorSprite* character);
	void runLeftShotAnimation(SGActorSprite* character, int animationCode);

	void shotRight(SGActorSprite* actorSprite);
	void shotRightDown(SGActorSprite* actorSprite);
	void shotLeft(SGActorSprite* actorSprite);
	void shotLeftDown(SGActorSprite* actorSprite);
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


