/*
 * 작성자: 윤정도
 * 생성일: 1/17/2023 8:42:04 AM
 * =====================
 *
 */


#pragma once

#pragma once

#include <SteinsGate/Research/ComboAction.h>

// 추후 캐릭터 추가가 만에하나 되는 경우
// 이녀석을 상속받아서 구현할 것
// 그리고 ActionManager에서는 캐릭터 타입에따라 추가 달리해줄 것
class PlayerActionJump : public ComboAction {
public:
	static PlayerActionJump* createRetain();

	bool init() override;
	void onActionBegin() override;
	void onKeyPressed(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) override;
private:
	float m_fJumpVelocity;
	float m_fJumpVelocityMax;
	float m_fJumpGravity;
	// 버프 정보는 플레이어로부터 읽어서 사용할 것
};





