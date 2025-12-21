/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "MonsterListener_NormalGoblin.h"

#include <SteinsGate/Client/AIComponent.h>

#include <SteinsGate/Client/WalkActivity_Goblin.h>
#include <SteinsGate/Client/IdleActivity_Goblin.h>
#include <SteinsGate/Client/AttackActivity_Goblin.h>
#include <SteinsGate/Client/HitActivity_Goblin.h>
#include <SteinsGate/Client/FallDownActivity_Goblin.h>
#include <SteinsGate/Client/SitActivity_Goblin.h>

//////////////////////////////////////////////////////////////////////////////////////////
void MonsterListener_NormalGoblin::OnCreated()
{
	AIComponent* pAIComponent = m_pMonster->GetComponent<AIComponent>();

	if (!pAIComponent)
		return;

	pAIComponent->SetWalkActivity(dbg_new WalkActivity_Goblin(m_pMonster));
	pAIComponent->SetIdleActivity(dbg_new IdleActivity_Goblin(m_pMonster));
	pAIComponent->SetAttackActivity(dbg_new AttackActivity_Goblin(m_pMonster));
	pAIComponent->SetHitActivity(dbg_new HitActivity_Goblin(m_pMonster));
	pAIComponent->SetFallDownActivity(dbg_new FallDownActivity_Goblin(m_pMonster));
	pAIComponent->SetSitActivity(dbg_new SitActivity_Goblin(m_pMonster));
}
