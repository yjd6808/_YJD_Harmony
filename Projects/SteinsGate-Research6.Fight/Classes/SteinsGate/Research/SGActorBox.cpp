/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 6:10:07 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGActorBox.h"

#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGActorListenerManager.h>
#include <SteinsGate/Research/SGMapLayer.h>

USING_NS_CC;
USING_NS_JC;

SGActorBox::SGActorBox()
	: m_iIdSequence(0)
	, m_pMapLayer(nullptr)
	, m_fZReorderTime(0.0f)
	, m_bCleared(false)
{}

SGActorBox::~SGActorBox() {
	clear();
}


void SGActorBox::init(SGMapLayer* mapLayer) {
	m_iIdSequence = 0;
	DebugAssertMessage(m_pMapLayer == nullptr, "맵 레이어가 이미 할당되어 있습니다. 먼저 clear를 호출해서 비워주세요");

	m_pMapLayer = mapLayer;
	m_pMapLayer->retain();
}

template <typename TActor>
void clearPool(SGHashMap<int, SGList<TActor*>>& pool) {
	pool.Keys().Extension().ForEach([&pool](int& code) {
		SGList<TActor*>& li = pool[code];
		auto it = li.Begin();
		while (it->HasNext()) {
			TActor* pActor = it->Next();
			CC_SAFE_RELEASE(pActor);
		}
	});
	pool.Clear();
}

void SGActorBox::clear() {
	if (m_bCleared)
		return;

	DebugAssertMessage(m_pMapLayer, "맵 레이어 존재하지 않습니다.");

	m_hActorMap.Values().Extension().ForEach([this](SGActor* actor) {
		CC_SAFE_RELEASE(actor);
	});
	m_hActorMap.Clear();

	clearPool<SGEffect>(m_hEffectPool);
	clearPool<SGMonster>(m_hMonsterPool);
	clearPool<SGProjectile>(m_hProjectilePool);
	clearPool<SGObstacle>(m_hObstaclePool);

	CC_SAFE_RELEASE_NULL(m_pMapLayer);
	m_bCleared = true;
}

void SGActorBox::update(float dt) {
	updateZOrder(dt);
	updateActors(dt);
	updateCleanUp();
}

