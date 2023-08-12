/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "IdleActivity_Goblin.h"

#include <SteinsGate/Client/Monster.h>
#include <SteinsGate/Client/AIInfo.h>
#include <SteinsGate/Client/Define_Animation.h>

IdleActivity_Goblin::IdleActivity_Goblin(Actor* actor)
	: IdleActivity(actor)
{}

void IdleActivity_Goblin::onUpdate(float dt) {
	updateLimitTime(dt);
}

void IdleActivity_Goblin::onActivitySelectFromAIRoutine(AIInfo* aiInfo, AIState_t aiState) {
	m_fLimitTime = SGRandom::random_real(aiInfo->IdleTime[0], aiInfo->IdleTime[1]);
}

void IdleActivity_Goblin::onActivityBegin() {
	m_pActor->runAnimation(MONSTER_ANIMATION_GOBLIN_IDLE);
}

void IdleActivity_Goblin::onActivityEnd() {

}

