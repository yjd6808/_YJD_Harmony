/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 6:10:07 PM
 * =====================
 *
 * 1. 캐릭터의 경우 외부 플레이어가 관리함
 * 2. 던전 단위, 마을 단위로 clean
 * 3.  던전, 마을 단위 이동시에는 풀까지 전부 비우기
 *     방단위 이동시에는 전부 풀로 복귀
 *
 * 전체 정리 순서
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
	, m_bCleared(true)
{}

SGActorBox::~SGActorBox() {
	clearAll();
}


void SGActorBox::init(SGMapLayer* mapLayer) {
	DebugAssertMsg(m_bCleared == true, "먼저 깔끔하게 비워주세요.");

	m_iIdSequence = 0;

	m_pMapLayer = mapLayer;
	m_pMapLayer->retain();
	m_bCleared = false;
}

template <typename TActor>
static int releasePool(SGHashMap<int, SGList<TActor*>>& pool) {
	int iReleaseCount = 0;
	pool.Keys().Extension().ForEach([&iReleaseCount , &pool](int& code) {
		SGList<TActor*>& li = pool[code];
		auto it = li.Begin();
		while (it->HasNext()) {
			TActor* pActor = it->Next();
			++iReleaseCount;
			CC_SAFE_RELEASE(pActor);
		}
	});
	pool.Clear();
	return iReleaseCount;
}

template <typename TActor>
static int releaseList(SGVector<TActor*>& actorList) {
	int iReleaseCount = 0;
	actorList.Extension().ForEach([&iReleaseCount](TActor* actor) {
		CC_SAFE_RELEASE(actor);
		++iReleaseCount;	
	});
	actorList.Clear();
	Log("[%10s] 릴리즈: %d개\n", typeid(TActor).name(), iReleaseCount);
	return iReleaseCount;
}

template <typename TActor>
static int cleanUpList(SGVector<TActor*>& actorList) {
	int iCleanUpCount = 0;
	actorList.Extension().ForEach([&iCleanUpCount](TActor* actor) {
		actor->cleanUpNext();
		++iCleanUpCount;
	});
	actorList.Clear();
	Log("[%10s] 클린업: %d개\n", typeid(TActor).name(), iCleanUpCount);
	return iCleanUpCount;
}

void SGActorBox::clearAll() {

	if (m_bCleared)
		return;

	// 캐릭터 제외하고 모두 릴리즈 수행
	DebugAssertMsg(m_pMapLayer, "[SGActorBox] 맵 레이어가 존재하지 않습니다.");

	// Step 1. 맵에 로딩되지 않은 풀의 엑터들을 정리해줘야한다.
	// Step 2. 맵에 로딩된 풀에 없는 엑터들을 정리해줘야한다.

	cleanUpList<SGCharacter>(m_vCharacters);
	Log("Step Before. 정리 대기중 엑터 수: %d개\n", m_hRemoveActorMap.Size());
	updateCleanUp();
	DebugAssertMsg(m_hRemoveActorMap.Size() == 0, "아직 반환안된 액터가 있습니다. 이러면 안됩니다.");

	// 캐릭터들은 액터 ID 초기화 해줘야함
	int iStep1ReleaseCount = 0;
	int iStep2ReleaseCount = 0;

	// Step1. 현재 맵에 로딩된 액터들 릴리즈
	iStep1ReleaseCount += releaseList<SGProjectile>(m_vProjectiles);
	iStep1ReleaseCount += releaseList<SGMonster>(m_vMonsters);
	iStep1ReleaseCount += releaseList<SGObstacle>(m_vObstacles);
	iStep1ReleaseCount += releaseList<SGEffect>(m_vEffectList);
	Log("Step1. 정리된 액터 수: %d개\n", iStep1ReleaseCount);
	DebugAssertMsg(iStep1ReleaseCount == m_hActorMap.Size(), "Step1. 실패");


	// Step2. 현재 맵에 로딩되지 않은 엑터들 릴리즈
	iStep2ReleaseCount += releasePool<SGEffect>(m_hEffectPool);
	iStep2ReleaseCount += releasePool<SGMonster>(m_hMonsterPool);
	iStep2ReleaseCount += releasePool<SGProjectile>(m_hProjectilePool);
	iStep2ReleaseCount += releasePool<SGObstacle>(m_hObstaclePool);
	Log("Step2. 정리된 액터 수: %d개\n", iStep2ReleaseCount);

	m_vZOrderedActors.Clear();
	m_vCollidableObstacles.Clear();
	m_vPhysicsActors.Clear();
	m_hActorMap.Clear();

	CC_SAFE_RELEASE_NULL(m_pMapLayer);
	m_bCleared = true;
}


