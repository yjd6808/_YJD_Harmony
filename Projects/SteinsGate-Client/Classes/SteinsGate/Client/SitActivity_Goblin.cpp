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

//////////////////////////////////////////////////////////////////////////////////////////
SitActivity_Goblin::SitActivity_Goblin(Actor* _pActor)
: SitActivity(_pActor)
{
}

void SitActivity_Goblin::OnActivityBegin()
{
	Monster* pMonster = dynamic_cast<Monster*>(pActor_);
	DebugAssert(pMonster);

	const MonsterStatInfo* pStatInfo = pMonster->GetStatInfo();
	const float limitTime = pStatInfo ? pStatInfo->downRecoverTime_ / 2 : 1.0f;

	limitTime_ = limitTime;
	pActor_->RunAnimation(DEF_ANIMATION_MONSTER_GOBLIN_SIT);
}
