/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGMonsterWalkActivity.h"

#include <SteinsGate/Research/SGMonster.h>
#include <SteinsGate/Research/SGAnimationDefine.h>
#include <SteinsGate/Research/SGMapLayer.h>
#include <SteinsGate/Research/SGVec2Ex.h>

#define DestinationMinDistX 7.0f	// 목표지점까지 거리가 이정도 이내이면 도착했다고 판정
#define DestinationMinDistY 5.0f

SGMonsterWalkActivity::SGMonsterWalkActivity(SGMonster* monster)
	: SGMonsterActivity(monster, AIActivity::Walk)
	, m_pTarget(nullptr) {}

void SGMonsterWalkActivity::onActivityBegin() {
	m_pMonster->runAnimation(MONSTER_ANIMATION_WALK);
}

void SGMonsterWalkActivity::setTarget(SGActor* target) {
	m_pTarget = target;
}

void SGMonsterWalkActivity::onUpdate(float dt) {
	updateLimitTime(dt);

	if (!isRunning())
		return;

	switch (m_eMode) {
		case eWander: updateWander(dt); break;
		case eTrack: updateTrack(dt); break;
		default: DebugAssertMessage(false, "몬스터 Walking 액티비티 모드가 이상합니다.");
	}

	SGMapLayer* pMapLayer = m_pMonster->getMapLayer();
	updateMove(dt, pMapLayer);
}

void SGMonsterWalkActivity::updateWander(float dt) {
	
}

void SGMonsterWalkActivity::updateTrack(float dt) {
	if (m_pTarget == nullptr)
		return;

	m_Destination =  m_pTarget->getPositionRealCenter();
}

void SGMonsterWalkActivity::updateMove(float dt, SGMapLayer* pMapLayer) {
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

	if (bXFinished && bYFinished) {
		stop();
		return;
	}

	SGMonsterInfo* pMonsterInfo = m_pMonster->getBaseInfo();
	SGMapInfo* pMapInfo = pMapLayer->getMapInfo();

	float fSpeedX = pMonsterInfo->MoveSpeedX * FPS1_v;

	if (!bXFinished && lr == Direction::Left) {
		thicknessPosLR.origin.x -= fSpeedX;
		updateLeftMove(pMapLayer, pMapInfo, thicknessPosLR);
	} else if (!bXFinished && lr == Direction::Right && !bXFinished) {
		thicknessPosLR.origin.x += fSpeedX;
		updateRightMove(pMapLayer, pMapInfo, thicknessPosLR);
	}

	float fSpeedY = pMonsterInfo->MoveSpeedY / 60.0f;

	if (!bYFinished && ud == Direction::Up) {
		thicknessPosUD.origin.y += fSpeedY;
		updateUpMove(pMapLayer, pMapInfo, thicknessPosUD);
	} else if (!bYFinished && ud == Direction::Down) {
		thicknessPosUD.origin.y -= fSpeedY;
		updateDownMove(pMapLayer, pMapInfo, thicknessPosUD);
	}

	
}

void SGMonsterWalkActivity::updateLeftMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& thicknessRect) {
	SGVec2 lb{ thicknessRect.origin.x, thicknessRect.origin.y };
	SGVec2 lt{ thicknessRect.origin.x, thicknessRect.origin.y + thicknessRect.size.height };

	// lb, lt 체크
	if (mapInfo->checkWall(lb) || mapInfo->checkWall(lt) || mapLayer->isCollideWithObstacles(thicknessRect))
		return;

	m_pMonster->setPositionRealX(thicknessRect.origin.x);
}


void SGMonsterWalkActivity::updateRightMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& thicknessRect) {
	SGVec2 rb{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y };
	SGVec2 rt{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y + thicknessRect.size.height };

	// rb, rt 체크
	if (mapInfo->checkWall(rb) || mapInfo->checkWall(rt) || mapLayer->isCollideWithObstacles(thicknessRect))
		return;

	m_pMonster->setPositionRealX(thicknessRect.origin.x);
}

void SGMonsterWalkActivity::updateUpMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& thicknessRect) {
	SGVec2 lt{ thicknessRect.origin.x, thicknessRect.origin.y + thicknessRect.size.height };
	SGVec2 rt{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y + thicknessRect.size.height };

	// lt, rt 체크
	if (mapInfo->checkWall(lt) || mapInfo->checkWall(rt) || mapLayer->isCollideWithObstacles(thicknessRect))
		return;

	m_pMonster->setPositionRealY(thicknessRect.origin.y);
}

void SGMonsterWalkActivity::updateDownMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& thicknessRect) {
	SGVec2 lb{ thicknessRect.origin.x, thicknessRect.origin.y };
	SGVec2 rb{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y };

	// lb, rb 체크
	if (mapInfo->checkWall(lb) || mapInfo->checkWall(rb) || mapLayer->isCollideWithObstacles(thicknessRect))
		return;

	m_pMonster->setPositionRealY(thicknessRect.origin.y);
}



void SGMonsterWalkActivity::setDestination(const SGVec2& destination) {
	m_Destination = destination;
}