void SGActorBox::clearRoom() {
	// 여기선 해줘야함 삭제시키는게 목적이 아니라 풀로 복귀 시키는게 목적이기 때문
	int iCleanUpCount = 0;

	iCleanUpCount += cleanUpList<SGProjectile>(m_vProjectiles);
	iCleanUpCount += cleanUpList<SGMonster>(m_vMonsters);
	iCleanUpCount += cleanUpList<SGObstacle>(m_vObstacles);
	iCleanUpCount += cleanUpList<SGEffect>(m_vEffectList);
	iCleanUpCount += cleanUpList<SGCharacter>(m_vCharacters);

	Log("Step Before. 정기 대기중 엑터 수: %d개\n", m_hRemoveActorMap.Size());
	if (updateCleanUp() != iCleanUpCount) {
		DebugAssertMsg(false, "정리가 제대로 수행되지 않았습니다.");
	}
	DebugAssertMsg(m_hRemoveActorMap.Size() == 0, "아직 반환안된 액터가 있습니다. 이러면 안됩니다.");

	m_vZOrderedActors.Clear();
	m_vCollidableObstacles.Clear();
	m_vPhysicsActors.Clear();
	m_hActorMap.Clear();

	// 풀 4개는 건들지 않음

	CC_SAFE_RELEASE_NULL(m_pMapLayer);
	m_bCleared = true;
}

void SGActorBox::update(float dt) {

	DebugAssertMsg(m_pMapLayer, "[SGActorBox] 맵 레이어가 세팅되지 않았습니다.");

	updateZOrder();
	updateActors(dt);
	updateCleanUp();
}

int SGActorBox::updateCleanUp() {
	int iCleanUpCount = m_qRemovedActors.Size();
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
			DebugAssertMsg(false, "이상한 액터 타입입니다.");
			break;
		}
		}

		m_pMapLayer->removeChild(pRemovedActor);
	}

	DebugAssertMsg(m_hRemoveActorMap.IsEmpty(), "아직 삭제되지 않은 액터가 있습니다. 말도 안됩니다.");
	return iCleanUpCount;
}


void SGActorBox::updateZOrder() {
	// 틱 이전, 이후 비교했을때 높은 확률로 대부분 정렬되어 있으므로 삽입 정렬 진행
	sortZOrderActor();

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

	for (int i = 0; i < m_vObstacles.Size(); ++i) {
		m_vObstacles[i]->update(dt);
	}

	for (int i = 0; i < m_vCharacters.Size(); ++i) {
		m_vCharacters[i]->update(dt);
	}

	for (int i = 0; i < m_vEffectList.Size(); ++i) {
		m_vEffectList[i]->update(dt);
	}
}


SGCharacter* SGActorBox::createCharacter(CharType_t charType, float x, float y, SGCharacterInfo& info) {
	SGCharacter* pCharacter = SGCharacter::create(charType, info);
	pCharacter->setPositionRealCenter(x, y);
	pCharacter->setAllyFlag(0);
	pCharacter->retain(); 

	return pCharacter;
}

SGProjectile* SGActorBox::createProejctileOnMap(SGActor* spawner, int projectileId) {
	DebugAssertMsg(m_pMapLayer, "맵 레이어가 존재하지 않습니다.");

	SGProjectileInfo* pInfo = SGDataManager::get()->getProjectileInfo(projectileId);
	

	if (!m_hProjectilePool.Exist(projectileId)) {
		m_hProjectilePool.Insert(Move(projectileId), SGList<SGProjectile*>());
	}
	
	SGProjectile* pProjectile = nullptr;
	SGActorListenerManager* pActorListenerManager = SGActorListenerManager::get();
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
	
	registerProjectile(pProjectile);
	m_pMapLayer->addChild(pProjectile);
	return pProjectile;
}


SGMonster* SGActorBox::createMonsterOnMap(int monsterCode, int aiCode, float x, float y) {
	DebugAssertMsg(m_pMapLayer, "맵 레이어가 존재하지 않습니다.");

	SGDataManager* pDataManager = SGDataManager::get();

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

	registerMonster(pMonster);
	m_pMapLayer->addChild(pMonster);

	return pMonster;
}

SGObstacle* SGActorBox::createObstacleOnMap(int obstacleCode, float x, float y) {
	DebugAssertMsg(m_pMapLayer, "맵 레이어가 존재하지 않습니다.");

	SGObstacleInfo* pObstacleInfo = SGDataManager::get()->getObstacleInfo(obstacleCode);

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

	registerObstacle(pObstacle);
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
	DebugAssertMsg(m_pMapLayer, "맵 레이어가 존재하지 않습니다.");

	SGEffectInfo* pEffectInfo = SGDataManager::get()->getEffectInfo(effectCode);

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
	m_pMapLayer->addChild(pEffect);
	return pEffect;
}


void SGActorBox::registerCharacter(SGCharacter* character) {
	m_vCharacters.PushBack(character);
	m_vPhysicsActors.PushBack(character);
	m_vZOrderedActors.PushBack(character);
	registerActor(character);
}

void SGActorBox::registerProjectile(SGProjectile* projectile) {
	m_vProjectiles.PushBack(projectile);
	m_vZOrderedActors.PushBack(projectile);
	registerActor(projectile);
}

