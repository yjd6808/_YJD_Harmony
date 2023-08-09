/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "MonsterWalkActivity.h"

#include <SteinsGate/Client/Monster.h>
#include <SteinsGate/Client/AnimationDefine.h>
#include <SteinsGate/Client/MapLayer.h>
#include <SteinsGate/Client/SGVec2Ex.h>
#include <SteinsGate/Client/MoveComponent.h>

#define DestinationMinDistX 7.0f	// 목표지점까지 거리가 이정도 이내이면 도착했다고 판정
#define DestinationMinDistY 5.0f

MonsterWalkActivity::MonsterWalkActivity(Monster* monster)
	: MonsterActivity(monster, AIActivityType::Walk)
	, m_pTarget(nullptr) {}

void MonsterWalkActivity::onActivityBegin() {
	m_pMonster->runAnimation(MONSTER_ANIMATION_WALK);
}

void MonsterWalkActivity::setTarget(Actor* target) {
	m_pTarget = target;
}

void MonsterWalkActivity::onUpdate(float dt) {
	updateLimitTime(dt);

	if (!isRunning())
		return;

	switch (m_eMode) {
		case eWander: updateWander(dt); break;
		case eTrack: updateTrack(dt); break;
		default: DebugAssertMsg(false, "몬스터 Walking 액티비티 모드가 이상합니다.");
	}

	updateMove(dt);
}

void MonsterWalkActivity::updateWander(float dt) {
	
}

void MonsterWalkActivity::updateTrack(float dt) {
	if (m_pTarget == nullptr)
		return;

	m_Destination =  m_pTarget->getPositionRealCenter();
}

void MonsterWalkActivity::updateMove(float dt) {
	Direction_t lr;
	Direction_t ud;
	SGRect thicknessPosLR = m_pMonster->getThicknessBoxRect();
	SGRect thicknessPosUD = thicknessPosLR;
	SGVec2 center = thicknessPosLR.getMid();

	SGVec2Ex::getLookDirection(center, m_Destination, lr, ud);

	bool bXFinished = false;
	bool bYFinished = false;

	if (SGMath::Abs(center.x - m_Destination.x) < DestinationMinDistX)
		bXFinished = true;

	if (SGMath::Abs(center.y - m_Destination.y) < DestinationMinDistY)
		bYFinished = true;

	MoveComponent* pMoveComponent = m_pMonster->getComponent<MoveComponent>();
	MonsterStatInfo* pStatInfo = m_pMonster->getStatInfo();

	if (bXFinished && bYFinished) {

		if (pMoveComponent) 
			pMoveComponent->setSpeed(0, 0);

		stop();
		return;
	}

	if (!pStatInfo || !pMoveComponent)
		return;

	// 디버깅 테스트
	// static MonsterStatInfo* pStatInfo = dbg_new MonsterStatInfo;
	// 
	// pStatInfo->MoveSpeedX = 150;
	// pStatInfo->MoveSpeedY = 150;
	
	float fSpeedX = 0;
	float fSpeedY = 0;

	if (!bXFinished && lr == Direction::Left) {
		fSpeedX = pStatInfo->MoveSpeedX * dt * -1;
	} else if (!bXFinished && lr == Direction::Right) {
		fSpeedX = pStatInfo->MoveSpeedX * dt;
	}

	if (!bYFinished && ud == Direction::Up) {
		fSpeedY = pStatInfo->MoveSpeedY * dt;
	} else if (!bYFinished && ud == Direction::Down) {
		fSpeedY = pStatInfo->MoveSpeedY * dt * -1;
	}

	pMoveComponent->setSpeed(fSpeedX, fSpeedY);
}

void MonsterWalkActivity::setDestination(const SGVec2& destination) {
	m_Destination = destination;
}





