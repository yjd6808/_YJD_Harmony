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

WalkActivity_Goblin::WalkActivity_Goblin(Actor* actor)
	: WalkActivity(actor)
{}

void WalkActivity_Goblin::onActivityBegin() {
	m_pActor->runAnimation(MONSTER_ANIMATION_GOBLIN_WALK);
}

void WalkActivity_Goblin::updateMoveImpl(float dt, MoveComponent* moveComponent, bool arrivedX, bool arrivedY, Direction_t lr, Direction_t ud) {

	Monster* pMonster = dynamic_cast<Monster*>(m_pActor);

	if (pMonster == nullptr) {
		DebugAssert(pMonster);
		return;
	}

	MonsterStatInfo* pStatInfo = pMonster->getStatInfo();

	if (pStatInfo == nullptr)
		return;

	float fSpeedX = 0;
	float fSpeedY = 0;

	if (!arrivedX && lr == Direction::Left) {
		fSpeedX = pStatInfo->MoveSpeedX * dt * -1;
	} else if (!arrivedX && lr == Direction::Right) {
		fSpeedX = pStatInfo->MoveSpeedX * dt;
	}

	if (!arrivedY && ud == Direction::Up) {
		fSpeedY = pStatInfo->MoveSpeedY * dt;
	} else if (!arrivedY && ud == Direction::Down) {
		fSpeedY = pStatInfo->MoveSpeedY * dt * -1;
	}

	moveComponent->setSpeed(fSpeedX, fSpeedY);
}
	