void SGActorBox::updateCleanUp() {
	while (!m_qRemovedActors.IsEmpty()) {
		SGActor* pRemovedActor = m_qRemovedActors.Front();
		m_qRemovedActors.Dequeue();
		m_hRemoveActorMap.Remove(pRemovedActor);

		switch (pRemovedActor->getType()) {
		case ActorType::Effect: {
			cleanUpEffect((SGEffect*)pRemovedActor);
			break;
		}
		case ActorType::Projectile: {
			cleanUpProjectile((SGProjectile*)pRemovedActor);
			break;
		}
		case ActorType::Monster: {
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
		default: {
			DebugAssertMessage(false, "이상한 액터 타입입니다.");
			break;
		}
		}

		m_pMapLayer->removeChild(pRemovedActor);
	}
}

void SGActorBox::updateZOrder(float dt) {

	m_fZReorderTime += dt;

	// 0.1초 정도마다 재정렬 진행해주자.
	if (m_fZReorderTime < ZReodrerDelay_v) {
		return;
	}
	m_fZReorderTime = 0.0f;

	// 틱 이전, 이후 비교했을때 높은 확률로 대부분 정렬되어 있으므로 삽입 정렬 진행
	m_vZOrderedActors.SortInsertion([](SGActor* lhs, SGActor* rhs) {
		return lhs->getThicknessBoxRect().getMidY() > rhs->getThicknessBoxRect().getMidY();
	});

	for (int iOrder = 1; iOrder <= m_vZOrderedActors.Size(); ++iOrder) {
		m_pMapLayer->reorderChild(m_vZOrderedActors[iOrder - 1], iOrder);
	}
}

void SGActorBox::updateActors(float dt) {
	for (int i = 0; i < m_vProjectiles.Size(); ++i) {
		m_vProjectiles[i]->update(dt);
	}

	for (int i = 0; i < m_vMonsters.Size(); ++i) {
		m_vMonsters[i]->update(dt);
	}

	for (int i = 0; i < m_vCollidableObstacles.Size(); ++i) {
		m_vCollidableObstacles[i]->update(dt);
	}

	for (int i = 0; i < m_vCharacters.Size(); ++i) {
		m_vCharacters[i]->update(dt);
	}

	for (int i = 0; i < m_vEffectList.Size(); ++i) {
		m_vEffectList[i]->update(dt);
	}
}


SGCharacter* SGActorBox::createCharacterOnMap(CharType_t charType, float x, float y, SGCharacterInfo& info) {
	DebugAssertMessage(m_pMapLayer, "맵 레이어 존재하지 않습니다.");

	SGCharacter* pCharacter = SGCharacter::create(charType, info);
	pCharacter->setPositionRealCenter(x, y);
	pCharacter->setAllyFlag(0);
	pCharacter->setMapLayer(m_pMapLayer);
	// pCharacter->retain(); 캐릭터 풀 필요
	registerZOrderActor(pCharacter);
	registerCharacter(pCharacter);
	registerPhysicsActor(pCharacter);
	registerActor(pCharacter);
	m_pMapLayer->addChild(pCharacter);
	return pCharacter;
}

SGProjectile* SGActorBox::createProejctileOnMap(SGActor* spawner, int projectileId) {
	DebugAssertMessage(m_pMapLayer, "맵 레이어 존재하지 않습니다.");

	SGProjectileInfo* pInfo = SGDataManager::getInstance()->getProjectileInfo(projectileId);
	

	if (!m_hProjectilePool.Exist(projectileId)) {
		m_hProjectilePool.Insert(Move(projectileId), SGList<SGProjectile*>());
	}
	
	SGProjectile* pProjectile = nullptr;
	SGActorListenerManager* pActorListenerManager = SGActorListenerManager::getInstance();
	SGList<SGProjectile*>& projectileList = m_hProjectilePool[projectileId];

	if (projectileList.IsEmpty()) {
		SGActorListener* pListener = pActorListenerManager->createProjectileListener(pInfo->ProjectileListenerCode);

		pProjectile = SGProjectile::create(spawner, pInfo);
		pProjectile->initListener(pListener);
		pProjectile->retain();
		pListener->onCreated();

	} else {
		pProjectile = projectileList.Front();
		pProjectile->initVariables();
		pProjectile->setSpawner(spawner);											// 위치 세팅전 스포너 세팅 먼저 해줘야함
		pProjectile->initThicknessBox(pProjectile->getBaseInfo()->ThicknessBox);	// 두께 박스 위치 재조정
		pProjectile->initPosition();												// 살제 위치 초기화
		pProjectile->getListener()->onCreated();
		projectileList.PopFront();
	}


	pProjectile->runAnimation(1);
	pProjectile->setAllyFlag(spawner->getAllyFlag());
	pProjectile->setMapLayer(m_pMapLayer);

	if (pInfo->SpawnEffect->Code != InvalidValue_v) {
		SGEffect* pSpawnEffect = createEffectOnMapBySpawner(spawner, pInfo->SpawnEffect->Code, pInfo->SpawnEffectOffsetX, pInfo->SpawnEffectOffsetY);
		pSpawnEffect->setLocalZOrder(spawner->getLocalZOrder() + 1);
	}
	
	registerZOrderActor(pProjectile);
	registerProjectile(pProjectile);
	registerActor(pProjectile);
	m_pMapLayer->addChild(pProjectile);
	return pProjectile;
}


SGMonster* SGActorBox::createMonsterOnMap(int monsterCode, int aiCode, float x, float y) {
	DebugAssertMessage(m_pMapLayer, "맵 레이어 존재하지 않습니다.");

	SGDataManager* pDataManager = SGDataManager::getInstance();

	SGMonsterInfo* pMonsterInfo = pDataManager->getMonsterInfo(monsterCode);
	SGAIInfo* pAIInfo = pDataManager->getAIInfo(aiCode);

	if (!m_hMonsterPool.Exist(monsterCode)) {
		m_hMonsterPool.Insert(Move(monsterCode), SGList<SGMonster*>());
	}

	SGMonster* pMonster = nullptr;
	SGList<SGMonster*>& monsterList = m_hMonsterPool[monsterCode];

	if (monsterList.IsEmpty()) {
		pMonster = SGMonster::create(pMonsterInfo, pAIInfo);
		pMonster->retain();
	} else {
		pMonster = monsterList.Front();
		pMonster->initVariables();
		monsterList.PopFront();
	}

	pMonster->setPositionReal(x, y);
	pMonster->setMapLayer(m_pMapLayer);
	pMonster->setAllyFlag(1);

	registerZOrderActor(pMonster);
	registerMonster(pMonster);
	registerPhysicsActor(pMonster);
	registerActor(pMonster);
	m_pMapLayer->addChild(pMonster);

	return pMonster;
}

SGObstacle* SGActorBox::createObstacleOnMap(int obstacleCode, float x, float y) {
	DebugAssertMessage(m_pMapLayer, "맵 레이어 존재하지 않습니다.");

	SGObstacleInfo* pObstacleInfo = SGDataManager::getInstance()->getObstacleInfo(obstacleCode);

	if (!m_hObstaclePool.Exist(obstacleCode)) {
		m_hObstaclePool.Insert(Move(obstacleCode), SGList<SGObstacle*>());
	}

	SGObstacle* pObstacle = nullptr;
	SGList<SGObstacle*>& obstacleList = m_hObstaclePool[obstacleCode];

	if (obstacleList.IsEmpty()) {
		pObstacle = SGObstacle::create(pObstacleInfo);
		pObstacle->retain();
	} else {
		pObstacle = obstacleList.Front();
		pObstacle->initVariables();
		obstacleList.PopFront();
	}
	pObstacle->setPositionReal(x, y);
	pObstacle->setMapLayer(m_pMapLayer);

	if (pObstacleInfo->ZOrederable)
		registerZOrderActor(pObstacle);

	if (pObstacleInfo->Colliadalble)
		registerObstacle(pObstacle);

	registerActor(pObstacle);
	m_pMapLayer->addChild(pObstacle);

	return pObstacle;
}



SGEffect* SGActorBox::createEffectOnMapBySpawner(SGActor* spawner, int effectCode, float offsetX, float offsetY) {
	SGEffect* pEffect = createEffectOnMap(effectCode);

	SGSize spawnerCanvsSize = spawner->getCanvasSize();
	SGVec2 spawnerCanvasPos = spawner->getCanvasPositionReal();

	pEffect->setSpriteDirection(spawner->getSpriteDirection());

	if (spawner->getSpriteDirection() == SpriteDirection::Right) {
		pEffect->setPositionReal(
			spawnerCanvasPos.x + offsetX,
			spawnerCanvasPos.y + offsetY
		);
	} else {
		pEffect->setPositionReal(
			spawnerCanvasPos.x + spawnerCanvsSize.width - offsetX,
			spawnerCanvasPos.y + offsetY
		);
	}

	

	return pEffect;
}

SGEffect* SGActorBox::createEffectOnMapBySpawner(SGActor* spawner, int effectCode, const SGVec2& offset) {
	return createEffectOnMapBySpawner(spawner, effectCode, offset.x, offset.y);
}

SGEffect* SGActorBox::createEffectOnMapAbsolute(int effectCode, SpriteDirection_t direction, float x, float y, int zOrder) {
	SGEffect* pEffect = createEffectOnMap(effectCode);
	pEffect->setSpriteDirection(direction);
	pEffect->setPositionRealCenter(x, y);
	pEffect->setLocalZOrder(zOrder);
	return pEffect;
}

SGEffect* SGActorBox::createEffectOnMapAbsolute(int effectCode, SpriteDirection_t direction, const SGVec2& pos, int zOrder) {
	return createEffectOnMapAbsolute(effectCode, direction, pos.x, pos.y, zOrder);
}

SGEffect* SGActorBox::createEffectOnMapAbsolute(int effectCode, float x, float y, int zOrder) {
	SGEffect* pEffect = createEffectOnMap(effectCode);
	pEffect->setPositionRealCenter(x, y);
	pEffect->setLocalZOrder(zOrder);
	return pEffect;
}

SGEffect* SGActorBox::createEffectOnMapAbsolute(int effectCode, const SGVec2& pos, int zOrder) {
	return createEffectOnMapAbsolute(effectCode, pos.x, pos.y, zOrder);
}


SGEffect* SGActorBox::createEffectOnMapTargetCollision(int effectCode, SpriteDirection_t direction, const SGHitInfo& info, bool randomRotation) {
	SGEffect* pEffect = createEffectOnMapAbsolute(effectCode, info.HitRect.getMid(), info.HitTarget->getLocalZOrder() + 1);
	pEffect->setSpriteDirection(direction);

	if (randomRotation) {
		pEffect->setRotation(SGRandom::random_real(0.0f, 360.0f));
	}

	return pEffect;
}

SGEffect* SGActorBox::createEffectOnMapTargetCollision(int effectCode, const SGHitInfo& info, bool randomRotation) {
	return createEffectOnMapTargetCollision(effectCode, SpriteDirection::Right, info, randomRotation);
}

SGEffect* SGActorBox::createEffectOnMapTargetCollision(int effectCode, const SGHitInfo& info, float offsetX, float offsetY, bool randomRotation) {
	SGEffect* pEffect = createEffectOnMapTargetCollision(effectCode, info, randomRotation);
	pEffect->setPosition(pEffect->getPositionX() + offsetX, pEffect->getPositionX() + offsetY);
	return pEffect;
}

SGEffect* SGActorBox::createEffectOnMapTargetCollision(int effectCode, const SGHitInfo& info, const SGVec2& offset, bool randomRotation) {
	return createEffectOnMapTargetCollision(effectCode, info, offset.x, offset.y, randomRotation);
}

SGEffect* SGActorBox::createEffectOnMap(int effectCode) {
	DebugAssertMessage(m_pMapLayer, "맵 레이어가 존재하지 않습니다.");

	SGEffectInfo* pEffectInfo = SGDataManager::getInstance()->getEffectInfo(effectCode);

	if (!m_hEffectPool.Exist(effectCode)) {
		m_hEffectPool.Insert(Move(effectCode), SGList<SGEffect*>());
	}

	SGEffect* pEffect = nullptr;
	SGList<SGEffect*>& effectList = m_hEffectPool[effectCode];

	if (effectList.IsEmpty()) {
		pEffect = SGEffect::create(pEffectInfo);
		pEffect->retain();
	}
	else {
		pEffect = effectList.Front();
		pEffect->initVariables();
		effectList.PopFront();
	}

	pEffect->runAnimation(1);
	pEffect->initThicknessBox({ 0, 0, 0, 0 });
	pEffect->setMapLayer(m_pMapLayer);
	registerEffect(pEffect);
	registerActor(pEffect);
	m_pMapLayer->addChild(pEffect);
	return pEffect;
}


void SGActorBox::registerZOrderActor(SGActor* actor) {
	m_vZOrderedActors.PushBack(actor);
}

void SGActorBox::registerCharacter(SGCharacter* character) {
	m_vCharacters.PushBack(character);
}

void SGActorBox::registerProjectile(SGProjectile* projectile) {
	m_vProjectiles.PushBack(projectile);
}

void SGActorBox::registerMonster(SGMonster* monster) {
	m_vMonsters.PushBack(monster);
}

void SGActorBox::registerObstacle(SGObstacle* obstacle) {
	m_vCollidableObstacles.PushBack(obstacle);
}

void SGActorBox::registerEffect(SGEffect* effect) {
	m_vEffectList.PushBack(effect);
}

void SGActorBox::registerPhysicsActor(SGPhysicsActor* physicsActor) {
	m_vPhysicsActors.PushBack(physicsActor);
}

void SGActorBox::registerActor(SGActor* actor) {
	int iUniqueId = ++m_iIdSequence;
	actor->setActorId(iUniqueId);
	m_hActorMap.Insert(iUniqueId, actor);
}

void SGActorBox::registerCleanUp(SGActor* actor) {
	// 이미 존재하는 경우 무시
	if (m_hRemoveActorMap.Exist(actor)) {
		return;
	}
	m_hRemoveActorMap.Insert(actor, actor);
	m_qRemovedActors.Enqueue(actor);
}

void SGActorBox::unregisterZOrderActor(SGActor* actor) {
	int iOffset = m_vZOrderedActors.OffsetLowerBound(actor->getLocalZOrder(), [](const SGActor* actor, int zorder) {
		return actor->getLocalZOrder() < zorder;
	});

	DebugAssertMessage(iOffset != -1, "Z 오더 목록에서 대상을 찾지 못했습니다.");

	if (m_vZOrderedActors[iOffset] == actor) {
		m_vZOrderedActors.RemoveAt(iOffset);
	}
}

void SGActorBox::unregisterProjectile(SGProjectile* projectile) {
	if (!m_vProjectiles.Remove(projectile)) {
		DebugAssertMessage(false, "프로젝틸 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void SGActorBox::unregisterCharacter(SGCharacter* chracter) {
	if (!m_vCharacters.Remove(chracter)) {
		DebugAssertMessage(false, "캐릭터 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void SGActorBox::unregisterMonster(SGMonster* mosnter) {
	if (!m_vMonsters.Remove(mosnter)) {
		DebugAssertMessage(false, "몬스터 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void SGActorBox::unregisterObstacle(SGObstacle* obstacle) {
	if (obstacle->getBaseInfo()->Colliadalble)
		if (!m_vCollidableObstacles.Remove(obstacle))
			DebugAssertMessage(false, "옵스터클 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
}

void SGActorBox::unregisterEffect(SGEffect* effect) {
	if (!m_vEffectList.Remove(effect)) 
		DebugAssertMessage(false, "이펙트 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
}

void SGActorBox::unregisterPhysicsActor(SGPhysicsActor* physicsActor) {
	if (!m_vPhysicsActors.Remove(physicsActor))
		DebugAssertMessage(false, "피직스 액터 목록에서 액터를 제거하는데 실패했습니다.");
}

void SGActorBox::unregisterActor(SGActor* actor) {
	DebugAssertMessage(actor->getActorId() != InvalidValue_v, "올바르지 않은 액터 ID입니다.");
	if (!m_hActorMap.Remove(actor->getActorId()))
		DebugAssertMessage(false, "공통 액터 목록에서 액터를 제거하는데 실패했습니다.");
	else
		actor->setActorId(InvalidValue_v);
}


void SGActorBox::cleanUpProjectile(SGProjectile* projectile) {
	unregisterProjectile(projectile);
	unregisterZOrderActor(projectile);
	unregisterActor(projectile);
	m_hProjectilePool[projectile->getBaseInfo()->Code].PushBack(projectile);
	// Log("삭제> 플레이어 프로젝틸 (%s), 남은 플레이어 프로젝틸 수 : %d, Z오더 액터 수: %d\n", projectile->getBaseInfo()->Name.Source(), m_vProjectiles.Size(), m_vZOrderedActors.Size());
}

void SGActorBox::cleanUpMonster(SGMonster* monster) {
	unregisterMonster(monster);
	unregisterZOrderActor(monster);
	unregisterPhysicsActor(monster);
	unregisterActor(monster);
	m_hMonsterPool[monster->getBaseInfo()->Code].PushBack(monster);
	// Log("삭제> 몬스터 (%s), 남은 몬스터 수 : %d, Z오더 액터 수: %d\n", monster->getBaseInfo()->Name.Source(), m_vMonsters.Size(), m_vZOrderedActors.Size());
}

void SGActorBox::cleanUpObstacle(SGObstacle* obstacle) {
	unregisterObstacle(obstacle);
	unregisterActor(obstacle);

	if (obstacle->getBaseInfo()->ZOrederable)
		unregisterZOrderActor(obstacle);

	m_hObstaclePool[obstacle->getBaseInfo()->Code].PushBack(obstacle);
}

void SGActorBox::cleanUpCharacter(SGCharacter* character) {
	unregisterCharacter(character);
	unregisterZOrderActor(character);
	unregisterActor(character);

	
}

void SGActorBox::cleanUpEffect(SGEffect* effect) {
	unregisterEffect(effect);
	unregisterActor(effect);

	m_hEffectPool[effect->getBaseInfo()->Code].PushBack(effect);
}

