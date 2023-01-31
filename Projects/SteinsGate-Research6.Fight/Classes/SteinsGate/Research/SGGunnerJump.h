/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */


#pragma once




#include <SteinsGate/Research/SGGunnerAction.h>

class SGCharacter;
class SGGunnerJump : public SGGunnerAction {
public:
	SGGunnerJump(SGPlayer* player, SGActionInfo* actionInfo);

	void onUpdate(float dt) override;
	void onActionBegin() override;
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
	void onKeyPressed(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) override;

	void updateJumpUp(SGCharacter* character, float dt);
	void updateJumpDown(SGCharacter* character, float dt);

	bool shot(SGCharacter* character);

	void reboundXLeft(SGCharacter* character);
	void reboundXRight(SGCharacter* character);
	void reboundX(SGCharacter* character);
	void reboundY(SGCharacter* character);
	void createBullet();
private:
	bool m_bJumpUpbegin{};
	bool m_bJumpDownBegin{};
	bool m_bCanFire{};
	bool m_bFireMode{};
	bool m_bWaitForFire{};
	bool m_bRightFire{};

	int m_iChargedShotCount{};
	int m_iShotCount{};
	int m_iMaxShotCount{};
};



