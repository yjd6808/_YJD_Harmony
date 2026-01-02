/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <sgcl/AIActivity.h>

class MoveComponent;

class WalkActivity : public AIActivity
{
public:
	enum Mode
	{
		eNone,
		eWander,
		eTrack
	};

	WalkActivity(Actor* _pActor);

	void OnActivitySelectFromAiRoutine(AIInfo* _pAiInfo, AIState_t _aiState) override;
	void OnUpdate(float _dt) override;

	void UpdateWander(float _dt);
	void UpdateTrack(float _dt);
	void UpdateMove(float _dt);

	virtual void UpdateMoveImpl(float _dt, MoveComponent* _pMoveComponent, bool _arrivedX, bool _arrivedY, Direction_t _lr, Direction_t _ud) = 0;

protected:
	cc::vec2 destination_;
	Actor* pTarget_;
	Mode mode_;
};
