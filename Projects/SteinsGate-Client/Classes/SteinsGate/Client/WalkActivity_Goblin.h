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
	WalkActivity_Goblin(Actor* _pActor);

	void OnActivityBegin() override;
	void updateMoveImpl(float _dt, MoveComponent* _pMoveComponent, bool _arrivedX, bool _arrivedY, Direction_t _lr,
	                    Direction_t _ud) override;
};
