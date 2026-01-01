/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "MonsterListener_DarkGoblin.h"

#include <sgcl/AIComponent.h>

#include <sgcl/WalkActivity_Goblin.h>
#include <sgcl/IdleActivity_Goblin.h>
#include <sgcl/AttackActivity_Goblin.h>
#include <sgcl/HitActivity_Goblin.h>
#include <sgcl/FallDownActivity_Goblin.h>
#include <sgcl/SitActivity_Goblin.h>

//////////////////////////////////////////////////////////////////////////////////////////
void MonsterListener_DarkGoblin::OnCreated()
{
	AIComponent* pAiComponent = m_pMonster->GetComponent<AIComponent>();

	if (!pAiComponent)
		return;

	pAiComponent->SetWalkActivity(dbg_new WalkActivity_Goblin(m_pMonster));
	pAiComponent->SetIdleActivity(dbg_new IdleActivity_Goblin(m_pMonster));
	pAiComponent->SetAttackActivity(dbg_new AttackActivity_Goblin(m_pMonster));
	pAiComponent->SetHitActivity(dbg_new HitActivity_Goblin(m_pMonster));
	pAiComponent->SetFallDownActivity(dbg_new FallDownActivity_Goblin(m_pMonster));
	pAiComponent->SetSitActivity(dbg_new SitActivity_Goblin(m_pMonster));
}
