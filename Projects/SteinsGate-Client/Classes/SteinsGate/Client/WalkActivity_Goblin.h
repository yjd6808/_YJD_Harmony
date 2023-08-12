/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/WalkActivity.h>

class WalkActivity_Goblin : public WalkActivity
{
public:
	WalkActivity_Goblin(Actor* actor);

	void onActivityBegin() override;
	void updateMoveImpl(float dt, MoveComponent* moveComponent, bool arrivedX, bool arrivedY, Direction_t lr, Direction_t ud) override;
};


