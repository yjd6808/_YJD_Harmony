/*
 * 작성자 : 윤정도
 */

#include "Tutturu.h"
#include "MapLayer.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/Global.h>
#include <SteinsGate/Client/ActionDefine.h>
#include <SteinsGate/Client/HostPlayer.h>

#include <SteinsGate/Common/RectEx.h>
#include <SteinsGate/Common/RectPoly.h>

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

	DeleteSafe(pMap);
	return nullptr;
}

MapLayer::MapLayer()
	: m_pMapInfo(nullptr)
	, m_pActorBox(nullptr)
{}

MapLayer::~MapLayer() {
}

bool MapLayer::init() {

	if (!Layer::init()) {
		return false;
	}

	m_pActorBox = ActorBox::get();
	m_pCamera = MimicCamera::create();
	this->addChild(m_pCamera);
	return true;
}



void MapLayer::onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	HostPlayer::get()->onKeyPressed(keyCode, event);
}

void MapLayer::onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) {
	HostPlayer::get()->onKeyReleased(keyCode, event);
}

MapInfo* MapLayer::getMapInfo() {
	return m_pMapInfo;
}

MimicCamera* MapLayer::getCamera() {
	return m_pCamera;
}


void MapLayer::update(float dt) {
	m_pActorBox->update(dt);
	m_pCamera->update(dt);
}

void MapLayer::loadMap(int mapCode) {
	DataManager* pDataManager = DataManager::get();
	ImagePackManager* pPackManager = ImagePackManager::get();
	MapInfo* pMap = pDataManager->getMapInfo(mapCode);

	m_pMapInfo = pMap;

	// 배경 로딩

	// 타일 로딩, 맨 밑에 타일들부터 차곡차곡 쌓아서 올린다.
	for (int i = pMap->TileHeight - 1, k = 0; i >= 0; --i, ++k) {	
		for (int j = 0; j < pMap->TileWidth; j++) {
			float fTileXPos = float(TileWidth_v) * j;
			float fTileYPos = float(TileHeight_v) * k;

			TileInfo* pTileInfo = pDataManager->getTileInfo(pMap->TileArray[i][j]);
			FrameTexture* pFrameTexture = pPackManager->getPack(pTileInfo->SgaIndex)->createFrameTexture(pTileInfo->ImgIndex, pTileInfo->SpriteIndex);

			SGSprite* pTileSprite = SGSprite::createWithTexture(pFrameTexture->getTexture());
			pTileSprite->setAnchorPoint(Vec2::ZERO);
			pTileSprite->setPosition(fTileXPos, fTileYPos);
			this->addChild(pTileSprite);
		}
	}

	// 오브젝트 로딩
	for (int i = 0; i < pMap->ObstacleList.Size(); ++i) {
		MapObjectInfo& objInfo = pMap->ObstacleList[i];
		m_pActorBox->createObstacleOnMap(objInfo.Code, objInfo.X, objInfo.Y);
	}

	// NPC 로딩

	// 몬스터 로딩
	m_pActorBox->createMonsterOnMap(2, 1, 600, 350);  // 테스트 몬스터 생성
	m_bMapLoaded = true;
}


void MapLayer::runFrameEvent(Actor* runner, FrameEventType_t frameEventType, int frameEventId) {

	if (frameEventType == FrameEventType::Projectile)
		m_pActorBox->createProejctileOnMap(runner, frameEventId);
	else if (frameEventType == FrameEventType::AttackBox) {
		// TODO: 어택박스 구현
	}
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
	return character;
}

bool MapLayer::collectEnemiesInActorRect(
	Actor* attacker,
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


bool MapLayer::collectEnemiesInActor(Actor* collector, Out_ SGVector<SGHitInfo>& hitTargets) {
	return collectEnemiesInActorRect(collector, collector->getActorRect(), hitTargets);
}


bool MapLayer::isCollideWithObstacles(const SGRect& rect) {

	ObstacleList& collidableObstacles = m_pActorBox->getCollidableObstacleList();

	for (int i = 0; i < collidableObstacles.Size(); ++i) {
		SGRect thicknessBox = collidableObstacles[i]->getThicknessBoxRect();

		if (thicknessBox.intersectsRect(rect))
			return true;
	}

	return false;
}

