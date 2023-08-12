/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/IdleActivity.h>

class IdleActivity_Goblin : public IdleActivity
{
public:
	IdleActivity_Goblin(Actor* actor);

	void onUpdate(float dt) override;

	void onActivitySelectFromAIRoutine(AIInfo* aiInfo, AIState_t aiState) override;
	void onActivityBegin() override;
	void onActivityEnd() override;
};




