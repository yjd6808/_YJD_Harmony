/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "WalkActivity_Goblin.h"

#include <SteinsGate/Client/Monster.h>
#include <SteinsGate/Client/SGVec2Ex.h>
#include <SteinsGate/Client/Define_Animation.h>
#include <SteinsGate/Client/MoveComponent.h>
#include <SteinsGate/Client/AIComponent.h>

////////////////////////////////////////////////////////////////////////////////////////
WalkActivity_Goblin::WalkActivity_Goblin(Actor* _pActor)
: WalkActivity(_pActor)
{
}

////////////////////////////////////////////////////////////////////////////////////////

void WalkActivity_Goblin::OnActivityBegin()
{
	pActor_->runAnimation(DEF_ANIMATION_MONSTER_GOBLIN_WALK);
}

////////////////////////////////////////////////////////////////////////////////////////

void WalkActivity_Goblin::updateMoveImpl(float _dt, MoveComponent* _pMoveComponent, bool _arrivedX, bool _arrivedY,
                                         Direction_t _lr, Direction_t _ud)
{
	Monster* pMonster = dynamic_cast<Monster*>(pActor_);

	if (pMonster == nullptr)
	{
		DebugAssert(pMonster);
		return;
	}

	MonsterStatInfo* pStatInfo = pMonster->getStatInfo();

	if (pStatInfo == nullptr)
	{
		return;
	}

	float speedX = 0;
	float speedY = 0;

	if (!_arrivedX && _lr == Direction::Left)
	{
		speedX = pStatInfo->moveSpeedX_ * _dt * -1;
	}
	else if (!_arrivedX && _lr == Direction::Right)
	{
		speedX = pStatInfo->moveSpeedX_ * _dt;
	}

	if (!_arrivedY && _ud == Direction::Up)
	{
		speedY = pStatInfo->moveSpeedY_ * _dt;
	}
	else if (!_arrivedY && _ud == Direction::Down)
	{
		speedY = pStatInfo->moveSpeedY_ * _dt * -1;
	}

	_pMoveComponent->setSpeed(speedX, speedY);
}
