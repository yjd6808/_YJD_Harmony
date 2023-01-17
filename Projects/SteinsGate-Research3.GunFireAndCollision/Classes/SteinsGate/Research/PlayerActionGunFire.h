/*
 * 작성자: 윤정도
 * 생성일: 1/15/2023 1:01:00 AM
 * =====================
 *
 */


#pragma once

// #include <SteinsGate/Research/ActionAbstract.h>
#include <SteinsGate/Research/ComboAction.h>


class PlayerActionGunFire : public ComboAction {
public:
	static PlayerActionGunFire* createRetain();

	bool init() override;
	void onActionBegin() override;
	void onActionEnd() override;
	void onAnimateBegin(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame) override;
	void onAnimateEnd(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame) override;
	void onFrameBegin(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame) override;
	void onKeyPressed(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) override;
	void onUpdate(float dt);
	void shot(CharacterSprite* character, int motionState);
private:
	bool m_bDownShotKeyPressedFirst{};	// 아래 방향키가 Shot[Down]Begin <--> ShotRight[Down]Begin 사이에 눌렸는지 체크용 처음에 한번만 체크함 / x를 누르고 아래방향을 누르는 경우가 잦음
	bool m_bDownShotKeyPressed{};		// 아래 방향키가 눌렸는지
	bool m_bNextFireCheck{};			// 
	bool m_bNextFire{};					// 
	bool m_bShotEnd{};					// 오른쪽 총 쏠때 키입력이 없어서 왼쪽 총을 꺼내지 않을지
	int m_iRightShotCount{};			// 오른쪽 총으로 몇번 쏠지
};



