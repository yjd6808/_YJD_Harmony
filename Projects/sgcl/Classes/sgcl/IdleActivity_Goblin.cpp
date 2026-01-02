/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Core.h"
#include "IdleActivity_Goblin.h"

#include <sgcl/Monster.h>
#include <sg/_Struct/SteinsGate_AI.h>
#include <sgcl/Define_Animation.h>

//////////////////////////////////////////////////////////////////////////////////////////
IdleActivity_Goblin::IdleActivity_Goblin(Actor* _pActor)
: IdleActivity(_pActor)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void IdleActivity_Goblin::OnUpdate(float _dt)
{
	UpdateLimitTime(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void IdleActivity_Goblin::OnActivitySelectFromAiRoutine(AIInfo* _pAiInfo, AIState_t _aiState)
{
	limitTime_ = jc::Random::GenerateF(_pAiInfo->idleTime_[0], _pAiInfo->idleTime_[1]);
}

//////////////////////////////////////////////////////////////////////////////////////////
void IdleActivity_Goblin::OnActivityBegin()
{
	pActor_->RunAnimation(DEF_ANIMATION_MONSTER_GOBLIN_IDLE);
}

//////////////////////////////////////////////////////////////////////////////////////////
void IdleActivity_Goblin::OnActivityEnd()
{
}
