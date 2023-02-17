/*
 * 작성자 : 윤정도
 */

#include "Tutturu.h"
#include "SGMapLayer.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGGlobal.h>
#include <SteinsGate/Client/SGActionDefine.h>
#include <SteinsGate/Client/SGHostPlayer.h>

#include <SteinsGate/Common/SGRectEx.h>
#include <SteinsGate/Common/RectPoly.h>

#include "SGGameScene.h"


USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;


// ==============================================================================================================================
// 
// ==============================================================================================================================

SGMapLayer* SGMapLayer::create() {
	SGMapLayer* pMap = dbg_new SGMapLayer;

	if (pMap && pMap->init()) {
		pMap->autorelease();
		return pMap;
	}

	DeleteSafe(pMap);
	return nullptr;
}

SGMapLayer::SGMapLayer()
	: m_pMapInfo(nullptr)
	, m_pActorBox(nullptr)
{}

SGMapLayer::~SGMapLayer() {
}

bool SGMapLayer::init() {

	if (!Layer::init()) {
		return false;
	}

	m_pActorBox = SGActorBox::get();
	return true;
}



void SGMapLayer::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	SGHostPlayer::get()->onKeyPressed(keyCode, event);
}

void SGMapLayer::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	SGHostPlayer::get()->onKeyReleased(keyCode, event);
}

SGMapInfo* SGMapLayer::getMapInfo() {
	return m_pMapInfo;
}


void SGMapLayer::update(float dt) {
	SGHostPlayer::get()->update(dt);

	m_pActorBox->update(dt);
}



void SGMapLayer::loadMap(int mapCode) {
	SGDataManager* pDataManager = SGDataManager::get();
	SGImagePackManager* pPackManager = SGImagePackManager::get();
	SGMapInfo* pMap = pDataManager->getMapInfo(mapCode);

	m_pMapInfo = pMap;

	// 배경 로딩

	// 타일 로딩, 맨 밑에 타일들부터 차곡차곡 쌓아서 올린다.
	for (int i = pMap->TileHeight - 1, k = 0; i >= 0; --i, ++k) {	
		for (int j = 0; j < pMap->TileWidth; j++) {
			float fTileXPos = float(TileWidth_v) * j;
			float fTileYPos = float(TileHeight_v) * k;

			SGTileInfo* pTileInfo = pDataManager->getTileInfo(pMap->TileArray[i][j]);
			SGFrameTexture* pFrameTexture = pPackManager->getPack(pTileInfo->NpkIndex)->createFrameTexture(pTileInfo->ImgIndex, pTileInfo->SpriteIndex);

			SGSprite* pTileSprite = SGSprite::createWithTexture(pFrameTexture->getTexture());
			pTileSprite->setAnchorPoint(Vec2::ZERO);
			pTileSprite->setPosition(fTileXPos, fTileYPos);
			this->addChild(pTileSprite);
		}
	}

	// 오브젝트 로딩
	for (int i = 0; i < pMap->ObstacleList.Size(); ++i) {
		SGMapObjectInfo& objInfo = pMap->ObstacleList[i];
		m_pActorBox->createObstacleOnMap(objInfo.Code, objInfo.X, objInfo.Y);
	}

	// NPC 로딩

	// 몬스터 로딩
	m_pActorBox->createMonsterOnMap(2, 1, 600, 350);  // 테스트 몬스터 생성
	m_bMapLoaded = true;
}


void SGMapLayer::runFrameEvent(SGActor* runner, FrameEventType_t frameEventType, int frameEventId) {

	if (frameEventType == FrameEventType::Projectile)
		m_pActorBox->createProejctileOnMap(runner, frameEventId);
	else if (frameEventType == FrameEventType::AttackBox) {
		// TODO: 어택박스 구현
	}
}


SGCharacter* SGMapLayer::findNearestCharacterInRadious(SGActor* stdActor, float radious, float& enemyDist) {

	int iAllyFlag = stdActor->getAllyFlag();
	SGVec2 stdPos = stdActor->getPositionRealCenter();
	SGCharacter* character = nullptr;
	float minDist = FLT_MAX;
	CharacterList& characterList = m_pActorBox->getCharacterList();

	for (int i = 0; i < characterList.Size(); ++i) {
		auto pCharacter = characterList[i];

		if (pCharacter == stdActor)
			continue;

		float dist = stdPos.distance(pCharacter->getPositionRealCenter());

		if (dist < radious && pCharacter->getAllyFlag() != iAllyFlag && dist < minDist) {
			minDist = dist;
			character = pCharacter;
		}
	}
	return character;
}

bool SGMapLayer::collectEnemiesInActorRect(
	SGActor* attacker,
	const SGActorRect& absoluteActorRect,
	Out_ SGVector<SGHitInfo>& hitTargets) {

	PhysicsActorList& physcisActorList = m_pActorBox->getPhysicsActorList();
	bool bFind = false;

	for (int i = 0; i < physcisActorList.Size(); ++i) {
		auto pHitTarget = physcisActorList[i];	// 공격받을 대상
		SGRect hitRect;
		SpriteDirection_t eHitDirection;

		if (pHitTarget->getAllyFlag() == attacker->getAllyFlag())
			continue;

		// 몬스터 기준으로 플레이어 충돌이라
		// eHitDirection은 플레이어의 충돌방향이 되므로, 반대로 돌려줘야함.
		if (pHitTarget->isCollide(absoluteActorRect, eHitDirection, hitRect)) {
			hitTargets.PushBack({ attacker, pHitTarget, SpriteDirection::Reverse[eHitDirection], hitRect, nullptr });
			bFind = true;
		}
	}

	return bFind;
}


bool SGMapLayer::collectEnemiesInActor(SGActor* collector, Out_ SGVector<SGHitInfo>& hitTargets) {
	return collectEnemiesInActorRect(collector, collector->getActorRect(), hitTargets);
}


bool SGMapLayer::isCollideWithObstacles(const SGRect& rect) {

	ObstacleList& collidableObstacles = m_pActorBox->getCollidableObstacleList();

	for (int i = 0; i < collidableObstacles.Size(); ++i) {
		SGRect thicknessBox = collidableObstacles[i]->getThicknessBoxRect();

		if (thicknessBox.intersectsRect(rect))
			return true;
	}

	return false;
}

