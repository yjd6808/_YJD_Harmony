/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "WalkActivity_Goblin.h"

#include "sgcl/Monster.h"
#include "sgcl/SGVec2Ex.h"
#include "sgcl/Define_Animation.h"
#include "sgcl/MoveComponent.h"
#include "sgcl/AIComponent.h"

////////////////////////////////////////////////////////////////////////////////////////
WalkActivity_Goblin::WalkActivity_Goblin(Actor* _pActor)
: WalkActivity(_pActor)
{
}

////////////////////////////////////////////////////////////////////////////////////////

void WalkActivity_Goblin::OnActivityBegin()
{
	pActor_->RunAnimation(DEF_ANIMATION_MONSTER_GOBLIN_WALK);
}

////////////////////////////////////////////////////////////////////////////////////////

void WalkActivity_Goblin::UpdateMoveImpl(float _dt, MoveComponent* _pMoveComponent, bool _arrivedX, bool _arrivedY,
                                         Direction_t _lr, Direction_t _ud)
{
	Monster* pMonster = dynamic_cast<Monster*>(pActor_);

	if (pMonster == nullptr)
	{
		jc_assert(pMonster);
		return;
	}

	MonsterStatInfo* pStatInfo = pMonster->GetStatInfo();

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

	_pMoveComponent->SetSpeed(speedX, speedY);
}