void SGActorBox::registerMonster(SGMonster* monster) {
	m_vMonsters.PushBack(monster);
	m_vZOrderedActors.PushBack(monster);
	m_vPhysicsActors.PushBack(monster);
	registerActor(monster);
}

void SGActorBox::registerObstacle(SGObstacle* obstacle) {
	m_vObstacles.PushBack(obstacle);

	if (obstacle->getBaseInfo()->ZOrederable)
		m_vZOrderedActors.PushBack(obstacle);

	if (obstacle->getBaseInfo()->Colliadalble)
		m_vCollidableObstacles.PushBack(obstacle);

	registerActor(obstacle);
}

void SGActorBox::registerEffect(SGEffect* effect) {
	m_vEffectList.PushBack(effect);
	registerActor(effect);
}


void SGActorBox::registerActor(SGActor* actor) {
	int iUniqueId = ++m_iIdSequence;
	actor->setActorId(iUniqueId);
	m_hActorMap.Insert(iUniqueId, actor);
}

void SGActorBox::sortZOrderActor() {
	m_vZOrderedActors.SortInsertion([](SGActor* lhs, SGActor* rhs) {
		return lhs->getThicknessBoxRect().getMidY() > rhs->getThicknessBoxRect().getMidY();
	});

	
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

	DebugAssertMsg(iOffset != -1, "Z 오더 목록에서 대상을 찾지 못했습니다.");
	DebugAssertMsg(m_vZOrderedActors[iOffset] == actor, "Z 오더 목록에서 찾은 대상이 동일하지 않습니다. 로직 오류! 로직 오류!");

	if (m_vZOrderedActors[iOffset] == actor) {
		m_vZOrderedActors.RemoveAt(iOffset);
	}
}

void SGActorBox::unregisterProjectile(SGProjectile* projectile) {
	if (!m_vProjectiles.Remove(projectile)) {
		DebugAssertMsg(false, "프로젝틸 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void SGActorBox::unregisterCharacter(SGCharacter* chracter) {
	if (!m_vCharacters.Remove(chracter)) {
		DebugAssertMsg(false, "캐릭터 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void SGActorBox::unregisterMonster(SGMonster* mosnter) {
	if (!m_vMonsters.Remove(mosnter)) {
		DebugAssertMsg(false, "몬스터 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void SGActorBox::unregisterObstacle(SGObstacle* obstacle) {

	if (!m_vObstacles.Remove(obstacle)) {
		DebugAssertMsg(false, "옵스터클 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}

void SGActorBox::unregisterColidableObstacle(SGObstacle* obstacle) {
	if (!m_vCollidableObstacles.Remove(obstacle)) {
		DebugAssertMsg(false, "충돌 가능한 옵스터클 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
	}
}



void SGActorBox::unregisterEffect(SGEffect* effect) {
	if (!m_vEffectList.Remove(effect)) 
		DebugAssertMsg(false, "이펙트 목록에서 삭제하고자하는 대상을 찾지못했습니다.");
}

void SGActorBox::unregisterPhysicsActor(SGPhysicsActor* physicsActor) {
	if (!m_vPhysicsActors.Remove(physicsActor))
		DebugAssertMsg(false, "피직스 액터 목록에서 액터를 제거하는데 실패했습니다.");
}

void SGActorBox::unregisterActor(SGActor* actor) {
	DebugAssertMsg(actor->getActorId() != InvalidValue_v, "올바르지 않은 액터 ID입니다.");
	if (!m_hActorMap.Remove(actor->getActorId()))
		DebugAssertMsg(false, "공통 액터 목록에서 액터를 제거하는데 실패했습니다.");
	else
		actor->setActorId(InvalidValue_v);
}


void SGActorBox::cleanUpProjectile(SGProjectile* projectile) {
	unregisterProjectile(projectile);
	unregisterZOrderActor(projectile);
	unregisterActor(projectile);
	m_hProjectilePool[projectile->getBaseInfo()->Code].PushBack(projectile);
	Log("삭제> 플레이어 프로젝틸 (%s), 남은 플레이어 프로젝틸 수 : %d, Z오더 액터 수: %d\n", projectile->getBaseInfo()->Name.Source(), m_vProjectiles.Size(), m_vZOrderedActors.Size());
}

void SGActorBox::cleanUpMonster(SGMonster* monster) {
	unregisterMonster(monster);
	unregisterZOrderActor(monster);
	unregisterPhysicsActor(monster);
	unregisterActor(monster);
	m_hMonsterPool[monster->getBaseInfo()->Code].PushBack(monster);
	Log("삭제> 몬스터 (%s), 남은 몬스터 수 : %d, Z오더 액터 수: %d\n", monster->getBaseInfo()->Name.Source(), m_vMonsters.Size(), m_vZOrderedActors.Size());
}

void SGActorBox::cleanUpObstacle(SGObstacle* obstacle) {
	unregisterObstacle(obstacle);
	unregisterActor(obstacle);

	if (obstacle->getBaseInfo()->Colliadalble) 
		unregisterColidableObstacle(obstacle);

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

