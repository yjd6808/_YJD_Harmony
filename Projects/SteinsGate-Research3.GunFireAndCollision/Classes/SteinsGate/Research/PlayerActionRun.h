/*
 * 작성자: 윤정도
 * 생성일: 1/14/2023 7:01:25 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/ComboAction.h>

class PlayerActionRun : public ComboAction {
public:
	static PlayerActionRun* createRetain(SpriteDirection_t direction);

	PlayerActionRun(SpriteDirection_t direction)
		: m_eDirection(direction) {}

	bool init() override;
	void onActionBegin() override;
	void onKeyPressed(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) override;
	void onKeyReleased(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) override;
	void onKeyReleasedBefore(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) override;
private:
	SpriteDirection_t m_eDirection;
};



