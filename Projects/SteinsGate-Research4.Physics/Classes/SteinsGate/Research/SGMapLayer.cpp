/*
 * 작성자 : 윤정도
 */

#include "SGMapLayer.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGGlobal.h>

#include <SteinsGate/Common/Engine/SGRectEx.h>


USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;


// ==============================================================================================================================
// 
// ==============================================================================================================================

SGMapLayer* SGMapLayer::create(int mapCode) {
	SGMapLayer* pMap = new (std::nothrow) SGMapLayer;

	if (pMap && pMap->init()) {
		pMap->loadMap(mapCode);
		pMap->autorelease();
		return pMap;
	}

	DeleteSafe(pMap);
	return nullptr;
}



SGMapLayer::SGMapLayer()
	: m_fZReorderTime(0.0f)
	, m_pMapInfo(nullptr)
	, m_pPlayer(nullptr)
{}

SGMapLayer::~SGMapLayer() {
	DeleteSafe(m_pPlayer); // 임시
}

// 임시
SGPlayer* MainPlayer_v = nullptr;


bool SGMapLayer::init() {

	if (!Layer::init()) {
		return false;
	}

	// =================================================
	// 임시 데이터 주입
	// =================================================

	SGCharacterInfo info;
	SGDataManager* pConfig = SGDataManager::getInstance();
	SGCharacterBaseInfo* pBaseInfo = pConfig->getCharacterBaseInfo(CharacterType::Gunner);
	
	for (int i = 0; i < VisualType::Max; ++i) {
		info.VisualInfo.NpkIndex[i] = pBaseInfo->DefaultVisualNpkIndex[i];
		info.VisualInfo.ImgIndex[i] = pBaseInfo->DefaultVisualImgIndex[i];
	}

	SGActionInfo* pIdle = pConfig->getActionInfo("idle");
	SGActionInfo* pWalk = pConfig->getActionInfo("walk");
	SGActionInfo* pRun = pConfig->getActionInfo("run");
	SGActionInfo* pSliding = pConfig->getActionInfo("sliding");
	SGActionInfo* pGunShot = pConfig->getActionInfo("gun_shot");
	SGActionInfo* pJump = pConfig->getActionInfo("jump");

	info.ValidAction.PushBack(pIdle->Code);
	info.ValidAction.PushBack(pWalk->Code);
	info.ValidAction.PushBack(pRun->Code);
	info.ValidAction.PushBack(pSliding->Code);
	info.ValidAction.PushBack(pGunShot->Code);
	info.ValidAction.PushBack(pJump->Code);

	m_pPlayer = new SGPlayer();
	MainPlayer_v = m_pPlayer;
	m_pPlayer->setCharacter(createCharacter(CharacterType::Gunner, 300, 250, info));
	m_pPlayer->initActionManager();
	m_pPlayer->initController();
	m_pPlayer->runBaseAction(BaseAction::Idle);
	m_pPlayer->setMapLayer(this);

	createMonster(2, 200, 200);

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(SGMapLayer::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(SGMapLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	this->scheduleUpdate();
	return true;
}

bool SGMapLayer::isCollideWithObstacles(const SGRect& rect) {
	for (int i = 0; i < m_vCollidableObstacles.Size(); ++i) {
		SGRect thicknessBox = m_vCollidableObstacles[i]->getThicknessBoxRect();

		if (thicknessBox.intersectsRect(rect))
			return true;
	}

	return false;
}


void SGMapLayer::onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {

	// SGRectEx::log(m_pPlayer->getCharacter()->getHitBox());
	// SGRectEx::log(m_pPlayer->getCharacter()->getThicknessBoxRect());

	m_pPlayer->onKeyPressed(keyCode, event);

	if (keyCode == EventKeyboard::KeyCode::KEY_F1)
		SGGlobal::getInstance()->toggleDrawBodyBoundingBox();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F2)
		SGGlobal::getInstance()->toggleDrawThicknessBox();
}

void SGMapLayer::onKeyReleased(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	m_pPlayer->onKeyReleased(keyCode, event);
}

SGMapInfo* SGMapLayer::getMapInfo() {
	return m_pMapInfo;
}

void SGMapLayer::runFrameEvent(SGActor* runner, FrameEventType_t frameEventType, int frameEventId) {

	if (frameEventType == FrameEventType::Projectile)
		createProejctile(runner, frameEventId);
	else if (frameEventType == FrameEventType::Hitbox)
		createHitbox(runner, frameEventId);

}

SGCharacter* SGMapLayer::createCharacter(CharacterType_t characterType, float x, float y, SGCharacterInfo& info) {
	SGCharacter* pPlayerCharacter = SGCharacter::create(characterType, info, this);
	pPlayerCharacter->setPositionRealCenter(x, y);
	registerZOrderActor(pPlayerCharacter);
	registerCharacter(pPlayerCharacter);
	this->addChild(pPlayerCharacter);
	return pPlayerCharacter;
}

SGProjectile* SGMapLayer::createProejctile(SGActor* spawner, int projectileId) {
	SGProjectileInfo* pInfo = SGDataManager::getInstance()->getProjectileInfo(projectileId);
	SGProjectile* pProjectile = SGProjectile::create(spawner, pInfo, this);
	this->addChild(pProjectile);
	registerZOrderActor(pProjectile);

	if (spawner == m_pPlayer->getCharacter()) {
		registerPlayerProjectile(pProjectile);
	}
	return pProjectile;
}

void SGMapLayer::createHitbox(SGActor* spawner, int hitBoxId) {
	
}

SGMonster* SGMapLayer::createMonster(int code, float x, float y) {
	SGMonsterInfo* pInfo = SGDataManager::getInstance()->getMonsterInfo(code);
	SGMonster* pMonster = SGMonster::create(pInfo, this);
	pMonster->setPositionReal(x, y);
	registerZOrderActor(pMonster);
	registerMonster(pMonster);

	this->addChild(pMonster);
	return pMonster;
}

SGObstacle* SGMapLayer::createObstacle(int code, float x, float y) {
	SGObstacleInfo* pObstacleInfo = SGDataManager::getInstance()->getObstacleInfo(code);
	SGObstacle* pObstacle = SGObstacle::create(pObstacleInfo, this);
	pObstacle->setPositionReal(x, y);

	if (pObstacleInfo->ZOrederable)
		registerZOrderActor(pObstacle);

	if (pObstacleInfo->Colliadalble)
		registerObstacle(pObstacle);

	this->addChild(pObstacle);
	return pObstacle;
}

void SGMapLayer::registerZOrderActor(SGActor* actor) {
	m_vZOrderedActors.PushBack(actor);
}

void SGMapLayer::registerCharacter(SGCharacter* character) {
	m_vCharacters.PushBack(character);
}

void SGMapLayer::registerPlayerProjectile(SGProjectile* projectile) {
	m_vPlayerProjectiles.PushBack(projectile);
}

void SGMapLayer::registerMonster(SGMonster* monster) {
	m_vMonsters.PushBack(monster);
}

void SGMapLayer::registerObstacle(SGObstacle* obstacle) {
	m_vCollidableObstacles.PushBack(obstacle);
}

void SGMapLayer::registerCleanUp(SGActor* actor) {
	m_qRemovedActors.Enqueue(actor);
}

void SGMapLayer::unregisterZOrderActor(SGActor* actor) {
	int iOffset = m_vZOrderedActors.OffsetLowerBound(actor->getLocalZOrder(), [](const SGActor* actor, int zorder) {
		return actor->getLocalZOrder() < zorder;
	});

	DebugAssertMessage(iOffset != -1, "Z 오더 목록에서 대상을 찾지 못했습니다.");

	if (m_vZOrderedActors[iOffset] == actor) {
		m_vZOrderedActors.RemoveAt(iOffset);
	}
}

void SGMapLayer::unregisterPlayerProjectile(SGProjectile* projectile) {
	if (m_vPlayerProjectiles.Remove(projectile) == false) {
		DebugAssertMessage(false, "플레이어 프로젝틸 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void SGMapLayer::unregisterMonster(SGMonster* mosnter) {
	if (m_vMonsters.Remove(mosnter) == false) {
		DebugAssertMessage(false, "몬스터 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void SGMapLayer::unregisterObstacle(SGObstacle* obstacle) {
	
}


void SGMapLayer::update(float dt) {
	m_pPlayer->update(dt);

	updateZOrder(dt);
	updateActors(dt);
	updatePlayerProjectiles(dt);
	cleanUpActors();	// 마지막에 무조건 호출
}

void SGMapLayer::updateZOrder(float dt) {

	m_fZReorderTime += dt;
	// 0.1초 정도마다 재정렬 진행해주자.
	if (m_fZReorderTime < 0.1f) {
		return;
	}
	m_fZReorderTime = 0.0f;

	// 틱 이전, 이후 비교했을때 높은 확률로 대부분 정렬되어 있으므로 삽입 정렬 진행
	m_vZOrderedActors.SortInsertion([](SGActor* lhs, SGActor* rhs) {
		return lhs->getThicknessBoxRect().getMidY() > rhs->getThicknessBoxRect().getMidY();
	});

	for (int iOrder = 1; iOrder <= m_vZOrderedActors.Size(); ++iOrder) {
		reorderChild(m_vZOrderedActors[iOrder - 1], iOrder);
	}
}

void SGMapLayer::updateActors(float dt) {
	for (int i = 0; i < m_vZOrderedActors.Size(); ++i) {
		m_vZOrderedActors[i]->update(dt);
	}
}


void SGMapLayer::updatePlayerProjectiles(float dt) {

	auto projectileIt = m_vPlayerProjectiles.Begin();
	while (projectileIt->HasNext()) {
		SGProjectile* pProjectile = projectileIt->Next();
		auto mobIt = m_vMonsters.Begin();
		while (mobIt->HasNext()) {
			SGMonster* pMonster = mobIt->Next();
			SGRect hitRect;
			SpriteDirection_t eHitDirection;
			
			if (!pProjectile->isCollide(pMonster, eHitDirection, hitRect)) {
				continue;
			}

			
		}

		if (pProjectile->isDistanceOver()) {
			registerCleanUp(pProjectile);
			continue;
		}

		if (pProjectile->isLifeTimeOver()) {
			registerCleanUp(pProjectile);
		}
	}
}


void SGMapLayer::loadMap(int mapCode) {
	SGDataManager* pDataManager = SGDataManager::getInstance();
	SGImagePackManager* pPackManager = SGImagePackManager::getInstance();
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
		createObstacle(objInfo.Code, objInfo.X, objInfo.Y);
		
	}

	// NPC 로딩
}



void SGMapLayer::cleanUpActors() {

	while (!m_qRemovedActors.IsEmpty()) {
		SGActor* pRemovedActor = m_qRemovedActors.Front();
		m_qRemovedActors.Dequeue();
		switch (pRemovedActor->getType()) {
		case ActorType::Projectile:{
			cleanUpProjectile((SGProjectile*)pRemovedActor);
			break;
		}
		case ActorType::Monster:{
			cleanUpMonster((SGMonster*)pRemovedActor);
			break;
		}
		case ActorType::Obstacle: {
			cleanUpObstacle((SGObstacle*)pRemovedActor);
			break;
		}
		case ActorType::Character: {
			cleanUpCharacter((SGCharacter*)pRemovedActor);
			break;
		}
		default: DebugAssert(false);
		}
	}
}

void SGMapLayer::cleanUpProjectile(SGProjectile* projectile) {
	if (m_pPlayer->getCharacter() == projectile->getSpawner()) {
		unregisterPlayerProjectile(projectile);
	}

	unregisterZOrderActor(projectile);

	Log("삭제> 플레이어 프로젝틸 (%s), 남은 플레이어 프로젝틸 수 : %d, Z오더 액터 수: %d\n", projectile->getBaseInfo()->Name.Source(), m_vPlayerProjectiles.Size(), m_vZOrderedActors.Size());
	removeChild(projectile);
}

void SGMapLayer::cleanUpMonster(SGMonster* monster) {
	unregisterMonster(monster);
	unregisterZOrderActor(monster);

	Log("삭제> 몬스터 (%s), 남은 몬스터 수 : %d, Z오더 액터 수: %d\n", monster->getBaseInfo()->Name.Source(), m_vMonsters.Size(), m_vZOrderedActors.Size());
	removeChild(monster);
}

void SGMapLayer::cleanUpObstacle(SGObstacle* obstacle) {
	unregisterObstacle(obstacle);

	if (obstacle->getBaseInfo()->ZOrederable)
		unregisterZOrderActor(obstacle);

	removeChild(obstacle);
}

void SGMapLayer::cleanUpCharacter(SGCharacter* character) {
	// 해당 캐릭터 소유의 프로젝틀 모두 삭제
	// 해당 캐릭터 소유의 히트박스 모두 삭제
	// 관련핵해서 다 정리 후 캐릭터 삭제
}
