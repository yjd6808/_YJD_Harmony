/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 10:17:26 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "SitActivity_Goblin.h"

#include <SteinsGate/Client/Monster.h>
#include <SteinsGate/Client/Define_Animation.h>

SitActivity_Goblin::SitActivity_Goblin(Actor* actor)
	: SitActivity(actor)
{}

void SitActivity_Goblin::onActivityBegin() {
	Monster* pMonster = dynamic_cast<Monster*>(m_pActor);
	DebugAssert(pMonster);

	const MonsterStatInfo* pStatInfo = pMonster->getStatInfo();
	const float fLimitTime = pStatInfo ? pStatInfo->DownRecoverTime / 2 : 1.0f;

	m_fLimitTime = fLimitTime;
	m_pActor->runAnimation(MONSTER_ANIMATION_GOBLIN_SIT);
}
