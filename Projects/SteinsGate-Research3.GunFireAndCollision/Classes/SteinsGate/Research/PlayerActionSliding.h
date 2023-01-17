/*
 * 작성자: 윤정도
 * 생성일: 1/14/2023 10:31:13 AM
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
	void onActionBegin() override;
private:
	SpriteDirection_t m_eDirection;
};
