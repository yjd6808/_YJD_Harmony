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

	WalkActivity(Actor* actor);

	void onActivitySelectFromAIRoutine(AIInfo* aiInfo, AIState_t aiState) override;
	void onUpdate(float dt) override;

	void updateWander(float dt);
	void updateTrack(float dt);
	void updateMove(float dt);

	virtual void updateMoveImpl(float dt, MoveComponent* moveComponent, bool arrivedX, bool arrivedY, Direction_t lr, Direction_t ud) = 0;
protected:
	SGVec2 m_Destination;
	Actor* m_pTarget;
	Mode m_eMode;
};


