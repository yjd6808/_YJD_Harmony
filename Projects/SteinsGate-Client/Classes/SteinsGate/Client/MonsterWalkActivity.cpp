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

	MapLayer* pMapLayer = m_pMonster->getMapLayer();
	updateMove(dt, pMapLayer);
}

void MonsterWalkActivity::updateWander(float dt) {
	
}

void MonsterWalkActivity::updateTrack(float dt) {
	if (m_pTarget == nullptr)
		return;

	m_Destination =  m_pTarget->getPositionRealCenter();
}

void MonsterWalkActivity::updateMove(float dt, MapLayer* pMapLayer) {
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

	MonsterStatInfo* pStatInfo = m_pMonster->getStatInfo();

	if (!pStatInfo)
		return;

	MapAreaInfo* pAreaInfo = pMapLayer->getMapAreaInfo();

	

	float fSpeedX = pStatInfo->MoveSpeedX * FPS1_v;

	if (!bXFinished && lr == Direction::Left) {
		thicknessPosLR.origin.x -= fSpeedX;
		updateLeftMove(pMapLayer, pAreaInfo, thicknessPosLR);
	} else if (!bXFinished && lr == Direction::Right && !bXFinished) {
		thicknessPosLR.origin.x += fSpeedX;
		updateRightMove(pMapLayer, pAreaInfo, thicknessPosLR);
	}

	float fSpeedY = pStatInfo->MoveSpeedY / 60.0f;

	if (!bYFinished && ud == Direction::Up) {
		thicknessPosUD.origin.y += fSpeedY;
		updateUpMove(pMapLayer, pAreaInfo, thicknessPosUD);
	} else if (!bYFinished && ud == Direction::Down) {
		thicknessPosUD.origin.y -= fSpeedY;
		updateDownMove(pMapLayer, pAreaInfo, thicknessPosUD);
	}
}

void MonsterWalkActivity::updateLeftMove(MapLayer* mapLayer, MapAreaInfo* areaInfo, const SGRect& thicknessRect) {
	SGVec2 lb{ thicknessRect.origin.x, thicknessRect.origin.y };
	SGVec2 lt{ thicknessRect.origin.x, thicknessRect.origin.y + thicknessRect.size.height };

	// lb, lt 체크
	if (areaInfo->checkWall(lb.x, lb.y) || areaInfo->checkWall(lt.x, lt.y) || mapLayer->isCollideWithMapObjects(thicknessRect))
		return;

	m_pMonster->setPositionRealX(thicknessRect.origin.x);
}


void MonsterWalkActivity::updateRightMove(MapLayer* mapLayer, MapAreaInfo* areaInfo, const SGRect& thicknessRect) {
	SGVec2 rb{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y };
	SGVec2 rt{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y + thicknessRect.size.height };

	// rb, rt 체크
	if (areaInfo->checkWall(rb.x, rb.y) || areaInfo->checkWall(rt.x, rt.y) || mapLayer->isCollideWithMapObjects(thicknessRect))
		return;

	m_pMonster->setPositionRealX(thicknessRect.origin.x);
}

void MonsterWalkActivity::updateUpMove(MapLayer* mapLayer, MapAreaInfo* areaInfo, const SGRect& thicknessRect) {
	SGVec2 lt{ thicknessRect.origin.x, thicknessRect.origin.y + thicknessRect.size.height };
	SGVec2 rt{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y + thicknessRect.size.height };

	// lt, rt 체크
	if (areaInfo->checkWall(lt.x, lt.y) || areaInfo->checkWall(rt.x, rt.y) || mapLayer->isCollideWithMapObjects(thicknessRect))
		return;

	m_pMonster->setPositionRealY(thicknessRect.origin.y);
}

void MonsterWalkActivity::updateDownMove(MapLayer* mapLayer, MapAreaInfo* areaInfo, const SGRect& thicknessRect) {
	SGVec2 lb{ thicknessRect.origin.x, thicknessRect.origin.y };
	SGVec2 rb{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y };

	// lb, rb 체크
	if (areaInfo->checkWall(lb.x, lb.y) || areaInfo->checkWall(rb.x, rb.y) || mapLayer->isCollideWithMapObjects(thicknessRect))
		return;

	m_pMonster->setPositionRealY(thicknessRect.origin.y);
}



void MonsterWalkActivity::setDestination(const SGVec2& destination) {
	m_Destination = destination;
}





