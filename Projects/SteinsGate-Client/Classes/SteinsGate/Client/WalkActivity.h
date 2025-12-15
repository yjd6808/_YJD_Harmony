/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/AIActivity.h>

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

	void updateWander(float _dt);
	void updateTrack(float _dt);
	void updateMove(float _dt);

	virtual void updateMoveImpl(float _dt, MoveComponent* _pMoveComponent, bool _arrivedX, bool _arrivedY,
	                            Direction_t _lr, Direction_t _ud) = 0;

protected:
	SGVec2 m_Destination;
	Actor* m_pTarget;
	Mode m_eMode;
};
