/*
 * 작성자 : 윤정도
 */

#include "SGMapLayer.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGGlobal.h>

#include <SteinsGate/Common/Engine/SGRectEx.h>

#include "SteinsGate/Common/Engine/RectPoly.h"


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
	SGActionInfo* pFallDown= pConfig->getActionInfo("fall_down");
	SGActionInfo* pSitRecover = pConfig->getActionInfo("sit_recover");
	SGActionInfo* pHit = pConfig->getActionInfo("hit");


	info.ValidAction.PushBack(pIdle->Code);
	info.ValidAction.PushBack(pWalk->Code);
	info.ValidAction.PushBack(pRun->Code);
	info.ValidAction.PushBack(pSliding->Code);
	info.ValidAction.PushBack(pGunShot->Code);
	info.ValidAction.PushBack(pJump->Code);
	info.ValidAction.PushBack(pFallDown->Code);
	info.ValidAction.PushBack(pSitRecover->Code);
	info.ValidAction.PushBack(pHit->Code);

	m_pPlayer = new SGPlayer();
	MainPlayer_v = m_pPlayer;
	m_pPlayer->setCharacter(createCharacter(CharacterType::Gunner, 300, 250, info));
	m_pPlayer->initActionManager();
	m_pPlayer->initController();
	m_pPlayer->runBaseAction(BaseAction::Idle);
	m_pPlayer->setMapLayer(this);

	createMonster(2, 1, 600, 350);

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



SGCharacter* SGMapLayer::findNearestCharacterInRadious(SGActor* stdActor, float radious, Out_ float& enemyDist) {
	int iAllyFlag = stdActor->getAllyFlag();
	SGVec2 stdPos = stdActor->getPositionRealCenter();
	SGCharacter* character = nullptr;
	float minDist = FLT_MAX;

	for (int i = 0; i < m_vCharacters.Size(); ++i) {
		auto pCharacter = m_vCharacters[i];

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


bool SGMapLayer::collectEnemiesInInstantAttackBox(
	SGActor* attacker,
	const SGActorRect& absoluteActorRect, SGVector<SGHitInfo>& hitMonsters) {

	bool bFind = false;
	
	for (int i = 0; i < m_vPhysicsActors.Size(); ++i) {
		auto pHitTarget = m_vPhysicsActors[i];	// 공격받을 대상
		SGRect hitRect;
		SpriteDirection_t eHitDirection;

		if (pHitTarget->getAllyFlag() == attacker->getAllyFlag())
			continue;

		// 몬스터 기준으로 플레이어 충돌이라
		// eHitDirection은 플레이어의 충돌방향이 되므로, 반대로 돌려줘야함.
		if (pHitTarget->isCollide(absoluteActorRect, eHitDirection, hitRect)) {
			hitMonsters.PushBack({pHitTarget, SpriteDirection::Reverse[eHitDirection], hitRect, nullptr });
			bFind = true;
		}
	}

	return bFind;
}


void SGMapLayer::onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {

	// SGRectEx::log(m_pPlayer->getCharacter()->getHitBox());
	// SGRectEx::log(m_pPlayer->getCharacter()->getThicknessBoxRect());

	m_pPlayer->onKeyPressed(keyCode, event);

	if (keyCode == EventKeyboard::KeyCode::KEY_F1)
		SGGlobal::getInstance()->toggleDrawBodyBoundingBox();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F2)
		SGGlobal::getInstance()->toggleDrawThicknessBox();
	else if (keyCode == EventKeyboard::KeyCode::KEY_F3)
		SGGlobal::getInstance()->toggleDrawAttackBox();
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
	else if (frameEventType == FrameEventType::AttackBox)
		createAttackBox(runner, frameEventId);
}

SGCharacter* SGMapLayer::createCharacter(CharacterType_t characterType, float x, float y, SGCharacterInfo& info) {
	SGCharacter* pPlayerCharacter = SGCharacter::create(characterType, info, this);
	pPlayerCharacter->setPositionRealCenter(x, y);
	pPlayerCharacter->setAllyFlag(0);
	registerZOrderActor(pPlayerCharacter);
	registerCharacter(pPlayerCharacter);
	registerPhysicsActor(pPlayerCharacter);
	this->addChild(pPlayerCharacter);
	return pPlayerCharacter;
}

SGProjectile* SGMapLayer::createProejctile(SGActor* spawner, int projectileId) {
	SGProjectileInfo* pInfo = SGDataManager::getInstance()->getProjectileInfo(projectileId);
	SGProjectile* pProjectile = SGProjectile::create(spawner, pInfo, this);
	pProjectile->setAllyFlag(spawner->getAllyFlag());
	this->addChild(pProjectile);
	registerZOrderActor(pProjectile);

	if (spawner == m_pPlayer->getCharacter()) {
		registerPlayerProjectile(pProjectile);
	}
	return pProjectile;
}

void SGMapLayer::createAttackBox(SGActor* spawner, int hitBoxId) {
	
}



SGMonster* SGMapLayer::createMonster(int code, int aiCode, float x, float y) {
	SGDataManager* pDataManager = SGDataManager::getInstance();

	SGMonsterInfo* pMonsterInfo = pDataManager->getMonsterInfo(code);
	SGAIInfo* pAIInfo = pDataManager->getAIInfo(aiCode);

	SGMonster* pMonster = SGMonster::create(pMonsterInfo, this, pAIInfo);
	pMonster->setPositionReal(x, y);
	pMonster->setAllyFlag(1);
	registerZOrderActor(pMonster);
	registerMonster(pMonster);
	registerPhysicsActor(pMonster);
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

void SGMapLayer::registerPhysicsActor(SGPhysicsActor* physicsActor) {
	m_vPhysicsActors.PushBack(physicsActor);
}

void SGMapLayer::registerCleanUp(SGActor* actor) {
	// 이미 존재하는 경우 무시
	if (m_hRemoveActorMap.Exist(actor)) {
		return;
	}
	m_hRemoveActorMap.Insert(actor, actor);
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

void SGMapLayer::unregisterPhysicsActor(SGPhysicsActor* physicsActor) {
	m_vPhysicsActors.Remove(physicsActor);
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

	for (int i = 0; i < m_vPlayerProjectiles.Size(); ++i) {
		SGProjectile* pProjectile = m_vPlayerProjectiles[i];

		/* 여기서 충돌 체크하면 안될 것 같다.
		 * 개틀링 총알, 양자폭탄 이런것도 투사체인데..양자 폭탄은 다 투과하고 바닥에만 충격을 주는데
		 * 투사체 종류별로 새로 구현해줘야하는건가?
		 * 고민을...
		for (int j = 0; j < m_vPhysicsActors.Size(); ++j) {
			SGPhysicsActor* pTarget = m_vPhysicsActors[j];
			SGRect hitRect;
			SpriteDirection_t eHitDirection;

			if (pProjectile->getSpawner()->getAllyFlag() == pTarget->getAllyFlag()) {
				continue;
			}

			if (!pProjectile->isCollide(pTarget, eHitDirection, hitRect)) {
				continue;
			}

			registerCleanUp(pProjectile);
			pTarget->hit(eHitDirection, hitRect, pProjectile->getBaseInfo()->AttackData);
			break;
		}
		*/
		if (pProjectile->isDistanceOver() ||
			pProjectile->isLifeTimeOver() ||
			pProjectile->isOnTheGround()) {
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
			// TODO: 프레임 텍스쳐 모았다가 나중에 해제

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
		m_hRemoveActorMap.Remove(pRemovedActor);

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
	unregisterPhysicsActor(monster);

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
