/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 4:24:01 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "MoveComponent.h"

#include <SteinsGate/Client/MapLayer.h>

USING_NS_JC;
USING_NS_CC;

MoveComponent::MoveComponent(Actor* actor)
	: ActorComponent(actor)
{}

void MoveComponent::onUpdate(float dt) {
	MapLayer* pMapLayer = m_pActor->getMapLayer();
	if (pMapLayer == nullptr) {
		return;
	}

	MapAreaInfo* pAreaInfo = pMapLayer->getMapAreaInfo();
	if (pAreaInfo == nullptr) {
		return;
	}

	// 좌,우 | 위,아래 독립적으로 계산해야함
	// 한개로만 하게되면 예를들어 Left, Down을 동시에 눌렀을 때
	// thickness.origin.x -= fSpeedX 적용된 값이 Down 체크시에도 적용되어버려서
	// 이 적용된 값 때문에 Down에서 lb, rb 충돌 체크가 항상 참이 되어버림
	// --------------------------------------------------------------
	//  23/01/28 -> 좌,우,위,아래 모두 독립적으로 가능하도록 추가
	SGRect thicknessPosLR = m_pActor->getThicknessBoxRect();
	SGRect thicknessPosUD = thicknessPosLR;

	thicknessPosLR.origin.x += m_Speed.x;
	thicknessPosUD.origin.y += m_Speed.y;

	if (m_Speed.x < 0) {
		updateLeftMove(pMapLayer, pAreaInfo, thicknessPosLR);
	} else {
		updateRightMove(pMapLayer, pAreaInfo, thicknessPosLR);
	}

	if (m_Speed.y < 0) {
		updateDownMove(pMapLayer, pAreaInfo, thicknessPosUD);
	} else {
		updateUpMove(pMapLayer, pAreaInfo, thicknessPosUD);
	}
}

void MoveComponent::updateLeftMove(MapLayer* mapLayer, MapAreaInfo* areaInfo, const SGRect& thicknessRect) {
	SGVec2 lb{ thicknessRect.origin.x, thicknessRect.origin.y };
	SGVec2 lt{ thicknessRect.origin.x, thicknessRect.origin.y + thicknessRect.size.height };

	// lb, lt 체크
	if (areaInfo->checkWall(lb.x, lb.y) || areaInfo->checkWall(lt.x, lt.y) || mapLayer->isCollideWithMapObjects(thicknessRect))
		return;

	m_pActor->setPositionRealX(thicknessRect.origin.x);
}


void MoveComponent::updateRightMove(MapLayer* mapLayer, MapAreaInfo* areaInfo, const SGRect& thicknessRect) {
	SGVec2 rb{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y };
	SGVec2 rt{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y + thicknessRect.size.height };

	// rb, rt 체크
	if (areaInfo->checkWall(rb.x, rb.y) || areaInfo->checkWall(rt.x, rt.y) || mapLayer->isCollideWithMapObjects(thicknessRect))
		return;

	m_pActor->setPositionRealX(thicknessRect.origin.x);
}

void MoveComponent::updateUpMove(MapLayer* mapLayer, MapAreaInfo* areaInfo, const SGRect& thicknessRect) {
	SGVec2 lt{ thicknessRect.origin.x, thicknessRect.origin.y + thicknessRect.size.height };
	SGVec2 rt{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y + thicknessRect.size.height };

	// lt, rt 체크
	if (areaInfo->checkWall(lt.x, lt.y) || areaInfo->checkWall(rt.x, rt.y) || mapLayer->isCollideWithMapObjects(thicknessRect))
		return;

	m_pActor->setPositionRealY(thicknessRect.origin.y);
}

void MoveComponent::updateDownMove(MapLayer* mapLayer, MapAreaInfo* areaInfo, const SGRect& thicknessRect) {
	SGVec2 lb{ thicknessRect.origin.x, thicknessRect.origin.y };
	SGVec2 rb{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y };

	// lb, rb 체크
	if (areaInfo->checkWall(lb.x, lb.y) || areaInfo->checkWall(rb.x, rb.y) || mapLayer->isCollideWithMapObjects(thicknessRect))
		return;

	m_pActor->setPositionRealY(thicknessRect.origin.y);
}

void MoveComponent::setSpeed(const SGVec2& speed) {
	m_Speed = speed;
}

void MoveComponent::setSpeed(float x, float y) {
	m_Speed.x = x;
	m_Speed.y = y;
}

const SGVec2& MoveComponent::getSpeed() const {
	return m_Speed;
}
