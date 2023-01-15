/*
 * 작성자: 윤정도
 * 생성일: 1/14/2023 7:01:25 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/ComboAction.h>

class PlayerActionSliding : public ComboAction {
public:
	static PlayerActionSliding* createRetain(SpriteDirection_t direction);

	PlayerActionSliding(SpriteDirection_t direction)
		: m_eDirection(direction) {}

	bool init() override;
private:
	SpriteDirection_t m_eDirection;
};



