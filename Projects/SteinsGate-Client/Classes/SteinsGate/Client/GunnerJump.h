/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/GunnerAction.h>

class Character;
class GunnerJump : public GunnerAction {
public:
	GunnerJump(HostPlayer* player, ActionInfo* actionInfo);

	void onUpdate(float dt) override;
	void onActionBegin() override;
	void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* frame) override;
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* frame) override;
	void onFrameBegin(ActorPartAnimation* animation, FrameTexture* frame) override;
	void onFrameEnd(ActorPartAnimation* animation, FrameTexture* frame) override;
	void onKeyPressed(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) override;

	void updateJumpUp(Character* character, float dt);
	void updateJumpDown(Character* character, float dt);

	bool shot(Character* character);

	void reboundXLeft(Character* character);
	void reboundXRight(Character* character);
	void reboundX(Character* character);
	void reboundY(Character* character);
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



