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
	IdleActivity_Goblin(Actor* _pActor);

	void OnUpdate(float _dt) override;

	void OnActivitySelectFromAiRoutine(AIInfo* _pAiInfo, AIState_t _aiState) override;
	void OnActivityBegin() override;
	void OnActivityEnd() override;
};
