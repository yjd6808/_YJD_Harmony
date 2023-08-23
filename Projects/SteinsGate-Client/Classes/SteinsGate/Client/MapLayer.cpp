/*
 * 작성자 : 윤정도
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "MapLayer.h"

#include <SteinsGate/Common/RectEx.h>
#include <SteinsGate/Common/RectPoly.h>

#include <SteinsGate/Client/Define_Action.h>


USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;


// ==============================================================================================================================
// 
// ==============================================================================================================================

MapLayer* MapLayer::create() {
	MapLayer* pMap = dbg_new MapLayer;

	if (pMap && pMap->init()) {
		pMap->autorelease();
		return pMap;
	}

	JCORE_DELETE_SAFE(pMap);
	return nullptr;
}

MapLayer::MapLayer()
	: m_pMapInfo(nullptr)
	, m_pActorBox(nullptr)
{}

MapLayer::~MapLayer() {
	_LogDebug_("맵 레이어 소멸");
}

bool MapLayer::init() {

	if (!Layer::init()) {
		return false;
	}

	m_pActorBox = ActorManager::Get();
	m_pCamera = MimicCamera::create();
	this->addChild(m_pCamera);
	return true;
}



void MapLayer::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	HostPlayer::Get()->onKeyPressed(keyCode, event);
}

void MapLayer::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	HostPlayer::Get()->onKeyReleased(keyCode, event);
}

MapPhysicsInfo* MapLayer::getMapPhysicsInfo() const {
	return m_pMapPhysicsInfo;
}

MapAreaInfo* MapLayer::getMapAreaInfo() const {
	return m_pMapAreaInfo;
}

MapInfo* MapLayer::getMapInfo() const {
	return m_pMapInfo;
}

MimicCamera* MapLayer::getCamera() const {
	return m_pCamera;
}


void MapLayer::update(float dt) {
	m_pActorBox->update(dt);
	m_pCamera->update(dt);
}

void MapLayer::loadMap(int mapCode) {
	m_pMapInfo = Core::DataManager->getMapInfo(mapCode);
	m_pMapAreaInfo = Core::DataManager->getMapAreaInfo(mapCode);
	m_pMapPhysicsInfo = Core::DataManager->getMapPhysicsInfo(m_pMapInfo->PhysicsCode);

	// 배경 로딩

	// 타일 로딩, 맨 밑에 타일들부터 차곡차곡 쌓아서 올린다.
	for (int i = m_pMapInfo->TileHeight - 1, k = 0; i >= 0; --i, ++k) {
		for (int j = 0; j < m_pMapInfo->TileWidth; j++) {
			const float fTileXPos = Const::Map::TileWidth * j;
			const float fTileYPos = Const::Map::TileHeight * k;

			const TileInfo* pTileInfo = Core::DataManager->getTileInfo(m_pMapInfo->TileArray[i][j]);
			FrameTexture* pFrameTexture = Core::Contents.PackManager->getPack(pTileInfo->SgaIndex)->createFrameTexture(pTileInfo->ImgIndex, pTileInfo->SpriteIndex);

			SGSprite* pTileSprite = SGSprite::createWithTexture(pFrameTexture->getTexture());
			pTileSprite->setAnchorPoint(Vec2::ZERO);
			pTileSprite->setPosition(fTileXPos, fTileYPos);
			this->addChild(pTileSprite);
		}
	}

	// 오브젝트 로딩
	for (int i = 0; i < m_pMapInfo->MapObjectList.Size(); ++i) {
		MapObjectPositionInfo& objInfo = m_pMapInfo->MapObjectList[i];
		m_pActorBox->createMapObjectOnMap(objInfo.Code, objInfo.X, objInfo.Y);
	}

	// NPC 로딩

	// 몬스터 로딩
	m_pActorBox->createMonsterOnMap(2, 1, 600, 350);  // 테스트 몬스터 생성
	m_bMapLoaded = true;
}

Character* MapLayer::findNearestCharacterInRadious(Actor* stdActor, float radious, float& enemyDist) {

	int iAllyFlag = stdActor->getAllyFlag();
	SGVec2 stdPos = stdActor->getPositionRealCenter();
	Character* character = nullptr;
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
	enemyDist = minDist;
	return character;
}

bool MapLayer::collectEnemiesInActorRect(
	Actor* attacker,
	const ActorRect& absoluteActorRect,
	JCORE_OUT SGVector<HitInfo>& hitTargets) {

	ActorList& physcisActorList = m_pActorBox->getPhysicsActorList();
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


bool MapLayer::collectEnemiesInActor(Actor* collector, JCORE_OUT SGVector<HitInfo>& hitTargets) {
	return collectEnemiesInActorRect(collector, collector->getActorRect(), hitTargets);
}


bool MapLayer::isCollideWithMapObjects(const SGRect& rect) const {

	MapObjectList& collidableMapObjects = m_pActorBox->getCollidableMapObjectList();

	for (int i = 0; i < collidableMapObjects.Size(); ++i) {
		SGRect thicknessBox = collidableMapObjects[i]->getThicknessBoxRect();

		if (thicknessBox.intersectsRect(rect))
			return true;
	}

	return false;
}

