/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "MonsterListener_DarkGoblin.h"

#include <SteinsGate/Client/AIComponent.h>

#include <SteinsGate/Client/WalkActivity_Goblin.h>
#include <SteinsGate/Client/IdleActivity_Goblin.h>
#include <SteinsGate/Client/AttackActivity_Goblin.h>
#include <SteinsGate/Client/HitActivity_Goblin.h>
#include <SteinsGate/Client/FallDownActivity_Goblin.h>
#include <SteinsGate/Client/SitActivity_Goblin.h>

void MonsterListener_DarkGoblin::onCreated() {
	AIComponent* pAIComponent = m_pMonster->getComponent<AIComponent>();

	if (!pAIComponent)
		return;

	pAIComponent->setWalkActivity(dbg_new WalkActivity_Goblin(m_pMonster));
	pAIComponent->setIdleActivity(dbg_new IdleActivity_Goblin(m_pMonster));
	pAIComponent->setAttackActivity(dbg_new AttackActivity_Goblin(m_pMonster));
	pAIComponent->setHitActivity(dbg_new HitActivity_Goblin(m_pMonster));
	pAIComponent->setFallDownActivity(dbg_new FallDownActivity_Goblin(m_pMonster));
	pAIComponent->setSitActivity(dbg_new SitActivity_Goblin(m_pMonster));
}